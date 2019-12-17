#include "lvl_spark.h"

bool lvl_spark::init()
{
    Level::goto_MMButt.setPosition( {Level::winW-160.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW-80.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
 //   Level::clearColor = sf::Color(0,100,200);

    std::ifstream fin("include/level/lvl_spark_data/init_data.txt");
    if( !fin ) { std::cout << "\nNo spark init_data"; return false; }

    size_t N; fin >> N;
    sparkVec.reserve(N);

    // some ui
    sf::Text label("sparks", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    float posX, posY; fin >> posX >> posY;
    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    float buttHgt = 25.0f;
    float dy = 17.0f;
    sparkList.init( posX, posY, 80.0f, buttHgt, label );
    label.setString("spark size");
    posY += buttHgt + dy;
    std::function<void(float)> p_SetFunc_sz = [this]( float x ) { for( spark& spk : sparkVec ) spk.setSize(x); };//spk.quadW = x; };
    float size0; fin >> size0;
    sparkList.pButtVec.push_back( new buttonValOnHit( posX, posY, stripLen, stripHgt, label, 1.0f, 10.0f, size0, p_SetFunc_sz ) );

    label.setString("fire direction");
    posY += stripHgt + dy;
    std::function<void(float)> p_SetFunc = [this]( float x )
    {
        float spd = sqrtf( sparkVel.x*sparkVel.x + sparkVel.y*sparkVel.y );
        sparkVel.x = cosf(x); sparkVel.y = sinf(x);
        sparkVel *= spd;
    };
    float ang0; fin >> ang0;
    float minVal, maxVal; fin >> minVal >> maxVal >> sparkVel.x; sparkVel.y = 0.0f;// before direction is set
 //   sparkList.pButtVec.push_back( new slideBar( posX, posY, stripLen, stripHgt, label, -3.1416f, 3.1416f, ang0, p_SetFunc ) );
    pSB_angle = new slideBar( posX, posY, stripLen, stripHgt, label, -4.0f, 4.0f, ang0, p_SetFunc );
    sparkList.pButtVec.push_back( pSB_angle );
    p_SetFunc(ang0);

    label.setString("spark speed"); posY += stripHgt + dy;
    std::function<void(float)> p_SetFunc_spd = [this]( float x )
    {
        float ang = atan2f( sparkVel.y, sparkVel.x );
        sparkVel.x = x*cosf(ang); sparkVel.y = x*sinf(ang);
        fireSpot.voh.set_fVal(x);
    };
    float magV0 = sqrtf( sparkVel.x*sparkVel.x + sparkVel.y*sparkVel.y );
    slideBar* pSB_spd = new slideBar( posX, posY, stripLen, stripHgt, label, minVal, maxVal, magV0, p_SetFunc_spd );
    sparkList.pButtVec.push_back( pSB_spd );
    label.setString("life time"); posY += stripHgt + dy;
    fin >> minVal >> maxVal >> tLife;
    sparkList.pButtVec.push_back( new buttonValOnHit( posX, posY, stripLen, stripHgt, label, minVal, maxVal, tLife, &tLife ) );

    label.setString("fire delay"); posY += stripHgt + dy;
    fin >> minVal >> maxVal >> tFire;
    sparkList.pButtVec.push_back( new buttonValOnHit( posX, posY, stripLen, stripHgt, label, minVal, maxVal, tFire, &tFire ) );

    label.setString("gravity"); posY += stripHgt + dy;
    fin >> maxVal >> grav;
    pSB_grav = new slideBar( posX, posY, stripLen, stripHgt, label, -maxVal, maxVal, grav, &grav );
    sparkList.pButtVec.push_back( pSB_grav );

    label.setString("flip period"); posY += stripHgt + dy;
    fin >> minVal >> maxVal >> flipPeriod;
    sparkList.pButtVec.push_back( new buttonValOnHit( posX, posY, stripLen, stripHgt, label, minVal, maxVal, flipPeriod, &flipPeriod ) );
    sparkList.persist = 3;
    sparkList.ownButts = true;
 //   sparkList.setSel(true);
    button::RegisteredButtVec.push_back( &sparkList );

    posX = 250.0f; posY = 10.0f;
    float buttW = 55.0f, buttH = 22.0f;
    dy = 2.0f;
    label.setString("EMIT");
    emitterList.init( posX, posY, buttW, buttH, label );

    std::function<void(bool*)> pFunc_2 = [this]( bool* pOn ) { if( pCurrOn ) *pCurrOn = false; pCurrOn = pOn; if( pCurrOn ) *pCurrOn = true; };
    label.setString( "stream" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&streamOn); }, label ) );
    label.setString( "flip" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&flipOn); }, label ) );
    label.setString( "spin" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&spinOn); }, label ) );
    label.setString( "sparkle" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 4.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&randOn); }, label ) );
    label.setString( "rain" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 5.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&lineOn); }, label ) );
    label.setString( "one" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 6.0f*(buttH+dy), buttW, buttH, [pFunc_2,this]() { pFunc_2(&oneOn); }, label ) );
    label.setString( "OFF" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 7.0f*(buttH+dy), buttW, buttH, [this](){ if( pCurrOn ) *pCurrOn = false; }, label ) );
    label.setString( "Clear" );
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 8.0f*(buttH+dy), buttW, buttH, [this](){ showArea = mapReady = false; for( auto& spk : sparkVec ) spk.tLife = -1.0f; }, label ) );
    emitterList.persist = 3;
    emitterList.ownButts = true;
    button::RegisteredButtVec.push_back( &emitterList );

    label.setString("HIT");
    hitButt.init( 400.0f, 10.0f, 40.0f, 20.0f, nullptr, label );// hitButt.setSel(true);
    button::RegisteredButtVec.push_back( &hitButt );

    float spotRadius = 4.0f;
    fin >> posX >> posY;
    fireSpot.init( sf::Vector2f(posX,posY), sf::Vector2f(15.0f,-8.0f), spotRadius, "emitter", 14 );
    std::function<void(float)> p_SetFunc_spd2 = [p_SetFunc_spd, pSB_spd](float x){ p_SetFunc_spd(x); if( pSB_spd ) pSB_spd->set_fVal(x); };
    fireSpot.init2( pSB_spd->fVal_min, pSB_spd->fVal_max, magV0, "speed", p_SetFunc_spd2 );
    button::RegisteredButtVec.push_back( &fireSpot );

    fin >> cr >> crBall >> wallCr;
    fin >> diff_ds >> ds_map;
 //   fin >> applySpring;

    label.setString("cross");
    crossList.init( 600.0f, 10.0f, 50.0f, 20.0f, label );
    label.setString("k value");
    float iVal; fin >> minVal >> maxVal >> iVal;
    crossStrip.init( 670.0f, 18.0f, 120.0f, 20.0f, label, minVal, maxVal, iVal, nullptr );
    crossList.pButtVec.push_back( &crossStrip );
    crossList.persist = 3;
    crossList.ownButts = false;
    button::RegisteredButtVec.push_back( &crossList );

    label.setString("DRAG");
    dragList.init( 600.0f, 40.0f, 50.0f, 20.0f, label );
    label.setString("k value");
    fin >> minVal >> maxVal >> iVal;
    dragStrip.init( 670.0f, 63.0f, 120.0f, 20.0f, label, minVal, maxVal, iVal, nullptr );
    dragList.pButtVec.push_back( &dragStrip );
    dragList.persist = 3;
    dragList.ownButts = false;
    button::RegisteredButtVec.push_back( &dragList );

    // end of datum from init_data.txt
    fin.close();

    fin.open("include/level/lvl_spark_data/barrier_data.txt");
    if( !fin ) { std::cout << "\nNo spark barrier_data"; return false; }

    // line source
    fin >> lineSceVtx[0].position.x >> lineSceVtx[0].position.y;
    fin >> lineSceVtx[1].position.x >> lineSceVtx[1].position.y;
    lineSceVtx[0].color = lineSceVtx[1].color = sf::Color::Blue;

    spark spk; spk.init( 1.0f, sf::Vector2f( sparkVel.x, 0.0f ), sf::Vector2f(posX,posY), sf::Color::Red );
    for( size_t i=0; i<N; ++i )
        sparkVec.push_back( spk );

    sparkIt = sparkVec.begin();
    p_SetFunc_sz(size0);// now that they exist

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

    std::function<void(const char*, char)> pFunc_LdFce = [this](const char* fName, char type)
    {
        std::ifstream fin1(fName);
        if( !fin1 ) { std::cout << "\nNo force_data"; return; }

        size_t numForces; fin1 >> numForces;
        pCFvec.reserve( numForces );
        FSvec.reserve( numForces );
        floatSpot FStemp;
        float minx, maxx;
        switch( type )
        {
            case 'S':
            for( size_t i = 0; i < numForces; ++i )
            {
                springForce* pSF = new springForce( fin1 );
                pCFvec.push_back( pSF );
                fin1 >> minx >> maxx;
                FStemp.init( pSF->ctrPos, sf::Vector2f(15.0f,-8.0f), 4.0f, "spring", 14, sf::Color::Yellow );
                FStemp.init2( minx, maxx, pSF->k, "k value", &pSF->k );
                FSvec.push_back( FStemp );
            }
            break;
            case 'I':
            for( size_t i = 0; i < numForces; ++i )
            {
                inverseSquare* pIV = new inverseSquare( fin1 );
                pCFvec.push_back( pIV );
                fin1 >> minx >> maxx;
                FStemp.init( pIV->ctrPos, sf::Vector2f(15.0f,-8.0f), 4.0f, "invSq", 14, sf::Color::Yellow );
                FStemp.init2( minx, maxx, pIV->k, "k value", &pIV->k );
                FSvec.push_back( FStemp );
            }
            break;
        }
        fin1.close();
    };

    // forces list
    posX = 500.0f; posY = 10.0f;
    buttW = 55.0f, buttH = 22.0f; dy = 2.0f;
    label.setString("Forces");
    forceList.init( posX, posY, buttW, buttH, label );

    std::function<void(void)> pFunc_clr = [this]() { for( auto& fs : FSvec ) button::unRegisterMe( &fs ); FSvec.clear(); for( auto& cf : pCFvec ) delete cf; pCFvec.clear(); };
    std::function<void(void)> pFunc_add = [this]() { for( auto& fs : FSvec ) button::registerUnique( &fs ); };

    label.setString( "spring" );
    std::function<void(void)> pF = [pFunc_clr, pFunc_add, pFunc_LdFce, this](){ pFunc_clr(); pFunc_LdFce("include/level/lvl_spark_data/spring_data.txt", 'S');  pFunc_add(); };
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, pF, label ) );
    label.setString( "inv sqr" );
    pF = [pFunc_clr, pFunc_add, pFunc_LdFce, this](){ pFunc_clr(); pFunc_LdFce("include/level/lvl_spark_data/invSq_data.txt", 'I');  pFunc_add(); };
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, pF, label ) );
    label.setString( "OFF" );
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [pFunc_clr, this]() { pFunc_clr(); mapReady = showArea = false; }, label ) );
    forceList.persist = 3;
    forceList.ownButts = true;
    button::RegisteredButtVec.push_back( &forceList );

    // save the current simulation to a sparkAni
    label.setString( "SAVE" );
    pF = [this](){ save_sparkAni_toFile( "include/level/lvl_spark_data/SavedSparkAni.txt" ); };
    saveSAbutt.init( 50.0f, 680.0f, 50.0f, 22.0f, pF, label );
    button::RegisteredButtVec.push_back( &saveSAbutt );


    label.setString( "AREA" );
    aniAreaButt.init( 150.0f, 680.0f, 50.0f, 22.0f, nullptr, label ); aniAreaButt.mode = 'I';
    button::RegisteredButtVec.push_back( &aniAreaButt );
    aniArea[0].color = aniArea[1].color = aniArea[2].color = aniArea[3].color =  aniArea[4].color = sf::Color(200,0,50,70);

    label.setString( "ds_map" );
    ds_mapStrip.init( 400.0f, 680.0f, 200.0f, 20.0f, label, 0.2f, 30.0f, 5.0f, &ds_map );
    button::RegisteredButtVec.push_back( &ds_mapStrip );

    // level start conditions
 //   streamOn = true;
 //   pCurrOn = &streamOn;

    return true;
}

bool lvl_spark::handleEvent( sf::Event& rEvent )
{
    static bool shiftDn = false;

    if (rEvent.type == sf::Event::MouseWheelScrolled)
    {
        float dAng = ( shiftDn ? 0.5f : 0.05f )*(float) rEvent.mouseWheelScroll.delta;
        float vx = sparkVel.x*cosf(dAng) - sparkVel.y*sinf(dAng);
        sparkVel.y = sparkVel.x*sinf(dAng) + sparkVel.y*cosf(dAng);
        sparkVel.x = vx;
        if( pSB_angle ) pSB_angle->set_fVal( atan2f( sparkVel.y, sparkVel.x ) );
    }
    else if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = true;
        else if ( rEvent.key.code == sf::Keyboard::Up ) { if( pSB_grav ) pSB_grav->set_fVal( grav + 0.001f ); }
        else if ( rEvent.key.code == sf::Keyboard::Down ) { if( pSB_grav ) pSB_grav->set_fVal( grav - 0.001f ); }
        else if ( rEvent.key.code == sf::Keyboard::G ) { if( pSB_grav ) pSB_grav->set_fVal( 0.0f ); }
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::RShift ) shiftDn = false;
    }

    return true;
}

void lvl_spark::update( float dt )
{
 //   if( !button::pButtMse && button::clickEvent_Lt() == 1 ) fireSpot.setPosition( button::mseX, button::mseY );
    if( !button::pButtMse && button::clickEvent_Lt() == 1 )
    {
        fireSpot.setPosition( sf::Vector2f(button::mseX, button::mseY) );
        std::cout << "\nemitPos.x = " << button::mseX << " emitPos.y = " << button::mseY;
    }

    if( flipOn ) flipX( sparkVel, dt, flipPeriod );
    else if( spinOn ) spin( sparkVel, dt, 5.0f*flipPeriod );

    static float tElap = 0.0f;
    tElap += dt;
    if( tElap > tFire )
    {
        tElap = 0.0f;
        if( pCurrOn && *pCurrOn )
        {
            ++sparkIt; if( sparkIt == sparkVec.end() ) sparkIt = sparkVec.begin();
            if( lineOn )
            {
                sf::Vector2f pos, vel;
                lineSource( lineSceVtx[0].position, lineSceVtx[1].position, 5.0f, 0.0f, pos, vel );
                sparkIt->reset( tLife, vel, pos );
                sparkIt->setColor( sf::Color(100,100,255) );
            }
            else if( randOn ) sparkIt->emitRand( 3.0f, 12.0f, 5.0f, 40.0f, fireSpot.getPosition() );
            else if( oneOn ) { oneOn = false; sparkIt->reset( tLife, sparkVel, fireSpot.getPosition() ); }
            else// streaming
            {
                static sf::Color clr( sf::Color::Red );
                if( clr == sf::Color::Red ) clr = sf::Color::Green;
                else if( clr == sf::Color::Green ) clr = mapReady ? sf::Color::Yellow : sf::Color::Red;
                else if( clr == sf::Color::Yellow ) clr = sf::Color::Red;

                sparkIt->setColor( clr );
                sparkIt->reset( tLife, sparkVel, fireSpot.getPosition() );
            }
        }
    }

    // apply forces
    sf::Vector2f acc( 0.0f, grav );
    for( spark& spk : sparkVec )
    {
        acc.x = 0.0f; acc.y = grav;
        if( !pCFvec.empty() )
        {
            std::vector<consForce*>& cfVec = pCFvec;//applySpring ? pSpringVec : pInvSqVec;
            sf::Vector2f pos = spk.getPosition();

            if( spk.vtx[0].color == sf::Color::Red )
                for( auto& fv : cfVec ) acc += fv->F( pos );
            else
            {
                if( spk.vtx[0].color == sf::Color::Green || !mapReady )
                {
                    float Uxy = 0.0f, Udx = 0.0f, Udy = 0.0f;
                    sf::Vector2f dx( diff_ds, 0.0f ), dy( 0.0f, diff_ds );

                    // find total potential energy
                    for( auto& fv : cfVec )
                    {
                        Uxy += fv->U( pos );// at (x,y)
                        Udx += fv->U( pos + dx );// at (x+dx,y)
                        Udy += fv->U( pos + dy );// at (x,y+dy)
                    }
                    // approximate -gradU
                    acc.x -= ( Udx - Uxy )/diff_ds;
                    acc.y -= ( Udy - Uxy )/diff_ds;
                }
                else if( spk.vtx[0].color == sf::Color::Yellow && mapReady )
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

        spk.update( acc, dt );
    }

    findAniArea();// update as needed

    if( hitButt.sel )
        for( spark& spk : sparkVec )
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

    fireSpot.draw(RT);
    for( const spark& spk : sparkVec ) spk.draw(RT);

    if( hitButt.sel )
    {
        if( lineVec.size() > 0 ) RT.draw( &lineVec[0], lineVec.size(), sf::Lines );
        for( const auto& crcl : circleVec ) RT.draw( crcl );
        RT.draw( lineSceVtx, 2, sf::Lines );
        RT.draw( line1Vtx, 8, sf::Lines );
    }
    return;
}

void lvl_spark::save_sparkAni_toFile( const char* fname )const
{
    std::ofstream fout( fname );
    if( !fout ) return;

    sparkAni SA;// buildup instance
    SA.spkVec = sparkVec;
    SA.quadW_all = sparkVec.empty() ? 4.0f : sparkVec[0].quadW;
    SA.doCross = crossList.sel; SA.crossK = crossStrip.fVal;
    SA.doDrag = dragList.sel; SA.dragK = dragStrip.fVal;
    SA.gravityOn = (grav != 0.0f); SA.gravity.x = 0.0f; SA.gravity.y = grav;
    SA.emitPeriod = tFire;
    SA.tLife_all = tLife;
    SA.mass_all = sparkVec.empty() ? 1.0f : sparkVec[0].m;
    SA.charge_all = sparkVec.empty() ? 1.0f : sparkVec[0].q;
    SA.fireSpeed = length(sparkVel); SA.fireAngle = atan2f(sparkVel.y, sparkVel.x);
    SA.doFlip = flipOn; SA.flipPeriod = flipPeriod;
    SA.doSpin = spinOn; SA.spinPeriod = flipPeriod;// not distinct in tester

    for( consForce* pCF : pCFvec ) pCF->ctrPos -= fireSpot.pos;
    SA.pCFvec = pCFvec;
    SA.findBounds();
    SA.to_stream( fout );
    SA.pCFvec.clear();// just borrowed them
    for( consForce* pCF : pCFvec ) pCF->ctrPos += fireSpot.pos;
    std::cout << "\nsaved as sparkAni";
}
// area is
//  0 1
//  3 2
void lvl_spark::findAniArea()// assigns to above 2 members
{
 //   if( !aniAreaButt.sel ) return;

    static bool aniAreaButtSel_last = false;

    // find bounds
 //   static float &xMin = aniArea[0].position.x, &xMax = aniArea[1].position.x;
 //   static float &yMin = aniArea[0].position.y, &yMax = aniArea[3].position.y;

    if( aniAreaButt.sel != aniAreaButtSel_last )// begin or end event
    {
        if( aniAreaButt.sel )// begin
        {
            bool iValsFound = false;
            for( const spark& spk : sparkVec )
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
        for( const spark& spk : sparkVec )
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
            for( auto& fv : pCFvec ) forceMap[r*cols_map+c] += fv->F( pos );
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
    for( auto pCF : pCFvec ) delete pCF;
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
