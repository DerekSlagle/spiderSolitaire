#include "lvl_spark.h"

bool lvl_spark::init()
{
    Level::goto_MMButt.setPosition( {Level::winW-160.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW-80.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
 //   Level::clearColor = sf::Color(0,100,200);

    std::ifstream fin( "images/dots9_SSdata.txt" );
    if( !fin ) { std::cout << "no dot SS data\n"; return false; }
    dotSS.init(fin);
    fin.close();

 //   spark::pTxt = &dotSS.txt;

    fin.open("include/level/lvl_spark_data/init_data.txt");
    if( !fin ) { std::cout << "\nNo spark init_data"; return false; }
    sf::Vector2f pos; fin >> pos.x >> pos.y;// for sparkAni

    if( !theSA.init( pos, &dotSS, "include/level/lvl_spark_data/SavedSparkAni.txt" ) ) { std::cout << "\ntheSA init failed"; return false; }

    float posX, posY; fin >> pos.x >> pos.y;// for control
  //  makeSparkAniControl( pos, sparkList, theSA );
 //   button::RegisteredButtVec.push_back( &sparkList );

    makeFireControl();// NEW!

    sf::Text label("???", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    posX = 250.0f; posY = 10.0f;
    float buttW = 55.0f, buttH = 22.0f;
    float dy = 2.0f;
    label.setString("EMIT");
    emitterList.init( posX, posY, buttW, buttH, label );

 //   std::function<void(bool*)> pFunc_2 = [this]( bool* pOn ) { if( pCurrOn ) *pCurrOn = false; pCurrOn = pOn; if( pCurrOn ) *pCurrOn = true; };
    std::function<void(bool*)> pFunc_2 = [this]( bool* pOn )
    {
        if( pCurrOn )
        {
            if( pCurrOn == pOn ) *pOn = !(*pOn);// toggle currnt
            else
            {
                *pCurrOn = false;// turn off old
                *pOn = true;
            }
        }
        pCurrOn = pOn;
    };
    label.setString( "stream" );
//    buttonRect* pStreamButt = new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&streamOn); }, label );
    buttonRect* pStreamButt = new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { streamOn = true; }, label );
    emitterList.pButtVec.push_back( pStreamButt );
    label.setString( "TEST" );
 //   emitterList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&forceTestOn); }, label ) );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { forceTestOn = streamOn = true; }, label ) );
    label.setString( "OFF" );
 //   buttonRect* pBR = new buttonRect( posX, posY + 4.0f*(buttH+dy), buttW, buttH, [this](){ if( pCurrOn ) *pCurrOn = false; }, label );
    buttonRect* pBR = new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [this](){ forceTestOn = streamOn = false; }, label );
    pBR->mode = 'I';
    emitterList.pButtVec.push_back( pBR );
    label.setString( "Clear" );
    pBR = new buttonRect( posX, posY + 4.0f*(buttH+dy), buttW, buttH, [this](){ showArea = mapReady = false; theSA.reset(); }, label );
    pBR->mode = 'I';
    emitterList.pButtVec.push_back( pBR );

    label.setString( "flip" );
    buttonRect* pFlipButt = new buttonRect( posX+buttW+dy, posY + 0.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&theSA.doFlip); }, label );
 //   buttonRect* pFlipButt = new buttonRect( posX+buttW+dy, posY + 1.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { theSA.doFlip = !theSA.doFlip; }, label );
    emitterList.pButtVec.push_back( pFlipButt );
    label.setString( "spin" );
    buttonRect* pSpinButt = new buttonRect( posX+buttW+dy, posY + 1.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&theSA.doSpin); }, label );
 //   buttonRect* pSpinButt = new buttonRect( posX+buttW+dy, posY + 2.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { theSA.doSpin = !theSA.doSpin; }, label );
    emitterList.pButtVec.push_back( pSpinButt );
    label.setString( "sparkle" );
    emitterList.pButtVec.push_back( new buttonRect( posX+buttW+dy, posY + 2.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&randOn); }, label ) );
    label.setString( "rain" );
    emitterList.pButtVec.push_back( new buttonRect( posX+buttW+dy, posY + 3.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&lineOn); }, label ) );
    label.setString( "one" );
 //   emitterList.pButtVec.push_back( new buttonRect( posX+buttW+dy, posY + 5.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&oneOn); }, label ) );
    emitterList.pButtVec.push_back( new buttonRect( posX+buttW+dy, posY + 4.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { oneOn = true; }, label ) );

    emitterList.persist = 3;
    emitterList.ownButts = true;
    button::RegisteredButtVec.push_back( &emitterList );
    emitterList.setSel(true);

 //   pCurrOn = &streamOn;
 //   if( theSA.doSpin ) pCurrOn = &theSA.doSpin;
 //   else if( theSA.doFlip ) pCurrOn = &theSA.doFlip;

    if( emitterList.sel )
    {
        if( theSA.doFlip ) { pFlipButt->setSel(true); pFlipButt->pHitFunc(); }
        else if( theSA.doSpin ) { pSpinButt->setSel(true); pSpinButt->pHitFunc(); }
        else { pStreamButt->setSel(true); pStreamButt->pHitFunc(); }
    }

 //   pStreamButt->setSel(true);
 //   pFunc_2(&streamOn);
 //   streamOn = true;

    float spotRadius = 4.0f;
  //  fin >> posX >> posY;
    std::function<void(float)> p_SetFunc_spd = [this]( float x ) { theSA.fireSpeed = x; fireSpot.voh.set_fVal(x); };
 //   fireSpot.init( sf::Vector2f(posX,posY), sf::Vector2f(15.0f,-8.0f), spotRadius, "emitter", 14 );
    fireSpot.init( theSA.getPosition(), sf::Vector2f(15.0f,-8.0f), spotRadius, "emitter", 14 );
 //   std::function<void(float)> p_SetFunc_spd2 = [p_SetFunc_spd, this](float x){ p_SetFunc_spd(x); if( pSB_spd ) pSB_spd->set_fVal(x); };
    std::function<void(float)> p_SetFunc_spd2 = [p_SetFunc_spd, this](float x){ p_SetFunc_spd(x); speedSB.set_fVal(x); };
 //   fireSpot.init2( pSB_spd->fVal_min, pSB_spd->fVal_max, theSA.fireSpeed, "speed", p_SetFunc_spd2 );
    fireSpot.init2( speedSB.fVal_min, speedSB.fVal_max, theSA.fireSpeed, "speed", p_SetFunc_spd2 );
    button::RegisteredButtVec.push_back( &fireSpot );
 //   theSA.emitPos = sf::Vector2f(posX,posY);

    fin >> cr >> crBall >> wallCr;
    fin >> diff_ds >> ds_map;

    fin >> frIdxFce >> frIdxGrad >> frIdxMap;

    // end of datum from init_data.txt
    fin.close();

    // spark txt coords
    size_t Nframes = dotSS.getFrCount(0);
    std::cout << "\nnum frames = " << Nframes;
    for( size_t i=0; i<theSA.spkVec.size(); ++i )
        theSA.spkVec[i].setTxtCoords( dotSS.getFrRect( i%Nframes, 0 ) );

    if( !LoadBarriers() ) return false;
    if( !LoadForceControls() ) return false;

    // save the current simulation to a sparkAni
    label.setString( "SAVE" );
    std::function<void(void)> pF = [this](){ save_sparkAni_toFile( "include/level/lvl_spark_data/SavedSparkAni.txt", "include/level/lvl_spark_data/SSAspark_data.txt" ); };
    saveSAbutt.init( 50.0f, 680.0f, 50.0f, 22.0f, pF, label );
    button::RegisteredButtVec.push_back( &saveSAbutt );


    label.setString( "AREA" );
    aniAreaButt.init( 150.0f, 680.0f, 50.0f, 22.0f, nullptr, label ); aniAreaButt.mode = 'I';
    button::RegisteredButtVec.push_back( &aniAreaButt );
    aniArea[0].color = aniArea[1].color = aniArea[2].color = aniArea[3].color =  aniArea[4].color = sf::Color(200,0,50,70);

    label.setString( "ds_map" );
    ds_mapStrip.init( 400.0f, 680.0f, 200.0f, 20.0f, label, 0.2f, 30.0f, 5.0f, &ds_map );
    button::RegisteredButtVec.push_back( &ds_mapStrip );

 //   label.setString( "TXTR" );
 //   txtOnOffButt.init( 320.0f, 680.0f, 50.0f, 22.0f, [this]() { spark::pTxt = txtOnOffButt.sel ? &dotSS.txt : nullptr; }, label );
//    button::RegisteredButtVec.push_back( &txtOnOffButt );

    posX = 20.0f; posY = 550.0f;
    buttW = 55.0f, buttH = 22.0f; dy = 2.0f;
    label.setString("SA spks");
    SAsparkLoad.init( posX, posY, buttW, buttH, label );
    label.setString("re load");
    pF = [this,pStreamButt,pFlipButt,pSpinButt,pFunc_2]()
    {
        theSA.loadSparks( "include/level/lvl_spark_data/SSAspark_data.txt" );
        emitterList.setSel(true);
        if( theSA.doFlip ) { pFlipButt->setSel(true); pFlipButt->pHitFunc(); }
        else if( theSA.doSpin ) { pSpinButt->setSel(true); pSpinButt->pHitFunc(); }
        else { pStreamButt->setSel(true); pStreamButt->pHitFunc(); }

    //    if( pCurrOn ) *pCurrOn = false;
        lineOn = randOn = false;
    //    pStreamButt->setSel(true);
    //    pFunc_2(&streamOn);
    };
    SAsparkLoad.pButtVec.push_back( new buttonRect( posX, posY + (buttH+dy), buttW, buttH, pF, label ) );
    SAsparkLoad.persist = 3;
    SAsparkLoad.ownButts = true;
    button::RegisteredButtVec.push_back( &SAsparkLoad );
    // level start conditions
 //   streamOn = true;
 //   pCurrOn = &streamOn;

    return true;
}

bool lvl_spark::LoadForceControls()
{
    // forces list
    float posX = 500.0f; float posY = 10.0f;
    float buttW = 55.0f, buttH = 22.0f; float dy = 2.0f;
    sf::CircleShape tmpCircle;
    tmpCircle.setFillColor( sf::Color::Magenta );
    tmpCircle.setRadius(4.0f);
    tmpCircle.setOrigin(4.0f,4.0f);
    sf::Text label("Forces", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    forceList.init( posX, posY, buttW, buttH, label );

    if( theSA.CFvec.size() == 1 )// no sub list for 1 force
    {
        theSA.CFvec[0].makeListControl( forceList );
        tmpCircle.setPosition( theSA.CFvec[0].ctrPos );
        forceSpotVec.push_back( tmpCircle );
    }
    else for( centralForce& rCF : theSA.CFvec )
    {
        posY += buttH + dy;
        label.setString( "fce" );
        buttonList* pBL = new buttonList( posX, posY, buttW, buttH, label );
        rCF.makeListControl( *pBL );
        forceList.pButtVec.push_back( pBL );
        tmpCircle.setPosition( rCF.ctrPos );
        forceSpotVec.push_back( tmpCircle );
    }

    label.setString( "on/off" );
    forceList.pButtVec.push_back( new buttonRect( posX, posY + (buttH+dy), buttW, buttH, [this](){ applyForce = !applyForce; }, label ) );

    forceList.persist = 3;
    forceList.ownButts = true;
    button::RegisteredButtVec.push_back( &forceList );

    std::ifstream fin( "include/level/lvl_spark_data/ForceLimits.txt" );
    if( !fin ) { std::cout << "no SA control data\n"; return false; }

    label.setString("cross");
    crossList.init( 800.0f, 10.0f, 50.0f, 20.0f, label );
    label.setString("k value");
    float minVal, maxVal, iVal; fin >> minVal >> maxVal >> iVal;
    crossStrip.init( 870.0f, 18.0f, 120.0f, 20.0f, label, minVal, maxVal, iVal, &theSA.crossK );
    crossList.pButtVec.push_back( &crossStrip );
    crossList.persist = 3;
    crossList.ownButts = false;
    button::RegisteredButtVec.push_back( &crossList );

    if( theSA.doCross ) { crossList.setSel(true); crossStrip.set_fVal( theSA.crossK ); }

    label.setString("DRAG");
    dragList.init( 800.0f, 40.0f, 50.0f, 20.0f, label );
    label.setString("k value");
    fin >> minVal >> maxVal >> iVal;
    dragStrip.init( 870.0f, 63.0f, 120.0f, 20.0f, label, minVal, maxVal, iVal, &theSA.dragK );
    dragList.pButtVec.push_back( &dragStrip );
    dragList.persist = 3;
    dragList.ownButts = false;
    button::RegisteredButtVec.push_back( &dragList );

    if( theSA.doDrag ) { dragList.setSel(true); dragStrip.set_fVal( theSA.dragK ); }

    label.setString("MASS");
    massList.init( 800.0f, 70.0f, 50.0f, 20.0f, label );
    label.setString("ratio");
 //   fin >> minVal >> maxVal >> iVal;
    massStrip.init( 870.0f, 108.0f, 120.0f, 20.0f, label, 0.5f, 10.0f, 1.0f, nullptr );
    massList.pButtVec.push_back( &massStrip );
    massList.persist = 3;
    massList.ownButts = false;
    button::RegisteredButtVec.push_back( &massList );

    return true;
}

void lvl_spark::makeSparkAniControl( sf::Vector2f pos, buttonList& BL, sparkAni& sa )// from fully initialized sparkAni
{
    std::ifstream fin( "include/level/lvl_spark_data/SAcontrol_data.txt" );
    if( !fin ) { std::cout << "no SA control data\n"; return; }

    sf::Text label("sparks", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    sparkAni* pSA = &sa;

 //   float posX, posY; fin >> posX >> posY;
    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    float buttHgt = 25.0f;
    float dy = 17.0f;
    sparkList.init( pos.x, pos.y, 80.0f, buttHgt, label );
    label.setString("spark size");
    pos.y += buttHgt + dy;
    std::function<void(float)> p_SetFunc_sz = [this,pSA]( float x ) { for( spark& spk : pSA->spkVec ) spk.setSize(x); };//spk.quadW = x; };
  //  float size0; fin >> size0;
    float minVal, maxVal; fin >> minVal >> maxVal;
    sparkList.pButtVec.push_back( new buttonValOnHit( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->quadW_all, p_SetFunc_sz ) );

    label.setString("fire direction");
    pos.y += stripHgt + dy;
    std::function<void(float)> p_SetFunc = [this,pSA]( float x )
    {
        pSA->fireAngle = x;
        pSA->fireDir.x = cosf(x); pSA->fireDir.y = sinf(x);
    };
 //   pSB_angle = new slideBar( pos.x, pos.y, stripLen, stripHgt, label, -4.0f, 4.0f, pSA->fireAngle, p_SetFunc );
 //   sparkList.pButtVec.push_back( pSB_angle );

    label.setString("spark speed"); pos.y += stripHgt + dy;
    std::function<void(float)> p_SetFunc_spd = [this,pSA]( float x )
    {
        pSA->fireSpeed = x;
        fireSpot.voh.set_fVal(x);
    };
    fin >> minVal >> maxVal;// sparkVel
 //   pSB_spd = new slideBar( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->fireSpeed, p_SetFunc_spd );
 //   sparkList.pButtVec.push_back( pSB_spd );

    label.setString("life time"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal;// >> theSA.tLife_all;
    sparkList.pButtVec.push_back( new buttonValOnHit( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->tLife_all, &pSA->tLife_all ) );

    label.setString("fire delay"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal;// >> theSA.emitPeriod;
    sparkList.pButtVec.push_back( new buttonValOnHit( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->emitPeriod, &pSA->emitPeriod ) );

    label.setString("gravity"); pos.y += stripHgt + dy;
    fin >> maxVal;// >> theSA.gravity.y; theSA.gravity.x = 0.0f;
    p_SetFunc = [this,pSA]( float x ) { pSA->gravity.y = x; pSA->gravityOn = true; };
 //   pSB_grav = new slideBar( pos.x, pos.y, stripLen, stripHgt, label, -maxVal, maxVal, pSA->gravity.y, p_SetFunc );
 //   sparkList.pButtVec.push_back( pSB_grav );

    label.setString("flip period"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal;// >> theSA.flipPeriod;
    sparkList.pButtVec.push_back( new buttonValOnHit( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->flipPeriod, &pSA->flipPeriod ) );

    label.setString("spin period"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal;// >> theSA.flipPeriod;
    sparkList.pButtVec.push_back( new buttonValOnHit( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, pSA->flipPeriod, &pSA->flipPeriod ) );

    sparkList.persist = 3;
    sparkList.ownButts = true;
}

void lvl_spark::makeFireControl(void)
{
/*
    controlSurface fireCsurf;// replaces sparkList
    multiSelector fireMS;// size tLife fireDelay gravity
    buttonValOnHit fireVoh;
    slideBar angleSB, speedSB;// fire angle spark speed

    buttonRect onOffButt;// toggle value of streaming
    controlSurface emitCsurf;// replaces emitterList
    std::vector<buttonRect> emitButtVec;// clear one test spin flip sparkle rain
    controlSurface settingsCsurf;// emit parameters
    multiSelector emitMS;// for spin flip sparkle rain map_ds
    buttonValOnHit emitVoh; */

    std::ifstream fin( "include/level/lvl_spark_data/fireControl.txt" );
    if( !fin ) { std::cout << "no FC control data\n"; return; }

    sf::Text label("sparks", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    sparkAni* pSA = &theSA;

    sf::Vector2f hbPos, Pos, csurfDim, ofst;
    fin >> hbPos.x >> hbPos.y >> Pos.x >> Pos.y;
    fin >> csurfDim.x >> csurfDim.y;
    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    fireCsurf.init( hbPos, Pos, csurfDim, "fire" );
    fin >> ofst.x >> ofst.y;// to MS
    fireMS.init( Pos + ofst, &fireVoh, fin );
    label.setString("fire angle");
    fin >> ofst.x >> ofst.y;// to angleSB
    std::function<void(float)> angleFunc = [this,pSA]( float x ) { pSA->fireAngle = x; pSA->fireDir.x = cosf(x); pSA->fireDir.y = sinf(x); };
    angleSB.init( Pos.x+ofst.x, Pos.y+ofst.y, stripLen, stripHgt, label, -3.1416f, 3.1416f, 0.0f, angleFunc );
    label.setString("fire speed");
    fin >> ofst.x >> ofst.y;// to angleSB
    float xMin, xMax, xInit; fin >> xMin >> xMax >> xInit;
    std::function<void(float)> speedFunc = [this,pSA]( float x ) { pSA->fireSpeed = x; fireSpot.voh.set_fVal(x); };
    speedSB.init( Pos.x+ofst.x, Pos.y+ofst.y, stripLen, stripHgt, label, xMin, xMax, xInit, speedFunc );
    fin >> ofst.x >> ofst.y;// to on/off butt
    label.setString("on/off");
    float buttW = 50.0f, buttH = 25.0f;
    onOffButt.init( Pos.x+ofst.x, Pos.y+ofst.y, buttW, buttH, [this](){ streamOn = onOffButt.sel; if( !streamOn ) forceTestOn = false; }, label );// aniAreaButt.mode = 'I';

    fin.close();

    fireCsurf.pButtVec.push_back( &fireMS );
    fireCsurf.pButtVec.push_back( &angleSB );
    fireCsurf.pButtVec.push_back( &speedSB );
    fireCsurf.pButtVec.push_back( &onOffButt );
    button::RegisteredButtVec.push_back( &fireCsurf );

 //   size_t MsId = 0;
    stripData* pSD = &fireMS.sdataVec.front();
    if( pSD && fireMS.sdataVec.size() >= 5 )
    {
    //    pSD->pSetFunc = [this,pSA](float x) { pSA->emitPeriod = x; };// tFire
        pSD->pSetVal = &pSA->emitPeriod;// tFire
        fireVoh.reInit( *pSD );// length
    //    (++pSD)->pSetFunc = [this,pSA](float x) { pSA->tLife_all = x; };// tLife
        (++pSD)->pSetVal = &pSA->tLife_all;// tLife
        (++pSD)->pSetFunc = [this,pSA](float x) { pSA->gravity.y = x; pSA->gravityOn = true; };// gravity
    //    (++pSD)->pSetFunc = [this,pSA](float x) { pSA->flipPeriod = x; };// cycle period
        (++pSD)->pSetVal = &pSA->flipPeriod;// cycle period
        (++pSD)->pSetFunc = [this,pSA](float x) { for( spark& spk : pSA->spkVec ) spk.setSize(x); };// aprk size
    }

    // extra functions
    fin.open( "include/level/lvl_spark_data/emitCsurf.txt" );
    if( !fin ) { std::cout << "no emit Csurf data\n"; return; }
    fin >> ofst.x >> ofst.y;// to homeButt
    hbPos = Pos + ofst;
    fin >> ofst.x >> ofst.y;// to surface
    sf::Vector2f sfcPos = hbPos + ofst;
    fin >> csurfDim.x >> csurfDim.y;
    emitCsurf.init( hbPos, sfcPos, csurfDim, "emit" );
    emitCsurf.homeButt.pHitFunc = [this](){ if( !emitCsurf.homeButt.sel ) { if( pCurrOn ) *pCurrOn = false; if( pSelButt ) pSelButt->setSel( false ); } };
    fireCsurf.onSurfaceOnly = false;
    emitButtVec.resize(7);
    buttonRect* pBR = &emitButtVec[0];
    fin >> ofst.x >> ofst.y;// to 1st Butt
    Pos = sfcPos + ofst;
    label.setString("clear");
    pBR->init( Pos.x, Pos.y, buttW, buttH, [this](){ showArea = mapReady = false; theSA.reset(); }, label ); pBR->mode = 'I';
    fin >> ofst.x >> ofst.y;// to other Butts
    ofst.x += buttW; ofst.y += buttH;
    label.setString("one");
    (++pBR)->init( Pos.x + ofst.x, Pos.y, buttW, buttH, [this](){ oneOn = true; }, label ); pBR->mode = 'I';
    label.setString("test");
    (++pBR)->init( Pos.x + 2.0f*ofst.x, Pos.y, buttW, buttH, [this](){ forceTestOn = streamOn; }, label );
    std::function<void(bool*,buttonRect*)> pFunc_2 = [this]( bool* pOn, buttonRect* pBR )
    {
        if( pCurrOn )
        {
            if( pCurrOn == pOn ) *pOn = !(*pOn);// toggle currnt
            else
            {
                *pCurrOn = false;// turn off old
                if( pSelButt ) pSelButt->setSel( false );
                pSelButt = pBR;
                *pOn = true;
            }
        }
        pCurrOn = pOn;
    };
    label.setString("spin"); ++pBR;
    pBR->init( Pos.x, Pos.y + ofst.y, buttW, buttH, [pFunc_2,pBR,this](){ pFunc_2( &theSA.doSpin, pBR ); }, label );
    label.setString("flip"); ++pBR;
    pBR->init( Pos.x + ofst.x, Pos.y + ofst.y, buttW, buttH, [pFunc_2,pBR,this](){ pFunc_2( &theSA.doFlip, pBR ); }, label );
    label.setString("sparkle"); ++pBR;
    pBR->init( Pos.x + 2.0f*ofst.x, Pos.y + ofst.y, buttW, buttH, [pFunc_2,pBR,this]() { pFunc_2( &randOn, pBR ); }, label );
    label.setString("rain"); ++pBR;
    pBR->init( Pos.x, Pos.y + 2.0f*ofst.y, buttW, buttH, [pFunc_2,pBR,this]() { pFunc_2( &lineOn, pBR ); }, label );

    fin.close();
    fireCsurf.pButtVec.push_back( &emitCsurf );
    for( auto& butt : emitButtVec ) emitCsurf.pButtVec.push_back( &butt );
    pCurrOn = &streamOn;
}

bool lvl_spark::LoadBarriers()
{
    std::ifstream fin("include/level/lvl_spark_data/barrier_data.txt");
    if( !fin ) { std::cout << "\nNo spark barrier_data"; return false; }

    // line source
    fin >> lineSceVtx[0].position.x >> lineSceVtx[0].position.y;
    fin >> lineSceVtx[1].position.x >> lineSceVtx[1].position.y;
    lineSceVtx[0].color = lineSceVtx[1].color = sf::Color::Blue;

    // line obstacles
    size_t numLines; fin >> numLines;
    lineVec.reserve( 2*numLines );
    sf::Vertex vtxTmp; vtxTmp.color = sf::Color::White;
    for( size_t i = 0; i < 2*numLines; ++i )
    {
         fin >> vtxTmp.position.x >> vtxTmp.position.y;
         lineVec.push_back( vtxTmp );
    }
    // circle obstacles
    size_t numCircles; fin >> numCircles;
    sf::CircleShape tmpCircle; tmpCircle.setFillColor( sf::Color(100,100,100) );
    for( size_t i = 0; i < numCircles; ++i )
    {
        sf::Vector2f ctrPos; float r;
         fin >> ctrPos.x >> ctrPos.y >> r;
         tmpCircle.setRadius(r);
         tmpCircle.setOrigin(r,r);
         tmpCircle.setPosition( ctrPos );
         circleVec.push_back( tmpCircle );
    }

    line1Vtx[0].color = line1Vtx[1].color =  line1Vtx[2].color =  line1Vtx[3].color = sf::Color::Magenta;

    fin >> line1Vtx[0].position.x >> line1Vtx[0].position.y;
    fin >> line1Vtx[1].position.x >> line1Vtx[1].position.y;
    fin >> line1Vtx[2].position.x >> line1Vtx[2].position.y;
    fin >> line1Vtx[3].position.x >> line1Vtx[3].position.y;
    line1Vtx[4].position = line1Vtx[1].position;
    line1Vtx[5].position = line1Vtx[2].position;
    line1Vtx[6].position = line1Vtx[3].position;
    line1Vtx[7].position = line1Vtx[0].position;

    fin.close();// end of barrier data

    sf::Text label("HIT", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    hitButt.init( 400.0f, 10.0f, 40.0f, 20.0f, nullptr, label );// hitButt.setSel(true);
    button::RegisteredButtVec.push_back( &hitButt );

    return true;
}

bool lvl_spark::handleEvent( sf::Event& rEvent )
{
    static bool shiftDn = false;

  /*  if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        float dAng = ( shiftDn ? 0.5f : 0.05f )*(float) rEvent.mouseWheelScroll.delta;
        theSA.fireAngle += dAng;

        float vx = theSA.fireDir.x*cosf(dAng) - theSA.fireDir.y*sinf(dAng);
        theSA.fireDir.y = theSA.fireDir.x*sinf(dAng) + theSA.fireDir.y*cosf(dAng);
        theSA.fireDir.x = vx;
        theSA.fireAngle = atan2f( theSA.fireDir.y, theSA.fireDir.x );
        if( pSB_angle ) pSB_angle->set_fVal( theSA.fireAngle );
    }   */
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = true;
     //   else if ( rEvent.key.code == sf::Keyboard::Up ) { if( pSB_grav ) pSB_grav->set_fVal( theSA.gravity.y + 0.001f ); }
     //   else if ( rEvent.key.code == sf::Keyboard::Down ) { if( pSB_grav ) pSB_grav->set_fVal( theSA.gravity.y - 0.001f ); }
     //   else if ( rEvent.key.code == sf::Keyboard::G ) { if( pSB_grav ) pSB_grav->set_fVal( 0.0f ); theSA.gravityOn = false; }
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = false;
    }

    (void) shiftDn;
    return true;
}

void lvl_spark::update( float dt )
{
 //   if( !button::pButtMse && button::clickEvent_Lt() == 1 ) fireSpot.setPosition( button::mseX, button::mseY );
    if( !button::pButtMse && button::clickEvent_Lt() == 1 )
    {
        fireSpot.setPosition( button::msePos() );
        theSA.emitPos = button::msePos();
        std::cout << "\nemitPos.x = " << button::mseX << " emitPos.y = " << button::mseY;
    }

    if( theSA.doFlip ) flipX( theSA.fireDir, dt, theSA.flipPeriod );
    else if( theSA.doSpin ) spin( theSA.fireDir, dt, 5.0f*theSA.flipPeriod );

    static float tElap = 0.0f;
    tElap += dt;
    if( tElap > theSA.emitPeriod )
    {
        tElap = 0.0f;
    //    if( pCurrOn && *pCurrOn )
        if( streamOn )
        {
         //   ++theSA.spkIt; if( theSA.spkIt == theSA.spkVec.end() ) theSA.spkIt = theSA.spkVec.begin();
            theSA.getNextSpark();
            theSA.spkIt->m = theSA.mass_all;

            if( massList.sel )
            {
                theSA.spkIt->reset( theSA.tLife_all, theSA.fireDir*theSA.fireSpeed, theSA.emitPos );
                static bool applyRatio = false;
                if( applyRatio ) theSA.spkIt->m *= massStrip.fVal;
                applyRatio = !applyRatio;
            }

            if( lineOn )
            {
                static int frmid = 0;
             //   if( !theSA.frIdxVec.empty() ) frmid = ( 1 + frmid )%theSA.frIdxVec.size();
                frmid = ( 1 + frmid )%dotSS.getFrCount(0);
                sf::Vector2f pos, vel;
                lineSource( lineSceVtx[0].position, lineSceVtx[1].position, 5.0f, 0.0f, pos, vel );
                theSA.spkIt->reset( theSA.tLife_all, vel, pos );
                theSA.spkIt->frIdx = frmid;
                theSA.spkIt->setTxtCoords( dotSS.getFrRect( frmid, 0 ) );
             //   theSA.spkIt->setColor( sf::Color(100,100,255) );
            }
            else if( randOn ) theSA.spkIt->emitRand( 3.0f, 12.0f, 5.0f, 40.0f, theSA.emitPos );
            else if( forceTestOn )// testing forces if list closed
            {
                static int FrId = frIdxFce;
                if( FrId == frIdxFce ) FrId = frIdxGrad;
                else if( FrId == frIdxGrad ) FrId = mapReady ? frIdxMap : frIdxFce;
                else if( FrId == frIdxMap ) FrId = frIdxFce;

                theSA.spkIt->frIdx = FrId;
                theSA.spkIt->setTxtCoords( dotSS.getFrRect( FrId, 0 ) );
                theSA.spkIt->reset( theSA.tLife_all, theSA.fireDir*theSA.fireSpeed, theSA.emitPos );
            }
            else// if( streamOn )// just stream
                theSA.spkIt->reset( theSA.tLife_all, theSA.fireDir*theSA.fireSpeed, theSA.emitPos );

      //      if( txtOnOffButt.sel ) theSA.spkIt->setColor( sf::Color::White );
        }
        else if( oneOn ) { oneOn = false; theSA.spkIt->reset( theSA.tLife_all, theSA.fireDir*theSA.fireSpeed, theSA.emitPos ); }
    }

    // apply forces
        sf::Vector2f acc( theSA.gravity );
        for( spark& spk : theSA.spkVec )
        {
            acc.x = 0.0f; acc.y = theSA.gravity.y*spk.m;
            if( applyForce )
            {
                if( !theSA.CFvec.empty() )
                {
                    sf::Vector2f pos = spk.getPosition();
                    if( !forceTestOn )
                        for( auto& fv : theSA.CFvec ) acc += fv.F( pos );
                    else if( spk.frIdx == frIdxFce )// red
                        for( auto& fv : theSA.CFvec ) acc += fv.F( pos );
                    else
                    {
                        if( spk.frIdx == frIdxGrad || !mapReady )
                            for( auto& fv : theSA.CFvec ) acc -= fv.gradU( pos, diff_ds );
                        else if( spk.frIdx == frIdxMap && mapReady )
                            acc += getAcc_map( pos );
                    }
                }

                if( crossList.sel )
                {
                    // a = k*( v.y, -v.x )
                    acc.x += crossStrip.fVal*spk.v.y;
                    acc.y -= crossStrip.fVal*spk.v.x;
                }

                if( dragList.sel ) acc -= dragStrip.fVal*spk.v;
            }

            spk.update( acc/spk.m, dt );
        }

        if( applyForce ) findAniArea();// update as needed


    if( hitButt.sel )
        for( spark& spk : theSA.spkVec )
            if( spk.tLife > 0.0f )
            {
                // vs lines
                for( auto it = lineVec.begin(); it != lineVec.end(); it += 2 )
                    spk.hitLine_2side( it->position, (it+1)->position, cr, dt );

                // vs circles
                for( const auto& crcl : circleVec )
                    spk.hitCircle( crcl.getPosition(), crcl.getRadius(), crBall, dt );

                // vs 1 side
                spk.hitLine_1side( line1Vtx[0].position, line1Vtx[1].position, crBall, dt );
                spk.hitLine_1side( line1Vtx[2].position, line1Vtx[3].position, crBall, dt );
                spk.hitLine_1side( line1Vtx[4].position, line1Vtx[5].position, crBall, dt );
                spk.hitLine_1side( line1Vtx[6].position, line1Vtx[7].position, crBall, dt );

                // vs floor annd walls
                if( spk.getPosition().y > Level::winH )// floor
                {
                    spk.setPosition( spk.getPosition().x, Level::winH );
                    if( spk.v.y > 0.0f ) spk.v.y *= -wallCr;
                }
                else if( spk.getPosition().x > Level::winW )// right wall
                {
                    spk.setPosition( Level::winW, spk.getPosition().y );
                    if( spk.v.x > 0.0f ) spk.v.x *= -wallCr;
                }
                else if( spk.getPosition().x < 0.0f )// left wall
                {
                    spk.setPosition( 0.0f, spk.getPosition().y );
                    if( spk.v.x < 0.0f ) spk.v.x *= -wallCr;
                }
            }

    return;
}

void lvl_spark::draw( sf::RenderTarget& RT ) const
{
    if( showArea )
    {
        if( mapReady ) RT.draw( aniArea, 5, sf::LinesStrip );
        else RT.draw( aniArea, 4, sf::Quads );
    }

    if( applyForce ) for( auto& cs : forceSpotVec ) RT.draw(cs);
    fireSpot.draw(RT);
    theSA.draw(RT);
 //   for( const spark& spk : sparkVec ) spk.draw(RT);

    if( hitButt.sel )
    {
        if( lineVec.size() > 0 ) RT.draw( &lineVec[0], lineVec.size(), sf::Lines );
        for( const auto& crcl : circleVec ) RT.draw( crcl );
        RT.draw( lineSceVtx, 2, sf::Lines );
        RT.draw( line1Vtx, 8, sf::Lines );
    }
    return;
}

void lvl_spark::save_sparkAni_toFile( const char* fname, const char* fnameSparks )
{
    std::ofstream fout( fname );
    if( !fout ) return;

    theSA.quadW_all = theSA.spkVec.empty() ? 4.0f : theSA.spkVec[0].quadW;
    theSA.doCross = crossList.sel; theSA.crossK = crossStrip.fVal;
    theSA.doDrag = dragList.sel; theSA.dragK = dragStrip.fVal;
    theSA.gravityOn = (theSA.gravity.y != 0.0f); theSA.gravity.x = 0.0f;
    theSA.mass_all = theSA.spkVec.empty() ? 1.0f : theSA.spkVec[0].m;
    theSA.charge_all = theSA.spkVec.empty() ? 1.0f : theSA.spkVec[0].q;

    theSA.findBounds();
    theSA.to_stream( fout );
    std::cout << "\nsaved as sparkAni";

    fout.close();

    if( fnameSparks ) theSA.saveSparks( fnameSparks );
}
// area is
//  0 1
//  3 2
void lvl_spark::findAniArea()// assigns to above 2 members
{
    if( !forceTestOn ) return;

    static bool aniAreaButtSel_last = false;

    if( aniAreaButt.sel != aniAreaButtSel_last )// begin or end event
    {
        if( aniAreaButt.sel )// begin
        {
            bool iValsFound = false;
            for( const spark& spk : theSA.spkVec )
                if( spk.tLife > 0.0f )
                {
                    sf::Vector2f spkPos = spk.getPosition();
                    if( !iValsFound )
                    {
                        iValsFound = true;
                        xMin_map = xMax_map = spkPos.x;
                        yMin_map = yMax_map = spkPos.y;
                        continue;
                    }
                    // bounds exist
                    if( spkPos.x < xMin_map ) xMin_map = spkPos.x;
                    else if( spkPos.x > xMax_map ) xMax_map = spkPos.x;

                    if( spkPos.y < yMin_map ) yMin_map = spkPos.y;
                    else if( spkPos.y > yMax_map ) yMax_map = spkPos.y;
                }

            showArea = iValsFound;
            mapReady = false;
            aniArea[0].color = aniArea[1].color = aniArea[2].color = aniArea[3].color =  aniArea[4].color = sf::Color(200,0,50,70);
        }
        else// end
        {
            std::cout << "\nadd buffer and find force map";
            float buffZone = 10.0f;
            xMin_map -= buffZone; yMin_map -= buffZone;
            xMax_map += buffZone; yMax_map += buffZone;

            aniArea[0].color = aniArea[1].color = aniArea[2].color = aniArea[3].color =  aniArea[4].color = sf::Color(255,255,255);
            aniArea[0].position.x = xMin_map; aniArea[0].position.y = yMin_map;// up lt
            aniArea[1].position.x = xMax_map; aniArea[1].position.y = yMin_map;// up rt
            aniArea[2].position.x = xMax_map; aniArea[2].position.y = yMax_map;// dn rt
            aniArea[3].position.x = xMin_map; aniArea[3].position.y = yMax_map;// dn lt
            aniArea[4].position = aniArea[0].position;

            findForceMap();
        }

        aniAreaButtSel_last = aniAreaButt.sel;// update
    }


    if( showArea && aniAreaButt.sel )
    {
        for( const spark& spk : theSA.spkVec )
            if( spk.tLife > 0.0f )
            {
                sf::Vector2f spkPos = spk.getPosition();
                // bounds exist
                if( spkPos.x < xMin_map ) xMin_map = spkPos.x;
                else if( spkPos.x > xMax_map ) xMax_map = spkPos.x;

                if( spkPos.y < yMin_map ) yMin_map = spkPos.y;
                else if( spkPos.y > yMax_map ) yMax_map = spkPos.y;
            }

        aniArea[0].position.x = xMin_map; aniArea[0].position.y = yMin_map;// up lt
        aniArea[1].position.x = xMax_map; aniArea[1].position.y = yMin_map;// up rt
        aniArea[2].position.x = xMax_map; aniArea[2].position.y = yMax_map;// dn rt
        aniArea[3].position.x = xMin_map; aniArea[3].position.y = yMax_map;// dn lt
    }

    return;
}

void lvl_spark::findForceMap()
{
    float W = xMax_map - xMin_map, H = yMax_map - yMin_map;
    cols_map = 1 + floor(W/ds_map); rows_map = 1 + (H/ds_map);
    forceMap.clear();
    forceMap.resize( rows_map*cols_map );// for rows-1 x cols-1 region within
    std::cout << "\nrows_map = " << rows_map << " cols_map = " << cols_map << " map sz = " << sizeof( sf::Vector2f )*forceMap.size();

    for( int r = 0; r < rows_map; ++r )
    {
        for( int c = 0; c < cols_map; ++c )
        {
            sf::Vector2f pos = sf::Vector2f( xMin_map + c*ds_map, yMin_map + r*ds_map );
            forceMap[r*cols_map+c] *= 0.0f;
            for( auto& fv : theSA.CFvec ) forceMap[r*cols_map+c] += fv.F( pos );
        }
    }

    xMin_map -= fireSpot.pos.x; yMin_map -= fireSpot.pos.y;
    xMax_map -= fireSpot.pos.x; yMax_map -= fireSpot.pos.y;
    mapReady = true;
}

sf::Vector2f lvl_spark::getAcc_map( sf::Vector2f pos )
{
    pos -= fireSpot.pos;
    const sf::Vector2f zero;
    if( pos.x < xMin_map || pos.x > xMax_map ) return zero;
    if( pos.y < yMin_map || pos.y > yMax_map ) return zero;

    int r = floor( (pos.y-yMin_map)/ds_map ), c = floor( (pos.x-xMin_map)/ds_map );
    if( r < 0 || r + 2 > rows_map || c < 0 || c + 2 > cols_map ) { return zero; }

    float dx = pos.x - xMin_map - ds_map*c;
    float dy = pos.y - yMin_map - ds_map*r;
    sf::Vector2f dVx = forceMap[r*cols_map+c+1] - forceMap[r*cols_map+c], dVy = forceMap[(r+1)*cols_map+c] - forceMap[r*cols_map+c];

    return forceMap[r*cols_map+c] + ( dx*dVx + dy*dVy )/ds_map;
}

void lvl_spark::cleanup()
{
 //   for( auto pCF : pCFvec ) delete pCF;
    return;
}

/*
void lvl_spark::findAniArea()// assigns to above 2 members
{
    // find bounds
    float &xMin = aniArea[0].position.x, &xMax = aniArea[1].position.x;
    float &yMin = aniArea[0].position.y, &yMax = aniArea[3].position.y;
    bool iValsFound = false;
    for( const spark& spk : sparkVec )
        if( spk.tLife > 0.0f )
        {
            sf::Vector2f spkPos = spk.getPosition();
            if( !iValsFound )
            {
                iValsFound = true;
                xMin = xMax = spkPos.x;
                yMin = yMax = spkPos.y;
                continue;
            }
            // bounds exist
            if( spkPos.x < xMin ) xMin = spkPos.x;
            else if( spkPos.x > xMax ) xMax = spkPos.x;

            if( spkPos.y < yMin ) yMin = spkPos.y;
            else if( spkPos.y > yMax ) yMax = spkPos.y;
        }

    showArea = iValsFound;
    if( showArea )
    {
     //   aniArea[0].position // xMin, yMin
        aniArea[1].position.y = yMin;// xMax
        aniArea[3].position.x = xMin;// yMax
        aniArea[2].position.x = xMax;
        aniArea[2].position.y = yMax;
    }
    return;
}   */
