#include "card_abs.h"

sf::Color card_abs::selectClr(0,255,255,50), card_abs::destClr(155,0,155,50);

card_abs::card_abs(): Level()
{
    button::pOKtoRun = [this](){ return !animating; };// block ui use during animations

    for( size_t i=0; i<4; ++i )
    {
     //   selectBox[i].color = sf::Color(0,255,255,50);
        destBox[i].color = card_abs::destClr;
    }
}

bool card_abs::loadCards()
{
    // to card_abs
    std::ifstream fin("images/cards/initData.txt");

    if( !fin ) { std::cout << "\nNo card initData"; return false; }
    char suit;
    int value;
    std::string imageFname;

    if( fin >> imageFname && cardBackTxt.loadFromFile( imageFname.c_str() ) )
    {
        card::w = static_cast<float>( cardBackTxt.getSize().x );
        card::h = static_cast<float>( cardBackTxt.getSize().y );
        card::flare = card::h/20.0f;// prefer to read from file
        card::backSpr.setTexture( cardBackTxt );
        card::backSpr.setPosition( 1000.0f, 100.0f );
    }
    else return false;

    size_t numTextures = 0, NumCardSets = 0, cardCnt = 0;
    fin >> cardColMaxLength >> doSeedRand;
    fin >> numTextures >> NumCardSets;
    if( doSeedRand ) std::srand ( unsigned ( std::time(0) ) );
    textureVec.reserve(numTextures);
    cardSetVec.reserve(numTextures);

    cardSetVec.reserve(numTextures);
    while( fin >> suit >> value >> imageFname && cardCnt++ < numTextures )
    {
        textureVec.push_back( sf::Texture() );
        if( !textureVec.back().loadFromFile( imageFname.c_str() ) ) return false;

        card newCard( textureVec.back(), suit, value );
        newCard.faceUp = false;
        cardSetVec.push_back( newCard );
    }

    return true;
}

void card_abs::startLegAnimation( linLeg& rLeg, LegAdapter& rLA, card* pCard, sf::Vector2f begin_pos, sf::Vector2f end_pos, float v )
{
//    begin_pos = pCard->getPosition();
    rLeg.init( begin_pos.x, begin_pos.y, end_pos.x, end_pos.y );
    rLA.init( &rLeg, [pCard](float x,float y){ pCard->setPosition(x,y); }, v );
    rLA.init_ab( card_xa, card_v0, card_xb, card_vf );
    animating = true;
}

void card_abs::startSelectedCardMove( cardList* pSceList, cardList* pDestList, cardNode* pSceNode, bool doFlip )
{
    sf::Vector2f begin_pos = pSceNode->pCard->getPosition();
    sf::Vector2f end_pos;// = pNode->pCard->spr.getPosition();
    cardNode* it = pSceNode->next;// before assigned = nullptr next line
    sf::Vector2f mvListPos( pDestList->tail ? pDestList->tail->pCard->getPosition() : sf::Vector2f(pDestList->x0,pDestList->y0) );
    if( pDestList->tail ) mvListPos += pDestList->tail->pCard->is_FaceUp() ? sf::Vector2f(pDestList->dx,pDestList->dyFU) : sf::Vector2f(pDestList->dx,pDestList->dyFD);
    moveList.x0 = mvListPos.x; moveList.y0 = mvListPos.y;
    moveList.dx = pDestList->dx; moveList.dyFU = pDestList->dyFU;
    moveList.head = moveList.tail = nullptr;
    moveList.append_One( pSceList, pSceNode, end_pos );
    linLegVec[0].init( begin_pos.x, begin_pos.y, end_pos.x, end_pos.y );
    card* pCard = pSceNode->pCard;
    float vel = card_v1;
    if( linLegVec[0].len < Level::winW/5.0f ) vel *= 0.4f;
    else if( linLegVec[0].len < Level::winW/2.0f ) vel *= 0.7f;
    LegAdaptVec[0].init( &linLegVec[0], [pCard](float x,float y) { pCard->setPosition(x,y); }, vel );
    LegAdaptVec[0].init_ab( card_xa, card_v0, card_xb, card_vf );
    size_t LAidx = 1;
    float offY = pSceList->dyFU;

    while( it )
    {
        cardNode* itNext = it->next;
        moveList.append_One( pSceList, it, end_pos );
        pCard = it->pCard;
    //    p_SetPosition = std::bind( [pCard](float x,float y) { pCard->setPosition(x,y); }, std::placeholders::_1, std::placeholders::_2 );
    //    LegAdaptVec[LAidx].init( &linLegVec[0], p_SetPosition, vel, 0.0f, offY );
        LegAdaptVec[LAidx].init( &linLegVec[0], [pCard](float x,float y) { pCard->setPosition(x,y); }, vel, 0.0f, offY );
        LegAdaptVec[LAidx].init_ab( card_xa, card_v0, card_xb, card_vf );
        it = itNext;
        ++LAidx;
        offY += pDestList->dyFU;
    }

    animating = true;
//    animateID = 3;
    pSceList->checkLength();

    // card flip
 //   if( pSceList->tail && !pSceList->tail->pCard->faceUp )
    if( doFlip && pSceList->tail && !pSceList->tail->pCard->faceUp )
    {
        pSceList->pFlipCard = pSceList->tail->pCard;
        pSceList->pFlipCard->initFlip(0.0f);
    }

}// end of startSelectedCardMove

void card_abs::moveOneCard_event( std::vector<cardList>& rCLvec )
{
    for( cardList& rCL : rCLvec )
    {
        cardNode* pNodeHit = nullptr;
        if( rCL.hit( pNodeHit, button::mseX, button::mseY ) )
        {
            if( button::clickEvent_Lt() == 1 )// sce chosen
            {
                if( dragDrop )// grab every time
                {
                    pListSce = &rCL;
                    pListSce->pNodeSel = pNodeHit;
               //     pNodeSce = pNodeHit;
                    sf::Vector2f msePos( button::mseX, button::mseY );
                //    grabOfst = msePos - pNodeSce->pCard->getPosition();
                    grabOfst = msePos - pNodeHit->pCard->getPosition();
                }
                else// 2 click move
                {
              //      if( pNodeSce && pListSce )// selecting dest - 2nd click
                    if( pListSce && pListSce->pNodeSel )// selecting dest - 2nd click
                    {
                        pListDest = &rCL;
                        pNodeDest = pNodeHit;// dm
                        // animate!
                        if( linLegVec.empty() ) linLegVec.push_back( linLeg() );
                        if( LegAdaptVec.empty() ) LegAdaptVec.push_back( LegAdapter() );
                        card* pCard = pListSce->pNodeSel->pCard;
                    //    sf::Vector2f begin_pos = pNodeSce->pCard->getPosition(), end_pos = pNodeDest->pCard->getPosition();
                        sf::Vector2f begin_pos = pCard->getPosition(), end_pos = pNodeDest->pCard->getPosition();

                        moveList.x0 = begin_pos.x; moveList.y0 = begin_pos.y;
                        end_pos.y += pNodeDest->pCard->is_FaceUp() ? pListDest->dyFU/2.0f : pListDest->dyFD/2.0f;//18.0f;
                      //  startLegAnimation( linLegVec[0], LegAdaptVec[0], pNodeSce->pCard, pNodeSce->pCard->getPosition(), end_pos, card_v1 );
                        startLegAnimation( linLegVec[0], LegAdaptVec[0], pCard, pCard->getPosition(), end_pos, card_v1 );
                      //  pListDest->insert_after( pNodeHit, pListSce, pNodeSce, end_pos );
                        pListDest->insert_after( pNodeHit, pListSce, pListSce->pNodeSel, end_pos );
                        pListDest->setSelectedNode( pNodeHit, false, destClr );// new
                    //    pNodeSce = nullptr;// onlt remaining static local
                    }
                    else// selecting sce - 1st click
                    {
                        pListSce = &rCL;
                     //   pNodeSce = pNodeHit;
                        pListSce->setSelectedNode( pNodeHit, false, selectClr );// new
                    }
                }
            }
            else// dest is chosen
            {
                if( dragDrop )// release to dest
                {
                  //  if( pListSce && pNodeSce )
                    if( pListSce && pListSce->pNodeSel )
                    {
                      //  if( pNodeHit == pNodeSce )// perhaps another toward head
                        if( pNodeHit == pListSce->pNodeSel )// perhaps another toward head
                        {
                            cardNode* it = pNodeHit->prev;
                            while( it && !it->pCard->hit(button::mseX, button::mseY) ) it = it->prev;
                            if( it ) pNodeHit = it;// another found
                            else continue;// next list
                        }

                        pListDest = &rCL;
                        sf::Vector2f end_pos;
                       // pListDest->insert_after( pNodeHit, pListSce, pNodeSce, end_pos );
                        pListDest->insert_after( pNodeHit, pListSce, pListSce->pNodeSel, end_pos );
                  //      pListDest->setSelectedNode( pNodeHit, destBox, false );// new
                   //     pListDest->pNodeSel = pNodeHit;
                        pListSce->setCardSpacing();
                        pListDest->setCardSpacing();
                     //   pNodeSce = nullptr;// onlt remaining static local
                        pListDest->pNodeSel = nullptr; pListDest = nullptr;
                        pListSce->pNodeSel = nullptr; pListSce = nullptr;
                    }
                }
             //   else if( pNodeSce && pNodeSce != pNodeHit )// release of sce node
             //       pNodeSce = nullptr;

                else if( pListSce && pListSce->pNodeSel != pNodeHit )// release of sce node
                    pListSce->pNodeSel = nullptr;
            }

            break;
        }
    }

    // drag drop release on no list
    if( dragDrop && pListSce && button::clickEvent_Lt() == -1 )
    {
        pListSce->setCardSpacing();
        pListSce->pNodeSel = nullptr; pListSce = nullptr;
    }

}

void card_abs::moveOneCard_finish()
{
    if( pListDest && pListSce )// && moveList.tail )// move complete
    {
        pListDest->setCardSpacing(); pListDest->pNodeSel = nullptr; pListDest = nullptr; std::cout << "\nappend";
        pListSce->setCardSpacing(); pListSce->pNodeSel = nullptr; pListSce = nullptr;
    //    pNodeDest = nullptr;// unused here yet
    }
}

void card_abs::moveOneCard_update()
{
//    if( dragDrop && pNodeSce )
    if( dragDrop && pListSce && pListSce->pNodeSel )
    {
        sf::Vector2f msePos( button::mseX, button::mseY );
    //    pNodeSce->pCard->setPosition( msePos - grabOfst );
        pListSce->pNodeSel->pCard->setPosition( msePos - grabOfst );
    }
}

void card_abs::updateAnimations( std::vector<cardList>& rCLvec, float dt )
{
    moveOneCard_update();
    updateDeal( rCLvec, dt );
    if( !( animating || card::anyFlipping ) ) return;

    if( card::anyFlipping )
    {
        card::anyFlipping = false;
        for( auto& rCL : rCLvec )
            rCL.flip_update();
    }

    if( animating )
    {
        animating = false;
        for( LegAdapter& LA : LegAdaptVec )
        {
            LA.update(dt);
            if( LA.pLeg ) animating = true;
        }

        if( !animating )
        {
            std::cout << "\nanimation done";
            if( animateID == 0 )
            {
                moveOneCard_finish();
                for( auto& rCL : rCLvec ) rCL.pNodeSel = nullptr;
            }
            else if( pEndAniFunc ) pEndAniFunc();
            else
            {
                animateID = 0;// all animation follow actions handled
                if( pListSce ) { pListSce->pNodeSel = nullptr; pListSce = nullptr; }
                if( pListDest ) { pListDest->pNodeSel = nullptr; pListDest = nullptr; }
            }
        }
    }// end if animating

    /*
    if( card::anyFlipping )
    {
        card::anyFlipping = false;
        for( auto& rCL : rCLvec )
            rCL.flip_update();
    }   */
}

void card_abs::updateDeal( std::vector<cardList>& rCLvec, float dt )
{
    if( dealID == 0 ) return;

 //   static int dealID_last = 0;
    static float deal_dt = 3.0f, deal_tm = 0.0f;
    static float deal_v = 10.0f;
    static size_t coLidx = 0;
    static size_t LAidx = 0;

    static std::vector<size_t> fdVec( rCLvec.size() ), fuVec( rCLvec.size() );

    // deal begins
    if( dealID_last != dealID )
    {
        std::cout << "\ndeal begins for dealID = " << dealID;
    //    linLegVec.resize( rCLvec.size() );
        size_t numCards = 0;
        std::string fname;
        if( dealID == 1 ) fname = deal1_fname;
        else if( dealID == 2 ) fname = deal2_fname;
        else { std::cout << "\nbad dealID = " << dealID << '\n'; return; }
        std::ifstream fin( fname.c_str() );
        if( !fin ) { std::cout << "\nbad fname = " << fname; return; }
        // read deal data
        fuVec.clear(); fdVec.clear();
        //fin >> deal_dt >> deal_x0 >> deal_y0 >> deal_v;
        fin >> deal_dt >> deal_v;
        for( auto& rCL : rCLvec )
        {
            size_t n;
            fin >> n; fdVec.push_back(n); numCards += n; std::cout << ' ' << n;
            fin >> n; fuVec.push_back(n); numCards += n; std::cout << ' ' << n;
        }

        std::cout << "\nnumCards = " << numCards;
        if( LegAdaptVec.size() < numCards ) LegAdaptVec.resize( numCards );
        if( linLegVec.size() < numCards ) linLegVec.resize( numCards );
        dealID_last = dealID;
        deal_tm = 0.0f;
        coLidx = 0;
        LAidx = 0;
        std::cout << std::endl;
        fin.close();
    }

    deal_tm += dt;
    if( deal_tm >= deal_dt )// deal a card
    {
    //    std::cout << "\ndeal a card";
    //    static size_t LAidx = 0;
        if( !drawList.tail )// no cards left
        {
            // reset statics
            std::cout << "\nout of cards";
            if( dealID == 1 && pEndDealFunc_1 ) pEndDealFunc_1();
            if( dealID == 2 && pEndDealFunc_2 ) pEndDealFunc_2();
            coLidx = 0;
            dealID_last = dealID = 0;
            LAidx = 0;
            return;
        }

        int deal_code = 0;// -1=fd, 0=no deal, +1=fu deal
        size_t coLidx_0 = coLidx;

        while( true )
        {
            if( fdVec[coLidx] > 0 ) { deal_code = -1; --fdVec[coLidx]; }// deal fd
            else if( fuVec[coLidx] > 0 ) { deal_code = 1; --fuVec[coLidx]; }// deal fu
            if( deal_code == 0 )
            {
                coLidx = (1 + coLidx)%rCLvec.size();
           //     std::cout << "\ndealing... coLidx = " << coLidx;
                if( coLidx == coLidx_0 )// went full circle. Deal is done
                {
                    // call deal end function here
                    if( dealID == 1 && pEndDealFunc_1 ) pEndDealFunc_1();
                    if( dealID == 2 && pEndDealFunc_2 ) pEndDealFunc_2();
                    // reset statics
                    std::cout << "\ndeal finished. coLidx = " << coLidx;
                    coLidx = 0;
                    dealID_last = dealID = 0;
                    LAidx = 0;

                    return;
                }
            }
            else break;
        }

        sf::Vector2f pos;
        rCLvec[coLidx].append_One( &drawList, drawList.tail, pos );
        card* pCard = rCLvec[coLidx].tail->pCard;
     //   sf::Vector2f curr_pos = pCard->getPosition();
        sf::Vector2f curr_pos( drawList.x0, drawList.y0 );
    //    pCard->faceUp = deal_code == 1;
        if( pCard->faceUp != (deal_code == 1) ) { pCard->initFlip(0.0f); rCLvec[coLidx].pFlipCard = pCard; }
        startLegAnimation( linLegVec[LAidx], LegAdaptVec[LAidx], pCard, curr_pos, pos, deal_v );
        ++LAidx;
        coLidx = (1 + coLidx)%rCLvec.size();
        deal_tm = 0.0f;
    }
}

void card_abs::startCardListMove( cardList& CL, sf::Vector2f disp, linLeg& rLeg, LegAdapter& LA, float v )// displacement from existing position
{
    rLeg.init( CL.x0, CL.y0, CL.x0 + disp.x, CL.y0 + disp.y );
    cardList* pCL = &CL;
    LA.init( &rLeg, [pCL](float x,float y) { pCL->setPosition(x,y); }, v );
    LA.init_ab( card_xa, card_v0, card_xb, card_vf );
 //   cardNode* it = CL.head;
 /*   while( it )
    {
        card* pCard =it->pCard;
        std::function<void(float,float)> p_SetPosition = [pCard](float x,float y) { pCard->setPosition(x,y); };
        LA.init( &rLeg, p_SetPosition, card_v1/2.0f, 0.0f, pCard->getPosition().y - CL.y0 );
        LA.init_ab( card_xa, card_v0, card_xb, card_vf );
        it = it->next;
    }   */

    animating = true;
}

// non members
void makeBox( sf::Vertex* pBox, cardNode& rNodeTop, cardNode& rNodeBottom )
{
    sf::Vector2f posTopLt = rNodeTop.pCard->getPosition();
    sf::Vector2f posBottRt = rNodeBottom.pCard->getPosition() + sf::Vector2f( card::w, card::h );
    pBox[0].position = posTopLt;
    pBox[1].position.x = posBottRt.x;
    pBox[1].position.y = posTopLt.y;
    pBox[2].position = posBottRt;
    pBox[3].position.x = posTopLt.x;
    pBox[3].position.y = posBottRt.y;
    pBox[4].position = pBox[0].position;
}
