#include "lvl_sparkAni.h"

const float myPI = 3.1415927f;
const float myPIx2 = 2.0f*myPI;

bool lvl_sparkAni::init()
{
    Level::goto_MMButt.setPosition( {Level::winW-160.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW-80.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
 //   Level::clearColor = sf::Color(50,50,50);



    if( !LoadBarriers() ) { std::cout << "no barriers data\n"; return false; }
  //  std::cerr << "\nGot here!";

    std::ifstream fin( "images/dots9_SSdata.txt" );
    if( !fin ) { std::cout << "no dot SS data\n"; return false; }
    dotSS.init(fin);
    fin.close();

 /*   fin.open( "include/level/lvl_sparkAni_data/ani_list.txt" );
    if( !fin ) return false;

    size_t numAnis; fin >> numAnis;
    pSpkAniVec.reserve(numAnis+1);
    for( size_t i=0; i < numAnis; ++i )
    {
        std::string fnameStr;
        fin >> fnameStr;
        sf::Vector2f pos; fin >> pos.x >> pos.y;
        sparkAni* pSA = new sparkAni;
        if( !pSA->init( pos, &dotSS, fnameStr.c_str() ) ) return false;
        pSpkAniVec.push_back( pSA );
    }
    fin.close();    */

    sf::Text label("LOAD", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    const char* fNameBase = "include/level/lvl_sparkAni_data/";
    std::string fNameStr = fNameBase;
 //   fNameStr += "savedAnis/SSAspark_data.txt";
 //   std::function<void(void)> pF = [this,fNameStr]() { if( !pSpkAniVec.empty() ) pSpkAniVec[0]->loadSparks( fNameStr.c_str() ); };
 //   LoadSparksNowButt.init( 250.0f, 10.0f, 60.0f, 25.0f, pF, label );
 //   button::RegisteredButtVec.push_back( &LoadSparksNowButt );

    fin.open( "include/level/lvl_sparkAni_data/firePattern/BKGDimage.txt" );
    if( !fin ) return false;
    std::string imgFname;
    getline( fin, imgFname );
    if( !BkgdImg.loadFromFile( imgFname.c_str()) ) return false;
    BkgdImg.createMaskFromColor( sf::Color::White );
    if( !BkgdTxt.loadFromImage( BkgdImg ) ) return false;
    float posX, posY; fin >> posX >> posY;
    float scale; fin >> scale;
    fin.close();
    BkgdSpr.setTexture( BkgdTxt );
    BkgdSpr.setScale( scale, scale );
    BkgdSpr.setPosition( posX, posY );

    fin.open( "include/level/lvl_sparkAni_data/printer_data.txt" );
    if( !fin ) return false;
 //   const float my_pi = 3.1415927f;
    fin >> imgFname;
    size_t Nsparks = 200; fin >> Nsparks;// for printer
    fin >> posX >> posY;
    float speed, dX, dY, spkSz; fin >> speed >> dX >> dY >> spkSz;
    float tlife; fin >> tlife;
    float angle; fin >> angle;
    std::cout << "\nangle = " << angle;
    fin.close();
    angle *= myPI/180.0f;
    sf::Vector2f Dir( cosf(angle), sinf(angle) );
    if( !spkPrnt.init( sf::Vector2f( posX, posY ), imgFname.c_str(), Dir, speed, dX, dY, tlife, spkSz, Nsparks ) ) return false;

    fin.open( "include/level/lvl_sparkAni_data/init_data.txt" );
    if( !fin ) return false;
    // background color
    unsigned int rd, gn, bu;
    fin >> rd >> gn >> bu;
    Level::clearColor = sf::Color(rd,gn,bu);
    // forces
    fin >> posX >> posY;
    if( !theFC.init( sf::Vector2f(Level::winW-posX,posY), "include/level/lvl_sparkAni_data/ForceControl_data.txt" ) ) return false;
    button::RegisteredButtVec.push_back( &theFC.Csurf );
    // printer
    fin >> posX >> posY;
    if( !thePrinter.init( sf::Vector2f(posX,posY), spkPrnt, "include/level/lvl_sparkAni_data/PrinterControl_data.txt" ) ) return false;
    button::RegisteredButtVec.push_back( &thePrinter.Csurf );

    // a bunch of sparks
    sf::Vector2f emitPos; fin >> emitPos.x >> emitPos.y;
    Nsparks = 300; fin >> Nsparks;// for emitter
    size_t i = 0;
    sparkVec.resize( Nsparks );
    for( spark& spk : sparkVec )
    {
        size_t idx = i%dotSS.getFrCount(0);
        spk.init( 1.0f, sf::Vector2f(0.0f,0.0f), sf::Vector2f(0.0f,0.0f), idx, -1.0f );
        spk.setTxtCoords( dotSS.getFrRect( idx, 0 ) );
        ++i;
    }
    sparkIt = sparkVec.begin();

    // emitter
    std::function<spark*(void)> pGetSpark = [this]() { return getNextSpark(); };
    std::function<spark*(float&)> pGetInitSpark = [this](float& emitDt) { return emitControl.getReadySpark( emitDt ); };

    fin >> posX >> posY;
    fin >> cr >> graphFuncs::hitOffset; std::cout << "\ncr = " << cr << " hitOffset = " << graphFuncs::hitOffset;

    // a bit more data
    float buttW, buttH; fin >> buttW >> buttH;
    sf::Vector2f fwButtPos, emitButtPos, axesButtPos, hitOfstStripPos, massBsPos;
    fin >> fwButtPos.x >> fwButtPos.y;
    fin >> emitButtPos.x >> emitButtPos.y;
    fin >> axesButtPos.x >> axesButtPos.y;
    fin >> hitOfstStripPos.x >> hitOfstStripPos.y;
    float hoMin, hoMax, hoInit; fin >> hoMin >> hoMax >> hoInit;
    fin >> massBsPos.x >> massBsPos.y;
    float  minVal, maxVal, iVal; fin >>  minVal >> maxVal >> iVal;
    massBS.init( massBsPos, sf::Vector2f(70.0f,8.0f), 120.0f, 20.0f, "MassRatio", "xM0", minVal, maxVal, iVal, nullptr );
    button::RegisteredButtVec.push_back( &massBS );
    fin.close();// closing init_data.txt

 //   label.setString("hit offset");
 //   hitOfstStrip.init( hitOfstStripPos.x, hitOfstStripPos.y, 140.0f, 20.0f, label, hoMin, hoMax, hoInit, &graphFuncs::hitOffset );
    label.setString("coef rest");
    hitOfstStrip.init( hitOfstStripPos.x, hitOfstStripPos.y, 140.0f, 20.0f, label, 0.1, 1.0, 0.8, &cr );
    button::RegisteredButtVec.push_back( &hitOfstStrip );

    if( !emitControl.init( sf::Vector2f(posX,posY), "include/level/lvl_sparkAni_data/emitControl.txt", spkEmitter, pGetSpark,
     [this](float x){ for( auto& spk : sparkVec ) spk.setSize(x); }, "include/level/lvl_sparkAni_data/firePattern/FP1.txt" ) ) return false;
    spkEmitter.init( emitPos, false, pGetInitSpark, &dotSS );
    button::RegisteredButtVec.push_back( &emitControl.Csurf );

    emitMark.setFillColor( sf::Color::Cyan );
    emitMark.setRadius(4.0f);
    emitMark.setOrigin(4.0f,4.0f);
    emitMark.setPosition( spkEmitter.emitPos );

    // color!
    theFC.Csurf.bkRect.setFillColor( sf::Color(0,128,128) );
    theFC.subSurf1.bkRect.setFillColor( sf::Color(0,64,128) );
    thePrinter.Csurf.bkRect.setFillColor( sf::Color(128,0,255) );
    emitControl.Csurf.bkRect.setFillColor( sf::Color(0,0,128) );

    const char* fName = "include/level/lvl_sparkAni_data/fireWork_data.txt";
    fin.open( fName );
    if( !fin ) return false;
 //   fireworkInit( fin );
    std::function<spark*(void)> GetSpark = [this](){ return getNextSpark(); };
    std::function<sf::IntRect(size_t)> GetFrRect = [this](size_t i){ return dotSS.getFrRect(i,0); };
    fin >> fireOfst.x >> fireOfst.y;
    fin >> fwPos1.x >> fwPos1.y;
    fw1.init( fwPos1, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin >> fwPos2.x >> fwPos2.y;
    fw2.init( fwPos2, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin >> fwPos3.x >> fwPos3.y;
    fw3.init( fwPos3, fin, GetSpark, GetFrRect, &anyLive_emit );
    fin.close();

    label.setString("FireWork");
    fireWorkButt.init( fwButtPos.x, fwButtPos.y, buttW, buttH, nullptr, label );
    button::RegisteredButtVec.push_back( &fireWorkButt );
    fireWorkButt.pHitFunc = [this,fName,GetSpark,GetFrRect]()
    {
        if( fireWorkButt.sel ) {
        std::ifstream Fin( fName );
        Fin >> fireOfst.x >> fireOfst.y;
        Fin >> fwPos1.x >> fwPos1.y;
        fw1.init( fwPos1, Fin, GetSpark, GetFrRect, &anyLive_emit );
        Fin >> fwPos2.x >> fwPos2.y;
        fw2.init( fwPos2, Fin, GetSpark, GetFrRect, &anyLive_emit );
        Fin >> fwPos3.x >> fwPos3.y;
        fw3.init( fwPos3, Fin, GetSpark, GetFrRect, &anyLive_emit );
        fw1.doFire = fw2.doFire = fw3.doFire = true; }
    };

    theFC.gravBS.voh.set_fVal( fw1.fwGrav );
    theFC.gravBS.setSel(true);
    theFC.subSurf1.setSel(true);

     // emit from graphFuncs
     const char* fNameEmit = "include/level/lvl_sparkAni_data/gfEmit_data.txt";
    fin.open( fName );
    if( !fin ) return false;
    gfEmit.init( fin );
    fin.close();
    label.setString("gfEmit");
    gfEmitButt.init( emitButtPos.x, emitButtPos.y, buttW, buttH, nullptr, label );
    button::RegisteredButtVec.push_back( &gfEmitButt );

    gfEmitButt.pHitFunc = [this,fNameEmit]()
    {
        if( !gfEmitButt.sel ) return;
        std::ifstream Fin( fNameEmit );
        size_t i = 0;
        for( i = 0; i < pGfVec.size(); ++i ) if( CsurfVec[i].sel ) { gfEmit.init( *pGfVec[i], Fin ); break; }
    //    if( gfSurf_p.sel ) gfEmit.init( gfPoly, Fin );
     //   else if( gfSurf_s.sel ) gfEmit.init( gfSine, Fin );
     //   else if( gfSurf_E.sel ) gfEmit.init( gfEllip, Fin );
        if( i == pGfVec.size() ) gfEmitButt.setSel( false );
    };

    // toggle axes view
    label.setString("axes on/off");
    showAxesButt.init( axesButtPos.x, axesButtPos.y, buttW, buttH, nullptr, label );
    showAxesButt.mode = 'I';
    button::RegisteredButtVec.push_back( &showAxesButt );
    showAxesButt.pHitFunc = [this]()
    {
        for( size_t i = 0; i < CsurfVec.size(); ++i )
            if( CsurfVec[i].sel ) { pGfVec[i]->showAxes = !pGfVec[i]->showAxes; break; }
    //    if( gfSurf_p.sel ) gfPoly.showAxes = !gfPoly.showAxes;
    //    else if( gfSurf_s.sel ) gfSine.showAxes = !gfSine.showAxes;
     //   else if( gfSurf_E.sel ) gfEllip.showAxes = !gfEllip.showAxes;
    };

    // line for gf normal
    Nline[0].color = Nline[1].color = sf::Color::Green;

    return true;
}

spark* lvl_sparkAni::getNextSpark()
{
    if( ++sparkIt == sparkVec.end() ) sparkIt = sparkVec.begin();
    anyLive_emit = true;
    sparkIt->vtx[0].color.a = sparkIt->vtx[1].color.a = sparkIt->vtx[2].color.a = sparkIt->vtx[3].color.a = 255;
    sparkIt->setSize( emitControl.sparkSzStrip.fVal );
 //   sparkIt->vtx[0].color.a = 255;
    return &(*sparkIt);
}

bool lvl_sparkAni::handleEvent( sf::Event& rEvent )
{
    static bool shiftDn = false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = true;
        else if ( rEvent.key.code == sf::Keyboard::S ) thePrinter.theStrip.set_fVal(4.56f);
        else if ( rEvent.key.code == sf::Keyboard::N ) seeLine = true;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = false;
        else if ( rEvent.key.code == sf::Keyboard::N ) seeLine = false;
    }

    (void)shiftDn;
    return true;
}

void lvl_sparkAni::dragSparkAnis()
{
    static sf::Vector2f grabPos;
    static sparkAni* pToSA = nullptr;
    if( button::clickEvent_Lt() == 1 )// grab
    {
        for( sparkAni* pSA : pSpkAniVec )
            if( pSA->hit( button::msePos() ) )
            {
                pToSA = pSA;
                grabPos = button::msePos() - pToSA->getPosition();
                break;
            }
    }
    else if( button::clickEvent_Lt() == -1 )
        pToSA = nullptr;// release
    else if( pToSA && button::mseDnLt && button::mseMoved )// drag
        pToSA->setPosition( button::msePos() - grabPos );
}

void lvl_sparkAni::updateSparks( std::vector<spark>& spkVec, bool& anyLive, float dt )
{
    if( !anyLive ) return;
    bool applyForce = theFC.subSurf1.sel || theFC.forceList.sel;
    anyLive = false;
    for( spark& spk : spkVec )
        if( spk.tLife > 0.0f )
        {
            if( applyForce ) spk.update( theFC.totalForce( spk.getPosition(), spk.v, spk.m ), dt );
            else spk.update( dt );
            anyLive = true;
        }
}

void lvl_sparkAni::update( float dt )// position update and collision testing
{
    if( !button::pButtMse && button::clickEvent_Lt() == 1 )// Left mse press
    {
        if( theFC. dragCondition() )// move force centers
            std::cout << "\nmove fce ctr";
        else if( emitControl.Csurf.sel )// move emitter
        {
            spkEmitter.emitPos = button::msePos();
            std::cout << "\nemitPos: " << spkEmitter.emitPos.x << ", " << spkEmitter.emitPos.y;
            emitMark.setPosition( button::msePos() );
        }
        else if( thePrinter.Csurf.sel )// move print box
            spkPrnt.setPosition( button::msePos() );
    }

    static std::default_random_engine rand_gen;// for spark frIdx, fire work position

    if( spkEmitter.update(dt) )
    {
        static std::uniform_int_distribution<int> frIdx_dist( 0, dotSS.getFrCount(0)-1 );
        sparkIt->frIdx = frIdx_dist(rand_gen);
        sparkIt->setTxtCoords( dotSS.getFrRect( sparkIt->frIdx, 0 ) );
        sparkIt->m = 1.0f;
        if( massBS.sel ) sparkIt->m += massBS.voh.fVal*sparkIt->frIdx;
    }
 //   spkEmitter.update(dt);
    thePrinter.update(dt);
//    gfEmitUpdate(dt);

    size_t idxSel = 0;
    for( idxSel = 0; idxSel < CsurfVec.size(); ++idxSel ) if( CsurfVec[idxSel].sel ) break;
 //   if( gfEmitButt.sel && idxSel < CsurfVec.size() )
    if( idxSel < CsurfVec.size() )
    {
        sf::Vector2f spkPos, spkVel;
        // old
    /*    if( gfEmitButt.sel && gfEmit.update( *pGfVec[idxSel], spkPos, spkVel, dt ) )
        {
            spark* pSpk = getNextSpark();
            pSpk->v = spkVel; pSpk->setPosition( spkPos );
            pSpk->tLife = gfEmit.tLifeEmit;
        }   */

        // new
        if( gfEmitButt.sel )
        {
            gfEmit.tLifeEmit = emitControl.tLifeStrip.fVal;
            gfEmit.tEmit = emitControl.firePeriodStrip.fVal;
            gfEmit.emitSpeed = emitControl.speedSlider.fVal;
            if( gfEmit.update( *pGfVec[idxSel], spkPos, spkVel, dt ) )
            {
                spark* pSpk = getNextSpark();
                pSpk->v = spkVel; pSpk->setPosition( spkPos );
                pSpk->tLife = gfEmit.tLifeEmit;
            }
        }


        if( seeLine )// view normal to gf curve
        {
            sf::Vector2f Ncurve;
            Nline[1].position = button::msePos();
            spkPos = Nline[1].position - pGfVec[idxSel]->pos;
            spkPos = pGfVec[idxSel]->toBase( spkPos );
            if( pGfVec[idxSel]->coordType == 'P' ) spkPos.x = atan2f( spkPos.y, spkPos.x );
            spkPos.x += pGfVec[idxSel]->x0;
            if( pGfVec[idxSel]->getGlobalMotion( spkPos.x, Nline[0].position, spkVel, Ncurve ) )
                Nline[1].position = Nline[0].position + 20.0f*Ncurve;
            else
                Nline[0].position.x = -1.0f;
        }
    }

    theFC.dragForceCenters();
//    fireworkUpdate(dt);
    if( fireWorkButt.sel )
    {

        static std::uniform_real_distribution<float> pos_dist(-1.0f,1.0f);
        if( !fw1.pShell ) { fw1.firePos = fwPos1 + fireOfst*pos_dist(rand_gen); fw1.doFire = true; }
        if( !fw2.pShell ) { fw2.firePos = fwPos2 + fireOfst*pos_dist(rand_gen); fw2.doFire = true; }
        if( !fw3.pShell ) { fw3.firePos = fwPos3 + fireOfst*pos_dist(rand_gen); fw3.doFire = true; }
    }
    fw1.update(dt);
    fw2.update(dt);
    fw3.update(dt);

    updateSparks( spkPrnt.spkVec, spkPrnt.anyLive, dt );
    updateSparks( sparkVec, anyLive_emit, dt );

    if( hitList.sel )
    {
        for( auto& pGF : pGfVec ) pGF->update(dt);
        pulseTime_e2 += dt;
        if( pulseTime_e2*angFreq_e2 > myPIx2 ) pulseTime_e2 -= myPIx2/angFreq_e2;

        if( spkPrnt.anyLive )
            for( spark& spk : spkPrnt.spkVec )
                if( spk.tLife > 0.0f )
                {
                    for( auto it = lineVec.begin(); it != lineVec.end(); it += 2 )
                        if( spk.hitLine_2sideA( it->position, (it+1)->position, cr, dt ) ) break;
                }

        if( anyLive_emit )
            for( spark& spk : sparkVec )
                if( spk.tLife > 0.0f )
                {
                    for( auto it = lineVec.begin(); it != lineVec.end(); it += 2 )
                        if( spk.hitLine_2sideA( it->position, (it+1)->position, cr, dt ) ) break;
                }

        // vs gFunc
        if( !( theFC.Csurf.sel || thePrinter.Csurf.sel || emitControl.Csurf.sel ) )
           for( size_t i = 0; i < pGfVec.size(); ++i ) if( CsurfVec[i].sel ) pGfVec[i]->drag();

    //    for( auto& pGF : pGfVec ) pGF->update(dt);

        if( anyLive_emit )
        for( spark& spk : sparkVec )
            if( spk.tLife > 0.0f )
            {
                sf::Vector2f Pos = spk.getPosition();
                for( size_t i = 0; i < pGfVec.size(); ++i )
                    if( CsurfVec[i].sel && pGfVec[i]->hit( Pos, spk.v, cr, dt ) ) { spk.setPosition( Pos ); break; }
            }

        if( spkPrnt.anyLive )
        for( spark& spk : spkPrnt.spkVec )
            if( spk.tLife > 0.0f )
            {
                sf::Vector2f Pos = spk.getPosition();
             //   bool Hit = false;
                for( size_t i = 0; i < pGfVec.size(); ++i )
                    if( CsurfVec[i].sel && pGfVec[i]->hit( Pos, spk.v, cr, dt ) ) { spk.setPosition( Pos ); break; }
             //   if( gfSurf_p.sel && gfPoly.hit( Pos, spk.v, cr, dt ) ) Hit = true;
             //   else if( gfSurf_s.sel && gfSine.hit( Pos, spk.v, cr, dt ) ) Hit = true;

              //  if( Hit ) spk.setPosition( Pos );
            }

        // vs circles
        for( const auto& crcl : circleVec )
        {
            if( spkPrnt.anyLive )
            for( spark& spk : spkPrnt.spkVec )
                if( spk.tLife > 0.0f ) spk.hitCircle( crcl.getPosition(), crcl.getRadius(), 0.6f, dt );

            if( anyLive_emit )
            for( spark& spk : sparkVec )
                if( spk.tLife > 0.0f ) spk.hitCircle( crcl.getPosition(), crcl.getRadius(), 0.6f, dt );
        }
    }

    return;
}

void lvl_sparkAni::draw( sf::RenderTarget& RT ) const
{
 //   for( sparkAni* pSA : pSpkAniVec ) pSA->draw(RT);

    if( emitControl.open && emitControl.fireFuncButt.sel ) RT.draw( BkgdSpr );
    if( anyLive_emit ) for( const spark& spk : sparkVec ) spk.draw( RT, &dotSS.txt );

    if( theFC.forceList.sel ) for( auto& cs : theFC.forceSpotVec ) RT.draw(cs);
    RT.draw( emitMark );

    if( hitList.sel )
    {
        if( lineVec.size() > 0 ) RT.draw( &lineVec[0], lineVec.size(), sf::Lines );
        for( const auto& crcl : circleVec ) RT.draw( crcl );
        for( size_t i = 0; i < pGfVec.size(); ++i ) if( CsurfVec[i].sel ) pGfVec[i]->draw(RT);

        if( seeLine && Nline[0].position.x >= 0.0f ) RT.draw( Nline, 2, sf::Lines );
    }

    return;
}

void lvl_sparkAni::cleanup()
{
    for( sparkAni* pSA : pSpkAniVec ) delete pSA;
    pSpkAniVec.clear();
    return;
}

bool lvl_sparkAni::LoadBarriers()
{
    std::ifstream fin("include/level/lvl_sparkAni_data/barrier_data.txt");
    if( !fin ) { std::cout << "\nNo spark barrier_data"; return false; }

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

    fin.close();// end of barrier data

    fin.open( "include/level/lvl_sparkAni_data/gfPoly.txt" );// i = 0
    if( !fin ) { std::cout << "no gfPoly data\n"; return false; }
    poly1.init(fin);
    fin >> angFreq_p;
    std::function<float(float)> Fyt = [](float t) { return 1.0f; };// static
    std::function<float(float,float)> pFy = nullptr;//[this,Fyt](float x,float t) { return Fyt(t)*poly1.y(x); };
    std::function<float(float,float)> Fdydt = nullptr;
    std::cerr << "\nAF = " << angFreq_p;
    gfPoly.tPeriod = angFreq_p == 0.0f ? -1.0f : myPIx2/angFreq_p;
    std::cerr << "\nTP = " << gfPoly.tPeriod;
    Fyt = [this](float t) { return cosf( angFreq_p*t ); };
    pFy = [this,Fyt](float x,float t) { return Fyt(t)*poly1.y(x); };
    Fdydt = [this,pFy](float x,float t) { return -angFreq_p*sinf(angFreq_p*t)*poly1.y(x); };
    std::function<float(float,float)> pFyPm = [this,Fyt](float x,float t) { return Fyt(t)*poly1.yPm(x); };
    std::function<void(float&,float&,float,float)> Fy_yPm = [this,Fyt](float& y, float& ypm, float x, float t) { poly1.y_yPm(y,ypm,x); y *= Fyt(t); ypm *= Fyt(t); };
    gfPoly.init( fin, Fy_yPm, Fdydt );
    fin.close();

    fin.open( "include/level/lvl_sparkAni_data/gfSine.txt" );// i = 1
    if( !fin ) { std::cout << "no gfSine data\n"; return false; }
    fin >> Amp_s >> wvLen_s >> gfSine.dxdt;
    pFy = [this](float x,float t) { return Amp_s*sinf( myPIx2*(x-t*gfSine.dxdt)/wvLen_s ); };
    pFyPm = [this](float x,float t) { return Amp_s*myPIx2*cosf( myPIx2*(x-t*gfSine.dxdt)/wvLen_s )/wvLen_s; };
    Fy_yPm = [this,pFy,pFyPm](float& y, float& ypm, float x, float t) { y = pFy(x,t); ypm = pFyPm(x,t); };
    Fdydt = [](float x,float t){ return 0.0f; };
    gfSine.gfColor = sf::Color(0,200,64);
    gfSine.init( fin, Fy_yPm, Fdydt );
    gfSine.tPeriod = wvLen_s/gfSine.dxdt;
    fin.close();

    fin.open( "include/level/lvl_sparkAni_data/gfHyper.txt" );// i = 1
    if( !fin ) { std::cout << "no gfSine data\n"; return false; }
    fin >> hypA >> hypB;
    pFy = [this](float x,float t) { return sqrtf( hypB*x*x + hypA ); };
    pFyPm = [this,pFy](float x,float t) { return hypB*x/pFy(x,t); };
 //   Fy_yPm = [this,pFy,pFyPm](float& y, float& ypm, float x, float t) { y = pFy(x,t); ypm = hypB*x/y; };
    Fy_yPm = [this,pFy,pFyPm](float& y, float& ypm, float x, float t) { y = pFy(x,t); ypm = hypB*x/y; if( !topBranch ) { y *= -1.0f; ypm *= -1.0f; } };
    Fdydt = nullptr;
    gfHyper.gfColor = sf::Color(0,100,200);
    gfHyper.init( fin, Fy_yPm, Fdydt );
    gfHyper.tPeriod = -1.0f;
    fin.close();

    fin.open( "include/level/lvl_sparkAni_data/gfFlower.txt" );// i = 2
    if( !fin ) { std::cout << "no gfFlower data\n"; return false; }
    fin >> numPetals >> R_Flwr >> Rin_Flwr >> gfFlower.dxdt;
    std::function<float(float,float)> pFxt_X = [this](float x,float t) { return x-t*gfFlower.dxdt; };
    pFy = [this,pFxt_X](float x,float t) { float X = pFxt_X(x,t); float r = R_Flwr*sinf( numPetals*X );  return r > Rin_Flwr ? r : Rin_Flwr; };
    pFyPm = [this,pFxt_X](float x,float t) { float X = pFxt_X(x,t); float r = R_Flwr*sinf( numPetals*X ); return r > Rin_Flwr ? numPetals*R_Flwr*cosf( numPetals*X ) : 0.0f; };
    Fdydt = nullptr;
    gfFlower.gfColor = sf::Color::Magenta;
    gfFlower.init( fin, pFy, pFyPm, Fdydt );
    fin.close();

    // regular polygon
    fin.open( "include/level/lvl_sparkAni_data/gfPolygonReg.txt" );// i = 0
    if( !fin ) { std::cout << "no gfPolygonReg data\n"; return false; }
    pgReg.init(fin);
    gfPolygon.tPeriod = -1.0f;
    Fy_yPm = [this](float& y, float& ypm, float x, float t) { float X = x-t*gfPolygon.dxdt; pgReg.r_rPm(y,ypm,X); };
    gfPolygon.gfColor = sf::Color::Yellow;
    gfPolygon.fnumPts = [this](void) { return pgReg.numPts(); };
    gfPolygon.fPt_n = [this](size_t i) { return pgReg.ptPos(i); };
    gfPolygon.init( fin, Fy_yPm, nullptr );
    fin.close();

    // Irregular polygon
    fin.open( "include/level/lvl_sparkAni_data/gfPolyIR.txt" );// i = 0
    if( !fin ) { std::cout << "no gfPolyIR data\n"; return false; }
    pgIrreg.init(fin);
    gfPolyIR.tPeriod = -1.0f;
    Fy_yPm = [this](float& y, float& ypm, float x, float t) { float X = x-t*gfPolyIR.dxdt; pgIrreg.r_rPm(y,ypm,X); };
    gfPolyIR.gfColor = sf::Color::Red;
    gfPolyIR.fnumPts = [this](void) { return pgIrreg.numPts(); };
    gfPolyIR.fPt_n = [this](size_t i) { return pgIrreg.ptPos(i); };
    gfPolyIR.init( fin, Fy_yPm, nullptr );
    fin.close();

    fin.open( "include/level/lvl_sparkAni_data/gfEllip.txt" );// i = 3
    if( !fin ) { std::cout << "no gfEllip data\n"; return false; }
    fin >> elip_a >> elip_b;
    std::function<float(float)> pG = [this](float x) { float ac = elip_a*sinf(x), bc = elip_b*cosf(x); return ac*ac + bc*bc; };
    pFy = [this,pG](float x,float t) { float X = x-t*gfEllip.dxdt; return elip_a*elip_b/sqrtf( pG(X) ); };
    pFyPm = [this,pG](float x,float t) { float X = x-t*gfEllip.dxdt; return 0.5f*elip_a*elip_b*( elip_b*elip_b-elip_a*elip_a )*sinf(2.0f*X)/powf( pG(X), 1.5f ); };
    Fdydt = nullptr;
    gfEllip.init( fin, pFy, pFyPm, Fdydt );
    fin.close();

    fin.open( "include/level/lvl_sparkAni_data/gfEllip2.txt" );// i = 4
    if( !fin ) { std::cout << "no gfEllip2 data\n"; return false; }
    fin >> elip_a2 >> elip_b2;
    pG = [this](float x) { float ac = elip_a2*sinf(x), bc = elip_b2*cosf(x); return ac*ac + bc*bc; };
    pFyPm = [this,pG](float x,float t) { float X = x-t*gfEllip2.dxdt; return 0.5f*elip_a2*elip_b2*( elip_b2*elip_b2-elip_a2*elip_a2 )*sinf(2.0f*X)/powf( pG(X), 1.5f ); };
    std::function<float(float)> pFy_x = [this,pG](float x) { return elip_a2*elip_b2/sqrtf( pG(x) ); };
    std::function<float(void)> pFy_t = [this](void) { return Amp_e2*sinf( angFreq_e2*pulseTime_e2 ); };
    std::function<float(float,float)> pFy_xt = [this,pFy_x,pFy_t](float x,float t) { float X = x-t*gfEllip2.dxdt; return pFy_t() + pFy_x(X); };
    Fdydt = [this](float x,float t) { return Amp_e2*angFreq_e2*cosf(angFreq_e2*pulseTime_e2); };
    gfEllip2.init( fin, pFy_xt, pFyPm, Fdydt );
    fin.close();


    // controls for the GFs
    const size_t numGF = 8;
    pGfVec.reserve(numGF);
    pGfVec.push_back( &gfPoly );
    pGfVec.push_back( &gfSine );
    pGfVec.push_back( &gfHyper );
    pGfVec.push_back( &gfFlower );
    pGfVec.push_back( &gfPolygon );
    pGfVec.push_back( &gfPolyIR );
    pGfVec.push_back( &gfEllip );
    pGfVec.push_back( &gfEllip2 );

    CsurfVec.resize(numGF);
    MsVec.resize(numGF);
    VohVec.resize(numGF);
    SbVec.resize(numGF);

    std::vector<std::string> strLabelVec;
    strLabelVec.reserve(numGF);
    strLabelVec.push_back( "poly" );
    strLabelVec.push_back( "sine" );
    strLabelVec.push_back( "hyper" );
    strLabelVec.push_back( "flower" );
    strLabelVec.push_back( "regPoly" );
    strLabelVec.push_back( "PolyIR" );
    strLabelVec.push_back( "ellip" );
    strLabelVec.push_back( "ellip2" );

    std::vector<std::string> fNameVec;
    fNameVec.reserve(numGF);
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfPolyMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfSineMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfHyperMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfFlowerMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfPolygonRegMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfPolyIRMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfEllipMS.txt" );
    fNameVec.push_back( "include/level/lvl_sparkAni_data/gfEllip2MS.txt" );

    sf::Text label("HIT", *button::pFont, 10);
    label.setColor( sf::Color::Black );
    sf::Vector2f hbPos( 350.0f, 10.0f );// gfSurf.homeButt position
    sf::Vector2f Pos( 470.0f, 10.0f );// gfSurf position
    float buttW = 50.0f, buttH = 25.0f, spc = 5.0f;
    float dyButt = buttH + spc;
    hitList.init( hbPos.x, hbPos.y, buttW, buttH, label );// hitButt.setSel(true);
    hitList.pHitFunc = [this](){ if( !hitList.sel ) gfEmitButt.setSel( false ); };
    hitList.persist = 3;
    hitList.ownButts = true;
    button::RegisteredButtVec.push_back( &hitList );

    label.setString("Cart");
    buttonList *pBLcart = new buttonList( hbPos.x, hbPos.y+dyButt, buttW, buttH, label );
    hitList.pButtVec.push_back( pBLcart );
    pBLcart->persist = 3;
    pBLcart->ownButts = false;
    label.setString("Polar");
    buttonList *pBLpolar = new buttonList( hbPos.x, hbPos.y+2.0f*dyButt, buttW, buttH, label );
    pBLpolar->persist = 3;
    pBLpolar->ownButts = false;
    hitList.pButtVec.push_back( pBLpolar );
    hbPos.x += buttW + spc;
    sf::Vector2f posCart = hbPos, posPolar = hbPos;

    sf::Vector2f ofst( 10.0f, 10.0f );// gfSurf position
    label.setString("direction");
    for( size_t i = 0; i < numGF; ++i )
    {
     //   hbPos.y += dyButt;
        graphFuncs* pGf = pGfVec[i];
        controlSurface& CS = CsurfVec[i];
        slideBar& SB = SbVec[i];
        multiSelector& MS = MsVec[i];
        buttonValOnHit& Voh = VohVec[i];
        auto &strLbl = strLabelVec[i];
        auto& strFname = fNameVec[i];
    //    CS.init( hbPos, Pos, sf::Vector2f(236.0f,150.0f), strLbl.c_str() );
        sf::Vector2f Pos2;
        if( i+1 < numGF )
        {
         //   hitList.pButtVec.push_back( &CS );
            if( pGf->coordType == 'C' ) { pBLcart->pButtVec.push_back( &CS ); hbPos = posCart; posCart.y += dyButt; }
            else { pBLpolar->pButtVec.push_back( &CS ); hbPos = posPolar; posPolar.y += dyButt; }

            CS.init( hbPos, Pos, sf::Vector2f(236.0f,150.0f), strLbl.c_str() );
            SB.init( Pos.x+ofst.x, 110.0f, 160.0f, 20.0f, label, -3.1416f, 3.1416f, atan2f( pGf->Tu.y, pGf->Tu.x), [this,pGf](float x){ pGf->setRotation(x); }  );
        }
        else // last onto previous surface
        {
        //    CS.pButtVec.push_back( &CsurfVec[i-1] );
            CsurfVec[i-1].pButtVec.push_back( &CS );
            CsurfVec[i-1].onSurfaceOnly = false;
            sf::Vector2f xu(1.0f,0.0f), yu(0.0f,1.0f);
            Pos2 = Pos + ( CsurfVec[i-1].bkRect.getLocalBounds().width + 5.0f )*xu;
            CS.init( Pos2-65.0f*xu+10.0f*yu, Pos2, sf::Vector2f(236.0f,150.0f), strLbl.c_str() );
            SB.init( Pos2.x+ofst.x, Pos2.y+100.0f, 160.0f, 20.0f, label, -3.1416f, 3.1416f, atan2f( pGf->Tu.y, pGf->Tu.x), [this,pGf](float x){ pGf->setRotation(x); }  );
        }
        CS.homeButt.pHitFunc = [this](){ gfEmitButt.setSel( false ); };
        CS.bkRect.setFillColor( sf::Color(64,0,50) );
        CS.pButtVec.push_back( &MS );
        CS.pButtVec.push_back( &SB );
     //   if( i+1 < numGF )
     //   { hitList.pButtVec.push_back( &CS ); }
     //   else { CS.pButtVec.push_back( &CsurfVec[i-1] ); }// last onto previous surface
      //  SB.init( Pos.x+ofst.x, 110.0f, 160.0f, 20.0f, label, -3.1416f, 3.1416f, atan2f( pGf->Tu.y, pGf->Tu.x), [this,pGf](float x){ pGf->setRotation(x); }  );

        fin.open( strFname.c_str() );
        if( !fin ) { std::cout << "\n no data " << i; return false; }
        MS.init( ( i+1 < numGF ? Pos : Pos2 ) + ofst, &Voh, fin );
        fin.close();
    }

    // poly graph
    size_t MsId = 0;
    stripData* pSD = &MsVec[MsId].sdataVec.front();
    if( pSD && !MsVec[MsId].sdataVec.empty() )
    {
        VohVec[MsId].reInit( *pSD );// length
        pSD->pSetFunc = [this,pSD](float x)
        {
            std::cout << "\n&&";
         //   gfPoly.x0 = x/2.0f;
            gfPoly.setLength(x);
            (pSD+1)->xMax = gfPoly.length;// (pSD+1)->xCurr = gfPoly.x0;
            (pSD+1)->xInit = gfPoly.length/2.0f;
            gfPoly.setGraph();
        };
        // x0 in range 0 to length
        (++pSD)->pSetFunc = [this](float x){ if( x > gfPoly.xRt ) x = gfPoly.xRt; gfPoly.x0 = x; gfPoly.setGraph(); };
        pSD->xMax = gfPoly.length;
     //   for( size_t i = 1; i < poly1.cVec.size() && i < MsVec[0].sdataVec.size() + 1; ++i )// each Ci in cVec
         //   MsVec[0].sdataVec[i].pSetFunc = [this,i](float x){ poly1.cVec[i] = x; gfPoly.setGraph(); };

        for( size_t i = 1; i < poly1.cVec.size(); ++i )// each Ci in cVec
            (++pSD)->pSetFunc = [this,i](float x){ poly1.cVec[i] = x; gfPoly.setGraph(); };

        MsVec[0].sdataVec.back().pSetFunc = [this](float x){ if( x != 0.0f ){ gfPoly.tElap *= angFreq_p/x; gfPoly.tPeriod = myPIx2/x; } else gfPoly.tPeriod = -1.0f; angFreq_p = x; gfPoly.setGraph(); };
        MsVec[0].sdataVec.back().pSetFunc( MsVec[0].sdataVec.back().xCurr );
    }
    else { std::cout << "\ngfPoly control : sdataVec.size() < 3"; return false; }

    // sine graph
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 4 )
    {
        pSD->pSetFunc = [this](float x){ Amp_s = x; gfSine.setGraph(); };
        VohVec[MsId].reInit( *pSD );
     //   MsVec[MsId].selButtVec.front().setSel(true);
        ++pSD;// wv_len
        pSD->pSetFunc = [this](float x) { gfSine.tElap *= x/wvLen_s; wvLen_s = x; if( gfSine.dxdt != 0.0f ) gfSine.tPeriod = x/gfSine.dxdt; gfSine.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// x0
        pSD->pSetFunc = [this](float x) { gfSine.x0 = x; gfSine.setGraph(); };
        ++pSD;// length
        pSD->pSetFunc = [this,pSD](float x) { gfSine.setLength(x); (pSD-1)->xInit = gfSine.length/2.0f; gfSine.setGraph(); };
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x)
        {
            if( x != 0.0f )gfSine.tElap *= gfSine.dxdt/x;
            gfSine.dxdt = x; gfSine.tPeriod = (x==0.0f ? -1.0f : wvLen_s/x);
            if( gfEmit.emitMode == 'C' && x*gfEmit.dxEmit < 0.0f ) gfEmit.dxEmit *= -1.0f;
            gfSine.setPosition( gfSine.pos );
        };
        pSD->pSetFunc( pSD->xCurr );
    }
    else { std::cout << "\ngfSine control : sdataVec.size() < 3"; return false; }

    // hyperbola graph
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 4 )
    {
        pSD->pSetFunc = [this](float x){ hypA = x*x; topBranch = (x > 0.0f); gfHyper.setGraph(); };// hypA
        pSD->pSetFunc( pSD->xCurr );
        VohVec[MsId].reInit( *pSD );
        (++pSD)->pSetFunc = [this](float x){ hypB = x*x; gfHyper.setGraph(); };// hypB
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;
        pSD->pSetFunc = [this,pSD](float x)
        {
            gfHyper.setLength(x);
            (pSD+1)->xMax = gfHyper.length;// x0 max
            (pSD+1)->xInit = gfHyper.length/2.0f;// x0 = middle
            if( gfHyper.x0 != (pSD+1)->xCurr ) (pSD+1)->xCurr = gfHyper.x0;
            gfHyper.setGraph();
        };
        // x0 in range 0 to length
        (++pSD)->pSetFunc = [this](float x){ if( x > gfHyper.xRt ) x = gfHyper.xRt; gfHyper.x0 = x; gfHyper.setGraph(); };
        pSD->xMax = gfHyper.length;
    }
    else { std::cout << "\ngfHyper control : sdataVec.size() < 2"; return false; }

    // Flower
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 3 )
    {
        // full radius
        pSD->pSetFunc = [this](float x){ R_Flwr = x; gfFlower.setGraph(); };
        VohVec[MsId].reInit( *pSD );
     //   MsVec[2].selButtVec.front().setSel(true);
        ++pSD;// inner radius to avoid trouble near r = 0
        pSD->pSetFunc = [this](float x) { Rin_Flwr = x; gfFlower.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dxdt = rotation rate
        pSD->pSetFunc = [this](float x) { gfFlower.set_dxdt(x); };
    //    {
     //       gfFlower.dxdt = x;
     //       gfFlower.tElap = 0.0f;
     //       gfFlower.setGraph();
     //   };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// numPetals
        pSD->pSetFunc = [this](float x) { numPetals = (int)floor(x); gfFlower.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
    }
    else { std::cout << "\ngfFlower : sdataVec.size() < 4"; return false; }

    // polygonReg
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 3 )
    {
        // number of sides
        pSD->pSetFunc = [this](float x)
        {
            pgReg.setNsides( (int)floor(x) );
            gfPolygon.vtxVec.clear();
            gfPolygon.vtxVec.resize( 1 + pgReg.numPts() );
            for( auto& vtx : gfPolygon.vtxVec ) vtx.color = gfPolygon.gfColor;
            gfPolygon.setGraph();
        };
        VohVec[MsId].reInit( *pSD );
     //   MsVec[3].selButtVec.front().setSel(true);
        ++pSD;// radius
        pSD->pSetFunc = [this](float x){ pgReg.R = x; gfPolygon.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x) { gfPolygon.set_dxdt(x); };
        pSD->pSetFunc( pSD->xCurr );
    }
    else { std::cout << "\ngfPolygonRegMS: sdataVec.size() < 3"; return false; }

    // polyIR
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 2 )
    {
        // set scale: not ready
        pSD->pSetFunc = [this](float x) { pgIrreg.scale = x; gfPolyIR.setGraph(); };
        VohVec[MsId].reInit( *pSD );
     //   MsVec[4].selButtVec.front().setSel(true);
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x) { gfPolyIR.set_dxdt(x); };
        std::cerr << "\ngot to polyIR";
    }
    else { std::cout << "\ngfPolygonRegMS: sdataVec.size() < 3"; return false; }

    // ellipse
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 3 )
    {
        pSD->pSetFunc = [this](float x){ elip_a = x; gfEllip.setGraph(); };
        VohVec[MsId].reInit( *pSD );
     //   MsVec[5].selButtVec.front().setSel(true);
        ++pSD;// elip_b
        pSD->pSetFunc = [this](float x) { elip_b = x; gfEllip.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x) { gfEllip.set_dxdt(x); };
        pSD->pSetFunc( pSD->xCurr );
    }
    else { std::cout << "\ngfEllip : sdataVec.size() < 3"; return false; }

    // elip2
    pSD = &MsVec[++MsId].sdataVec.front();
    if( MsVec[MsId].sdataVec.size() >= 5 )
    {
        pSD->pSetFunc = [this](float x){ elip_a2 = x; gfEllip2.setGraph(); };
        VohVec[MsId].reInit( *pSD );
    //    MsVec[6].selButtVec.front().setSel(true);
        ++pSD;// elip_b
        pSD->pSetFunc = [this](float x) { elip_b2 = x; gfEllip2.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x) { gfEllip2.set_dxdt(x); };
        pSD->pSetFunc( pSD->xCurr );

        ++pSD;// Amp_e2
        pSD->pSetFunc = [this](float x) { Amp_e2 = x; gfEllip2.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// angFreq_e2 - pulse time
        pSD->pSetFunc = [this](float x)
        {
            angFreq_e2 = x;
            pulseTime_e2 = 0.0f;
            gfEllip2.setGraph();
        };
        pSD->pSetFunc( pSD->xCurr );

        gfEllip2.showAxes = false;
    }
    else { std::cout << "\ngfEllip2: sdataVec.size() < 5"; return false; }

 //   std::cerr << "\nGot to gfPolygon msinit!";




    return true;
}
