#include "spiderSolitaire.h"

bool spiderSolitaire::init()
{
    drawPos.x = Level::winW-150.0f;
    drawPos.y = Level::winH-150.0f;
    completePos.x = 50.0f;
    completePos.y = Level::winH-150.0f;
    Level::clearColor = sf::Color(0,64,128);
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;

    deal1_fname = "include/level/spiSol_data/deal1_data.txt";
    deal2_fname = "include/level/spiSol_data/deal2_data.txt";

    pEndDealFunc_1 = [this](){ to_SF_string( drawMsgNum, drawList.numCards()/numCols ); };
    pEndDealFunc_2 = [this]()
    {
        if( drawList.numCards() > 0 ) to_SF_string( drawMsgNum, drawList.numCards()/numCols );
         else drawMsgNum.setString("");// dont display 0
    };

    // size the vectors
    numLegs = 104;
    linLegVec.resize(numLegs);
    LegAdaptVec.resize(numLegs);
    cardCol.resize(numCols);

    // control stuff
    Level::goto_MMButt.setPosition( {Level::winW/2-40.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW/2+40.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    sf::Text label("Quit", *button::pFont, 14);
    label.setColor( sf::Color::Black );
//    quitButt.init( Level::winW/2+40.0f, 10.0F, 60.0f,30.0f, [this](){ Quit = true; }, label );// quitButt.mode = 'I';
 //   button::RegisteredButtVec.push_back( &quitButt );
    label.setString("Undo");
    undoButt.init( 30.0f, 200.0f, 60.0f, 30.0f, [this](){ undoLastMove(); }, label ); undoButt.mode = 'I';
    button::RegisteredButtVec.push_back( &undoButt );
    label.setCharacterSize( 10 );
    label.setString("New game");
    newGameButt.init( 30.0f, 10.0f, 60.0f, 30.0f, [this](){ dealID = 0; startUpColAnimation(); animateID = 5; }, label ); newGameButt.mode = 'I';
    button::RegisteredButtVec.push_back( &newGameButt );
    label.setString("number of suits");
    suitSelect.init( 15.0f, 45.0f, 90.0f, 20.0f, 3, label, 0 );
    suitSelect.pFuncIdx = std::bind( [this](size_t i) { if(i==0) numSuits = 1; else if(i==1) numSuits = 2; else if(i==2) numSuits = 4; }, std::placeholders::_1 );
    button::RegisteredButtVec.push_back( &suitSelect );
    label.setString("Save game");
    saveGameButt.init( 30.0f, 150.0f, 60.0f, 30.0f, [this](){ saveGame("savedGame.txt"); }, label ); saveGameButt.mode = 'I';
    button::RegisteredButtVec.push_back( &saveGameButt );
    label.setString("Ld Sv game");
    pRBss = &suitSelect;
    LdSvGameButt.init( 30.0f, 100.0f, 60.0f, 30.0f, [this](){ dealID = 0; startUpColAnimation(); animateID = 6; }, label ); LdSvGameButt.mode = 'I';
    button::RegisteredButtVec.push_back( &LdSvGameButt );

    label.setColor(sf::Color::Red);
    label.setCharacterSize( 20 );
    label.setString("SCORE");
    scoreMsg = scoreMsgNum = label;
    scoreMsg.setPosition( 1000.0f, 20.0f );
    scoreMsgNum.setPosition( 1100.0f, 20.0f );
    label.setString("?");
    drawMsgNum = label;
    drawMsgNum.setPosition( drawPos.x - 30.0f, drawPos.y + 80.0f );
    label.setString("");
    completeMsgNum = label;
    completeMsgNum.setPosition( completePos.x - 30.0f, completePos.y + 80.0f );
    label.setString("You Win!");
    winMsg = label;
    winMsg.setPosition( Level::winW/2.0f, Level::winH/2.0f );
    winMsg.setCharacterSize( 30 );
    winMsg.setColor(sf::Color::Green);

    if( !loadFirstGame() ) return false;
    if( !loadFireWorks() ) return false;

    label.setString("Flip Flare");
    flareStrip.init( 350.0f, 680.0f, 140.0f, 20.0f, label, 0.0f, 0.1f, 0.05f, [this](float x){ card::flare = card::h*x; } );
    button::RegisteredButtVec.push_back( &flareStrip );

    return true;
}

bool spiderSolitaire::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::H )
        {
            int N = findMoves();// for move hints
            std::cout << "\nfound " << N << " moves.";
        }
    }

    return true;
}

void spiderSolitaire::handleCardHit()// READY
{
    if( button::clickEvent_Lt() != 1 ) return;// no left click

    bool found = false;

    for( cardList& rCL : cardCol )
    {
        cardNode* pNodeHit = nullptr;
        if( rCL.hit( pNodeHit, button::mseX, button::mseY ) )
        {
        //    if( pListSce && pNodeSce )
            if( pListSce && pListSce->pNodeSel )
            {
                pListDest = &rCL;
                found = true;
            }
            else if( pNodeHit )
            {
            //    if( pNodeHit->pCard->faceUp )
                if( pNodeHit->pCard->faceUp && isValidMove( nullptr, *pNodeHit ) )
                {
                    pListSce = &rCL;
                    pListSce->setSelectedNode( pNodeHit, true, card_abs::selectClr );
               //     pNodeSce = pNodeHit;
               //     makeBox( selectBox, *pNodeSce, *rCL.tail );
               //     isSelection = true;
                }
                else if( pNodeHit == rCL.tail )// flip end card
                    pNodeHit->pCard->faceUp = true;

                found = true;
            }
        }
       if( found ) break;
    }

    if( !found && drawList.tail )// hit the drawList?
    {
        if( drawList.tail->pCard->hit( button::mseX, button::mseY ) )
        {
            dealID = 2;
        //    dealingLayer = true;
            animateID = 2;
            sceListHistory.clear();
            destListHistory.clear();
            sceNodeHistory.clear();
        }
    }

    // process hit
    if( pListDest )
    {
     //   if( isValidMove( pListDest->tail, *pNodeSce ) )
        if( isValidMove( pListDest->tail, *pListSce->pNodeSel ) )
        {
        //    startSelectedCardMove( pListSce, pListDest, pNodeSce );
            startSelectedCardMove( pListSce, pListDest, pListSce->pNodeSel );
            animateID = 3;
            to_SF_string( scoreMsgNum, --score );
            if( pListSce->tail && !pListSce->tail->pCard->faceUp ) flipHistory.push_back(true);
            else flipHistory.push_back(false);

            sceListHistory.push_back( pListSce );
            destListHistory.push_back( pListDest );
         //   sceNodeHistory.push_back( pNodeSce );
            sceNodeHistory.push_back( pListSce->pNodeSel );

        }
        else pListDest = nullptr;

        if( pListSce ) pListSce->pNodeSel = nullptr;
        pListSce = nullptr;
     //   pNodeSce = nullptr;
     //   pListDest = nullptr;
     //   isSelection = false;// at animation end insttead?
    }

    return;
}// end of handleCardHit()

void spiderSolitaire::update( float dt )
{
    if( !( animating || button::pButtMse ) ) handleCardHit();

    updateDeal( cardCol, dt );

    if( gameWon )
    {
        static std::default_random_engine rand_gen;
        static std::uniform_real_distribution<float> pos_dist(-1.0f,1.0f);
        if( !fw1.pShell ) { fw1.firePos = fwPos1 + fireOfst*pos_dist(rand_gen); fw1.doFire = true; }
        if( !fw2.pShell ) { fw2.firePos = fwPos2 + fireOfst*pos_dist(rand_gen); fw2.doFire = true; }
        if( !fw3.pShell ) { fw3.firePos = fwPos3 + fireOfst*pos_dist(rand_gen); fw3.doFire = true; }
        fw1.update(dt);
        fw2.update(dt);
        fw3.update(dt);
        updateFireworks( dt );
    }


 //   bool isNeed = dealingAll || dealingLayer || animating || card::anyFlipping;
    bool isNeed = dealID != 0 || animating || card::anyFlipping;
    isNeed = isNeed || ( pListSce && pListSce->pNodeSel );
    if( !isNeed ) return;

    if( animating )
    {
        animating = false;
        const float setMoveVel = 0.7f*card_v1;
        for( LegAdapter& LA : LegAdaptVec )
        {
            const Leg* pLegPrev = LA.pLeg;
            LA.update(dt);
            if( LA.pLeg )
            {
                animating = true;
                if( LA.pLeg != pLegPrev )// for completeList 2 Leg animation
                {
                    LA.v = LA.v1 = setMoveVel;// change v on Leg change
                    LA.init_ab( card_xa, card_v0, card_xb, card_vf );
                }
            }
        }

        if( !animating )
        {
            movingSet = false;// may now draw just completeList.tail->pCard
            drawDestBox = false;
            if( pListDest && moveList.tail )// move complete
            {
                pListDest->append_toEnd( &moveList, moveList.head );// ok almost always

                if( pListDest->checkLength() ) std::cout << "\nLength change!";
                if( pListDest == &completeList ) to_SF_string( completeMsgNum, completeList.numCards()/13 );
                pListDest = nullptr;
            }

            // check if animation completed any sets in the cardCols
            if( animateID > 1 && animateID < 5 )// 1=dealAll, 2=dealLayer, 3=moveSet, 4=moveComplete
            {
                gameWon = true;
                for( cardList& rCL : cardCol )
                {
                    if( rCL.tail ) gameWon = false;// not yet

                    cardNode* pKing = nullptr;
                    if( rCL.endIsComplete( pKing ) )
                    {
                        sceListHistory.clear();
                        destListHistory.clear();
                        sceNodeHistory.clear();
                        flipHistory.clear();

                        movingSet = true;
                        pListDest = &completeList;
                        animateID = 4;
                        startCompleteCardMove( &rCL, setMoveVel );
                        rCL.checkLength();
                        score += 100;
                        to_SF_string( scoreMsgNum, score );
                        return;
                    }

                    // compress?
                    if( animateID == 2 ) rCL.checkLength();// following dealLayer
                }
            }

            if( animateID == 5 ) makeNewGame();// pre newGame ani ended
            if( animateID == 6 ) loadSavedGame("savedGame.txt");// pre saveGame ani ended

            animateID = 0;// all animation follow actions handled
        }// end if !animating
    }// end if animating
    else if( pListSce && pListSce->pNodeSel )//( pNodeSce )// draw destBox on potential card selections during mouse over
    {
        cardList* listHit = nullptr;
        cardNode* pNodeHit = nullptr;// just need for argument below

     /*   for( cardList& rCL : cardCol )
            if( rCL.hit( pNodeHit, button::mseX, button::mseY ) && rCL.tail && rCL.tail->pCard->face == pListSce->pNodeSel->pCard->face + 1 )
            {
                pNodeHit = rCL.tail;
                while( pNodeHit->prev && pNodeHit->prev->pCard->faceUp && pNodeHit->pCard->face + 1 == pNodeHit->prev->pCard->face )
                    pNodeHit = pNodeHit->prev;

                makeBox( destBox, *pNodeHit, *rCL.tail );
                drawDestBox = true;
                listHit = &rCL;
                break;
            }   */

        for( cardList& rCL : cardCol )
            if( rCL.hit( pNodeHit, button::mseX, button::mseY ) && rCL.endIsStraight( pNodeHit, pListSce->pNodeSel->pCard->face ) )
            {
                makeBox( destBox, *pNodeHit, *rCL.tail );
           //     makeBox( rCL.box, *pNodeHit, *rCL.tail );
                drawDestBox = true;
                listHit = &rCL;
                break;
            }


        if( !listHit ) drawDestBox = false;
    }

    if( card::anyFlipping )
    {
        card::anyFlipping = false;
        for( auto& rCL : cardCol )
        {
            if( rCL.pFlipCard )
            {
                rCL.pFlipCard->continueFlip();
                if( rCL.pFlipCard->flipState == 0 ) rCL.pFlipCard = nullptr;
                else card::anyFlipping = true;
            }
        }
    }

    return;
}

void spiderSolitaire::draw( sf::RenderTarget& RT ) const
{
        if( drawList.tail ) drawList.tail->pCard->draw(RT);
        for( const cardList& cl : cardCol ) cl.draw(RT);

        if( completeList.tail ) completeList.tail->pCard->draw(RT);
        if( moveList.tail ) moveList.draw(RT);

        if( drawDestBox ) RT.draw( destBox, 4, sf::Quads );
        RT.draw( scoreMsg );
        RT.draw( scoreMsgNum );
        RT.draw( completeMsgNum );
        RT.draw( drawMsgNum );
        if( gameWon ) RT.draw( winMsg );
        if( anyLive_emit ) for( const spark& spk : sparkVec ) spk.draw( RT, &dotSS.txt );
}

void spiderSolitaire::cleanup()
{
    return;
}

bool spiderSolitaire::undoLastMove()
{
    if( animating ) return false;

    if( destListHistory.size() > 0 && sceListHistory.size() > 0 && sceNodeHistory.size() > 0 )
    {
        cardList* pSceList = sceListHistory.back();
        pListDest = sceListHistory.back();
        if( pSceList->tail && flipHistory.back() )// pSceList->tail->pCard->faceUp = false;
        {
        pSceList->pFlipCard = pSceList->tail->pCard;
        pSceList->tail->pCard->initFlip( pSceList->dyFD - pSceList->dyFU );
        pSceList->tail->pCard->setPosition( pSceList->getNodePosition( *pSceList->tail ) );

        }

        startSelectedCardMove( destListHistory.back(), pSceList, sceNodeHistory.back() );//, !flipHistory.back() );
        sceListHistory.pop_back();
        destListHistory.pop_back();
        sceNodeHistory.pop_back();
        flipHistory.pop_back();

        return true;
    }
    return false;
}

void spiderSolitaire::startCompleteCardMove( cardList* pSceList, float v0 )
{
    cardNode* pKing = nullptr;
    if( !pSceList->endIsComplete( pKing ) ) return;// no complete set

    sf::Vector2f pos1 = pKing->pCard->getPosition();
    sf::Vector2f pos2 = pSceList->tail->pCard->getPosition();
    pos2.y += 20.0f;
    triLeg[0].init( pos1.x, pos1.y, pos2.x, pos2.y );
    triLeg[1].init( pos2.x, pos2.y, completeList.x0, completeList.y0 );
    triLeg[0].next = triLeg+1;
    triLeg[1].prev = triLeg;
    float len0 = triLeg[0].len;

    cardNode* it = pKing;
    size_t LAidx = 0;

    while( it )
    {
        cardNode* itNext = it->next;
        sf::Vector2f end_pos;
        moveList.append_One( pSceList, it, end_pos );
        card* pCard = it->pCard;
        end_pos = pCard->getPosition();
        std::function<void(float,float)> p_SetPosition = std::bind( [pCard](float x,float y) { pCard->setPosition(x,y); }, std::placeholders::_1, std::placeholders::_2 );
        LegAdaptVec[LAidx].init( triLeg, p_SetPosition, v0, 0.0f, 0.0f );

        float len1  = sqrtf( (end_pos.x-pos2.x)*(end_pos.x-pos2.x) + (end_pos.y-pos2.y)*(end_pos.y-pos2.y) );
        LegAdaptVec[LAidx].s = len0 - len1;
        pCard->setPosition( triLeg[0].x(len0-len1), triLeg[0].y(len0-len1) );
        LegAdaptVec[LAidx].v *= len1/len0;
        LegAdaptVec[LAidx].v1 = LegAdaptVec[LAidx].v;
    //    std::cout << dealLegAdapt[LAidx].s << ' ';
        it = itNext;
        ++LAidx;
    }

    if( pSceList->tail && !pSceList->tail->pCard->faceUp )
    {
        pSceList->pFlipCard = pSceList->tail->pCard;
        pSceList->pFlipCard->initFlip(0.0f);
    }

    animating = true;
    animateID = 4;
}


void spiderSolitaire::startUpColAnimation()// game ending ani
{
    sf::Vector2f disp( 0.0f, -Level::winH );
    for( size_t i = 0; i < numCols; ++i )
        startCardListMove( cardCol[i], disp, linLegVec[i], LegAdaptVec[i], card_v1/2.0f );

    animating = true;
}

void spiderSolitaire::startDownColAnimation()// game ending ani
{
    sf::Vector2f disp( 0.0f, Level::winH );
    for( size_t i = 0; i < numCols; ++i )
    {
     //   cardCol[i].setPosition( cardCol[i].x0, card_y0 - Level::winH );// move to start position
        cardCol[i].y0 = card_y0 - Level::winH;
        startCardListMove( cardCol[i], disp, linLegVec[i], LegAdaptVec[i], card_v1/2.0f );
    }

    animateID = 7;// begin saved game
    animating = true;
}

/*
void spiderSolitaire::startUpColAnimation()// game ending ani
{
    size_t idxLA = 0;// LegAdaptor index
    for( size_t i = 0; i < numCols; ++i )
    {
        float fposY = card_y0 - (float)Level::winH;
        linLegVec[i].init( cardCol[i].x0, cardCol[i].y0, cardCol[i].x0, fposY );

        cardNode* it = cardCol[i].head;
        while( it )
        {
            card* pCard =it->pCard;
            std::function<void(float,float)> p_SetPosition = std::bind( [pCard](float x,float y) { pCard->setPosition(x,y); }, std::placeholders::_1, std::placeholders::_2 );
            LegAdaptVec[idxLA].init( &linLegVec[i], p_SetPosition, card_v1/2.0f, 0.0f, pCard->getPosition().y - cardCol[i].y0 );
            LegAdaptVec[idxLA++].init_ab( card_xa, card_v0, card_xb, card_vf );
            it = it->next;
        }
    }

    animating = true;
}
*/
/*
void spiderSolitaire::startDownColAnimation()
{
    size_t idxLA = 0;// LegAdaptor index
    for( size_t i = 0; i < numCols; ++i )
    {
        float iposY = card_y0 - Level::winH;
        cardCol[i].setPosition( cardCol[i].x0, iposY );

        linLegVec[i].init( cardCol[i].x0, iposY, cardCol[i].x0, card_y0 );

        cardNode* it = cardCol[i].head;
        while( it )
        {
            card* pCard = it->pCard;
            std::function<void(float,float)> p_SetPosition = [pCard](float x,float y) { pCard->setPosition(x,y); };
            LegAdaptVec[idxLA].init( &linLegVec[i], p_SetPosition, card_v1/2.0f, 0.0f, pCard->getPosition().y - iposY );
            LegAdaptVec[idxLA++].init_ab( card_xa, card_v0, card_xb, card_vf );
            it = it->next;
        }

        cardCol[i].y0 = card_y0;
    }

    animating = true;
    animateID = 7;// begin saved game
}
*/

bool spiderSolitaire::loadFirstGame()
{
    if( textureVec.size() == 0 && !loadCards() ) return false;
    // SS specific
    numSuits = 1;
    cardVec.reserve(2*cardSetVec.size());
    for( size_t i=0; i<8; ++i )
        for( size_t j=0; j<cardSetVec.size()/4; ++j )
            cardVec.push_back( cardSetVec[j] );

    // make drawList
    completeList.setBasePosition( completePos.x, completePos.y, 0.0f, 0.0f, 0.0f );
    moveList.setBasePosition( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
    drawList.setBasePosition( drawPos.x, drawPos.y, 0.0f, 0.0f, 0.0f );
    for( size_t i=0; i<numCols; ++i )
    {
        cardCol[i].setBasePosition( card_x0+i*card_dx, card_y0, 0.0f, card_dy, card_dyFD );
        cardCol[i].maxLength = cardColMaxLength;
    }

    makeNewGame();

    return true;
}

void spiderSolitaire::renew_cardVec( bool go )
{
    sceListHistory.clear();
    destListHistory.clear();
    sceNodeHistory.clear();
    flipHistory.clear();

    static size_t numSuitsLast = numSuits;
    if( go || ( numSuitsLast != numSuits ) )// make new cardVec
    {
        cardVec.clear();
        cardVec.reserve( 2*textureVec.size() );

        switch( numSuits )
        {
            case 1:// x8 sets
            for(size_t i=0; i<8; ++i)
                for( size_t j=0; j<cardSetVec.size()/4; ++j ) cardVec.push_back( cardSetVec[j] );
            break;
            case 2:// x4 sets
            for(size_t i=0; i<4; ++i)
                for( size_t j=0; j<cardSetVec.size()/2; ++j ) cardVec.push_back( cardSetVec[j] );
            break;
            case 4:// x2 sets
            for(size_t i=0; i<2; ++i)
                for( size_t j=0; j<cardSetVec.size(); ++j ) cardVec.push_back( cardSetVec[j] );
            break;
        }

        numSuitsLast = numSuits;
    }

    completeList.head = completeList.tail = nullptr;
    for( size_t i=0; i<numCols; ++i )
    {
        cardCol[i].head = cardCol[i].tail = nullptr;
        cardCol[i].isFullLength = true;
        cardCol[i].dyFU = cardCol[i].dyFU_full;
        cardCol[i].dyFD = cardCol[i].dyFD_full;
    }
    to_SF_string( scoreMsgNum, score );
    completeMsgNum.setString("");
    drawMsgNum.setString("");
}

void spiderSolitaire::makeNewGame()
{
    score = 500;
    gameWon = false;
    for( spark& spk : sparkVec ) spk.tLife = -1.0f;
    anyLive_emit = false;
    dealID_last = 0;
    fw1.doFire = fw2.doFire = fw3.doFire = false;
    fw1.pShell = fw2.pShell = fw3.pShell = nullptr;

    renew_cardVec(false);
    std::random_shuffle( cardVec.begin(), cardVec.end() );

    cardNodeVec.clear();
    cardNodeVec.reserve( cardVec.size() );
    for( card& rCard : cardVec ) cardNodeVec.push_back( cardNode( &rCard, nullptr, nullptr ) );
    cardNodeVec.back().next  = nullptr;

    std::vector<cardNode>::iterator it = cardNodeVec.begin();
    drawList.head = &cardNodeVec.front();
    drawList.tail = nullptr;

    while( it != cardNodeVec.end() )
    {
         drawList.push_back( *it );
         it->pCard->faceUp = false;
        ++it;
    }
    drawList.tail->next = nullptr;

    float fi = 0.0f;
    for( auto& CL : cardCol )
    {
        CL.x0 = card_x0 + fi*card_dx;
        CL.y0 = card_y0;
        fi += 1.0f;
    }

    dealID = 1;
 //   dealingAll = true;
    animateID = 1;

    return;
}

void spiderSolitaire::loadSavedGame( const char* fileName )
{
    std::ifstream fin( fileName );
    if( !fin ) return;

    gameWon = false;
    for( spark& spk : sparkVec ) spk.tLife = -1.0f;
    anyLive_emit = false;
    dealID_last = 0;
    fw1.doFire = fw2.doFire = fw3.doFire = false;
    fw1.pShell = fw2.pShell = fw3.pShell = nullptr;

    size_t num_cols = 0;
    fin >> numSuits >> score >> num_cols;
    renew_cardVec(true);// new deck even if same numSuits
    cardNodeVec.clear();
    cardNodeVec.reserve( cardVec.size() );
    for( card& rCard : cardVec ) cardNodeVec.push_back( cardNode( &rCard, nullptr, nullptr ) );
    cardNodeVec.back().next  = nullptr;

    std::vector<bool> useVec( cardVec.size() );// all false

    for( auto& CL : cardCol )
    {
        size_t numFD, numFU;
        fin >> numFD >> numFU;
        // find and push_back the nodes
        for( size_t i=0; i<numFD+numFU; ++i )
        {
            char suit; size_t face;
            fin >> suit >> face;
            size_t idx = face - 1;
            while( idx < cardNodeVec.size() )
            {
                if( cardNodeVec[idx].pCard->suit == suit && !useVec[idx] )
                {
                    useVec[idx] = true;
                    if( i >= numFD ) cardNodeVec[idx].pCard->faceUp = true;
                    CL.push_back( cardNodeVec[idx] );
                    break;
                }
                idx += 13;
            }
        }

        CL.checkLength();
    }

    // make drawList
    drawList.head = drawList.tail = nullptr;
    size_t numDrawCards;
    fin >> numDrawCards;
    to_SF_string( drawMsgNum, numDrawCards/numCols );
    for( size_t i=0; i<numDrawCards; ++i )
    {
        char suit; size_t face;
        fin >> suit >> face;
        size_t idx = face - 1;
        while( idx < cardNodeVec.size() )
        {
            if( cardNodeVec[idx].pCard->suit == suit && !useVec[idx] )
            {
                useVec[idx] = true;
                drawList.push_back( cardNodeVec[idx] );
                break;
            }
            idx += 13;
        }
    }

    // all remaining cards in the completeList
    completeList.head = completeList.tail = nullptr;
    size_t n = 0;
    for( size_t i=0; i<cardNodeVec.size(); ++i )
        if( !useVec[i] )
        {
            cardNodeVec[i].pCard->faceUp = true;
            completeList.push_back( cardNodeVec[i] );
            useVec[i] = true;
            ++n;
        }

    to_SF_string( completeMsgNum, n/13 );
    // ANIMATE CARDcOL ENTRY FROM ABOVE
    startDownColAnimation();
    if( pRBss ) pRBss->setIndex(numSuits-1);

    return;
}

bool spiderSolitaire::loadFireWorks()
{
    std::ifstream fin( "images/dots9_SSdata.txt" );
    if( !fin ) { std::cout << "no dot SS data\n"; return false; }
    dotSS.init(fin);
    fin.close();

    // a bunch of sparks
    sf::Vector2f emitPos; fin >> emitPos.x >> emitPos.y;
    size_t Nsparks = 300;
    size_t i = 0;
    sparkVec.resize( Nsparks );
    sparkIt = sparkVec.begin();
    for( spark& spk : sparkVec )
    {
        size_t idx = i%dotSS.getFrCount(0); ++i;
        spk.init( 1.0f, sf::Vector2f(0.0f,0.0f), sf::Vector2f(0.0f,0.0f), idx, -1.0f );
        spk.setTxtCoords( dotSS.getFrRect( idx, 0 ) );
    }

    const char* fName = "include/level/spiSol_data/fireWork_data.txt";
    fin.open( fName );
    if( !fin ) return false;
 //   fireworkInit( fin );
    std::function<spark*(void)> GetSpark = [this]()
    {
        if( ++sparkIt == sparkVec.end() ) sparkIt = sparkVec.begin();
        return &(*sparkIt);
    };
    std::function<sf::IntRect(size_t)> GetFrRect = [this](size_t i){ return dotSS.getFrRect(i,0); };
    fin >> fireOfst.x >> fireOfst.y;
    fin >> fwPos1.x >> fwPos1.y;
    fw1.init( fwPos1, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin >> fwPos2.x >> fwPos2.y;
    fw2.init( fwPos2, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin >> fwPos3.x >> fwPos3.y;
    fw3.init( fwPos3, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin.close();

    return true;
}

void spiderSolitaire::updateFireworks( float dt )
{
    static std::default_random_engine rand_gen;
    static std::uniform_real_distribution<float> pos_dist(-1.0f,1.0f);
    if( !fw1.pShell ) { fw1.firePos = fwPos1 + fireOfst*pos_dist(rand_gen); fw1.doFire = true; }
    if( !fw2.pShell ) { fw2.firePos = fwPos2 + fireOfst*pos_dist(rand_gen); fw2.doFire = true; }
    if( !fw3.pShell ) { fw3.firePos = fwPos3 + fireOfst*pos_dist(rand_gen); fw3.doFire = true; }
    fw1.update(dt);
    fw2.update(dt);
    fw3.update(dt);

    if( !anyLive_emit ) return;
    anyLive_emit = false;
    for( spark& spk : sparkVec )
        if( spk.tLife > 0.0f )
        {
            spk.update( sf::Vector2f(0.0f,fw1.fwGrav), dt );
            anyLive_emit = true;
        }
}

bool spiderSolitaire::saveGame( const char* fileName )
{
    std::ofstream fout( fileName );
    if( !fout ) return false;

    fout << numSuits << ' ' << score << ' ' << numCols;
    for( auto& CL : cardCol )
    {
        fout << '\n' << CL.numFacedown() << ' ' << CL.numFaceup();
        for( const cardNode* it = CL.head; it; it = it->next )
            fout << ' ' << it->pCard->suit << ' ' << it->pCard->face;
    }

    fout << '\n' << drawList.numCards();
    for( const cardNode* it = drawList.head; it; it = it->next )
            fout << ' ' << it->pCard->suit << ' ' << it->pCard->face;

    return true;
}

int spiderSolitaire::findMoves()// for move hints
{
    int numMoves = 0;
    size_t dest_col=0, sce_col=0;
    //iterate over potential dest columns
    for( const cardList& rCLdest : cardCol  )
    {
        if( !rCLdest.tail || rCLdest.tail->pCard->face == 1 ){ ++dest_col; continue; }// no card over ace

        sce_col=0;
        const card& destTailCard = *rCLdest.tail->pCard;// must continue straight. Suit need not match
        for( const cardList& rCLsce : cardCol  )
        {
            if( !rCLsce.tail ){ ++sce_col; continue; }
            // can we find a straight back to destTail->pCard->face -1 ?
            if( rCLsce.tail->pCard->face + 1 == destTailCard.face )// one card move
            {
                std::cout << '\n' << rCLsce.tail->pCard->face << " in col " << sce_col << " onto ";
                std::cout << destTailCard.face << " in col " << dest_col;
                ++numMoves;
                if( rCLsce.tail->prev && rCLsce.tail->prev->pCard->faceUp && rCLsce.tail->prev->pCard->suit == rCLsce.tail->pCard->suit && rCLsce.tail->prev->pCard->face == destTailCard.face )// straight continues
                    std::cout << " SUB LIST SHIFT - 1 card";
            }
            else if( rCLsce.tail->pCard->face + 1 < destTailCard.face )// straight formed ?
            {
                char matchSuit = rCLsce.tail->pCard->suit;
                int lastFace = rCLsce.tail->pCard->face;
                for( const cardNode* pCN = rCLsce.tail->prev; pCN && pCN->pCard->faceUp && pCN->pCard->suit == matchSuit && pCN->pCard->face == ++lastFace; pCN = pCN->prev  )
                {
                    if( lastFace + 1 == destTailCard.face )// straight found
                    {
                        std::cout << '\n' << pCN->pCard->face << " in col " << sce_col << " onto ";
                        std::cout << destTailCard.face << " in col " << dest_col;
                        ++numMoves;

                        if( pCN->prev && pCN->prev->pCard->faceUp && pCN->prev->pCard->suit == matchSuit && pCN->prev->pCard->face == destTailCard.face )// straight continues
                            std::cout << " SUB LIST SHIFT";
                    }
                }
            }

            ++sce_col;
        }

        ++dest_col;
    }

    return numMoves;
}

bool isValidMove( cardNode* pDestTailNode, cardNode& sceNode )
{
    if( pDestTailNode && ( pDestTailNode->pCard->face != sceNode.pCard->face+1 ) ) return false;// dest tail must continue countdown

    // require countup and suit match in moving list
    char matchSuit = sceNode.pCard->suit;
    cardNode* it = sceNode.next;// so prev exists

    while( it )
    {
        if( it->pCard->suit != matchSuit ) return false;
        if( it->pCard->face + 1 != it->prev->pCard->face ) return false;
        it = it->next;
    }

    return true;
}
