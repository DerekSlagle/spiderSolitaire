#include "cardPlay.h"

bool cardPlay::init()
{
    Level::goto_MMButt.setPosition( {Level::winW/2-40.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW/2+40.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::clearColor = sf::Color(0,100,200);

    deal1_fname = "include/level/cardPlay_data/deal1_data.txt";
    deal2_fname = "include/level/cardPlay_data/deal2_data.txt";

    if( !loadFirstGame() ) return false;

//    std::function<void(size_t,size_t,size_t)> pFuncAll = [this]( size_t i, size_t j, size_t n, button* pButt )
    std::function<void(size_t,size_t,size_t)> pFuncAll = [this]( size_t i, size_t j, size_t n )
    {
        if( !animating && cardListVec.size() > i && cardListVec.size() > j && n > 0 )
        {
            pListSce = &cardListVec[i];
            pListDest = &cardListVec[j];
            if( pListSce->tail )
            {
                startSelectedCardMove( pListSce, pListDest, pListSce->tail, false );
                xferCount = n - 1;
                animateID = 8;
            }
            else
            {
                if( xferList.pButt_sel ) xferList.pButt_sel->setSel(false);
            //    xferButt.setSel(false); xferButt2.setSel(false); xferButt3.setSel(false);
                pListSce = pListDest = nullptr;
            }
        }
    };

    sf::Text label("xfer", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    float posX = 200.0f, posY = 10.0f;
    float buttH = 25.0f, buttW = 60.0f;
    float dy = 2.0f;
    xferList.init( posX, posY, buttW, buttH, label );
    label.setString( "xfer 013" );
    xferList.pButtVec.push_back( new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, [pFuncAll]() { pFuncAll(0,1,3); }, label ) );
    label.setString( "xfer 124" );
    xferList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, [pFuncAll]() { pFuncAll(1,2,4); }, label ) );
    label.setString( "xfer 202" );
    xferList.pButtVec.push_back( new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [pFuncAll]() { pFuncAll(2,0,2); }, label ) );
    xferList.persist = 3;
    xferList.ownButts = true;
    button::RegisteredButtVec.push_back( &xferList );

    moveModeMsg = label; moveModeMsg.setColor( sf::Color::Red ); moveModeMsg.setCharacterSize(20);
    moveModeMsg.setPosition( 1000.0f, 10.0f );
    label.setString( "move mode" );
    moveModeButt.init( 900.0f, 10.0f, 60.0f, 30.0f, nullptr, label );
   // moveModeButt.pHitFunc = [this]() { dragDrop = moveModeButt.sel; if( moveModeButt.sel ) moveModeMsg.setString("move mode: drag & drop"); else moveModeMsg.setString("move mode: 2 clicks"); };
    moveModeButt.pHitFunc = [this]() { if( !pListSce ) dragDrop = moveModeButt.sel; if( moveModeButt.sel ) moveModeMsg.setString("move mode: drag & drop"); else moveModeMsg.setString("move mode: 2 clicks"); };
    moveModeButt.setSel( dragDrop ); moveModeButt.pHitFunc();
    button::RegisteredButtVec.push_back( &moveModeButt );

    // for the xferButt xfers
    pEndAniFunc = [this]()
    {
        if( animateID == 8 && pListSce && pListDest )// xfer
        {
            if( moveList.tail ) pListDest->append_toEnd( &moveList, moveList.head );
            if( pListSce->tail && xferCount > 0 )
            {
                startSelectedCardMove( pListSce, pListDest, pListSce->tail, false );
                --xferCount;
            }
            else
            {
                pListSce->pNodeSel = nullptr; pListSce = nullptr;
                pListDest->pNodeSel = nullptr; pListDest = nullptr;
                xferCount = 0;
                animateID = 0;
                //xferButt.setSel(false); xferButt2.setSel(false); xferButt3.setSel(false);
                if( xferList.pButt_sel ) xferList.pButt_sel->setSel(false);
            }
        }
        else
        {
            animateID = 0;// all animation follow actions handled
            drawList.isFanned = false;// support animation of cards to crawList
            if( pListSce ) { pListSce->pNodeSel = nullptr; pListSce = nullptr; }
            if( pListDest ) { pListDest->pNodeSel = nullptr; pListDest = nullptr; }
        }
    };

    return true;
}

bool cardPlay::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Num1 ) { animateID = dealID = 1; std::cout << "\ndealID = " << dealID; }
        else if ( rEvent.key.code == sf::Keyboard::Num2 ) { animateID = dealID = 2; std::cout << "\ndealID = " << dealID; }
    }
    return true;
}

void cardPlay::update( float dt )
{
 //   if( !( animating || button::pButtMse ) ) handleCardHit();
    if( !animating ) handleCardHit();
    updateAnimations( cardListVec, dt );
}

void cardPlay::handleCardHit()
{
    // drag drop release over a button
    if( button::pButtMse && dragDrop && pListSce && button::clickEvent_Lt() == -1 )
    {
        pListSce->setCardSpacing();
        pListSce->pNodeSel = nullptr; pListSce = nullptr;
    }

    if( button::pButtMse || !button::clickEvent() || button::clickEvent_Rt() == -1 ) return;// excluding only right button release

    if( button::clickEvent_Rt() == 1 )
        for( cardList& rCL : cardListVec )
        {
            cardNode* pNodeHit = nullptr;
            if( rCL.hit( pNodeHit, button::mseX, button::mseY ) )
                { pNodeHit->pCard->initFlip(0.0f); break; }
        }
    else moveOneCard_event( cardListVec );

    // return to drawList
    if( drawList.tail && drawList.tail->pCard->hit(button::mseX, button::mseY) )
    {
        size_t i = 0;
        drawList.isFanned = true;// support animation of cards to crawList
        if( LegAdaptVec.size() < cardNodeVec.size() ) LegAdaptVec.resize( cardNodeVec.size() );
        if( linLegVec.size() < cardNodeVec.size() ) linLegVec.resize( cardNodeVec.size() );
        for( cardList& rCL : cardListVec )
        {
//            while( rCL.head )
            while( rCL.head )
            {
                sf::Vector2f pos;
                drawList.append_One( &rCL, rCL.tail, pos );
                card* pCard = drawList.tail->pCard;
                startLegAnimation( linLegVec[i], LegAdaptVec[i], pCard, pCard->getPosition(), pos, card_v1 );
                ++i;
            }
            std::cout << "\ni = " << i;
        }
    }
}

void cardPlay::draw( sf::RenderTarget& RT ) const
{
    if( drawList.tail )
    {
        if( animating ) drawList.draw(RT);
        else drawList.tail->pCard->draw(RT);
    }

    for( const cardList& cl : cardListVec ) cl.draw(RT);
    moveList.draw(RT);
//    if( dragDrop && pNodeSce ) pNodeSce->pCard->draw(RT);
    if( dragDrop && pListSce && pListSce->pNodeSel ) pListSce->pNodeSel->pCard->draw(RT);
    RT.draw( moveModeMsg );
}

void cardPlay::cleanup() {}

void cardPlay::makeNewGame()
{
    cardNodeVec.clear();
    cardNodeVec.reserve( cardSetVec.size() );
    for( card& rCard : cardSetVec ) cardNodeVec.push_back( cardNode( &rCard, nullptr, nullptr ) );
    cardNodeVec.back().next  = nullptr;

    std::ifstream fin("include/level/cardPlay_data/init_data.txt");
    if( !fin ) { std::cout << "\nNo card initData"; return; }
    float dLx0, dLy0; fin >> dLx0 >> dLy0;
    fin >> card_x0 >> card_y0 >> card_dx >> card_v1 >> dragDrop;
    size_t numCols; fin >> numCols;
    cardListVec.resize(numCols);

    linLegVec.resize( cardNodeVec.size() );
    LegAdaptVec.resize( cardNodeVec.size() );

 //   float dX = 0.0f;
    std::vector<cardNode>::iterator it = cardNodeVec.begin();
    size_t coLidx = 0;
    for( cardList& CL : cardListVec )
    {
        CL.maxLength = cardColMaxLength;

        size_t numCards; fin >> numCards;
        float v, x0, y0, xf, yf; fin >> v >> x0 >> y0 >> xf >> yf;
        CL.setBasePosition( x0, y0, 0.0f, card_dy, card_dyFD );
        for( size_t i=0; it != cardNodeVec.end() && i<numCards; ++i )
        {
            CL.push_back(*it);
            it->pCard->faceUp = true;
            ++it;
        }

        startCardListMove( CL, sf::Vector2f(xf-x0,yf-y0), linLegVec[coLidx], LegAdaptVec[coLidx], v );
        ++coLidx;
    }

    // all remaining to drawList
 //   drawList.setBasePosition( dLx0, dLy0, 0.0f, 0.0f, 0.0f );
    drawList.setBasePosition( dLx0, dLy0 );
    while( it != cardNodeVec.end() )
    {
         drawList.push_back( *it );
         it->pCard->faceUp = false;
        ++it;
    }
    drawList.tail->next = nullptr;
}

bool cardPlay::loadFirstGame()
{
    if( cardSetVec.size() == 0 && !loadCards() ) return false;
        // new game code
    makeNewGame();
    return true;
}
