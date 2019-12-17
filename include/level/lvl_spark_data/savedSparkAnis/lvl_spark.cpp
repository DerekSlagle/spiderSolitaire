#include "lvl_spark.h"

// spark mfs
void spark::init( float mass, sf::Vector2f vel, sf::Vector2f pos, sf::Color clr, float t_Life )
{
    m = mass; v = vel; tLife = t_Life;
    setPosition( pos );
    vtx[0].color = vtx[1].color = vtx[2].color = clr;
}

void spark::reset( float t_Life, sf::Vector2f vel, sf::Vector2f pos )
{
    tLife = t_Life;
    v = vel;
    setPosition( pos );
    vtx[0].color.a = vtx[1].color.a = vtx[2].color.a = 255;
}

void spark::setPosition( sf::Vector2f pos )
{
    vtx[0].position = pos + sf::Vector2f( 0.0f, -triW/sqrt(3.0f) );
    vtx[1].position = pos + sf::Vector2f( triW/2.0f, triW/4.0f );
    vtx[2].position = pos + sf::Vector2f( -triW/2.0f, -triW/4.0f );
}

sf::Vector2f spark::getPosition() const
{
    return vtx[0].position + sf::Vector2f( 0.0f, triW/sqrt(3.0f) );
}

void spark::update( float dt )// free
{
    if( tLife > 0.0f )
    {
        setPosition( getPosition() + v*dt );
        tLife -= dt;
        // no fade yet
    }
}

bool spark::hitLine_2side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f L( pt2 - pt1 ), sep( getPosition() - v*dt - pt1 );
    float magL = sqrtf( L.x*L.x + L.y*L.y );
    sf::Vector2f Lu( L/magL );
    float magV = sqrtf( v.x*v.x + v.y*v.y );
    sf::Vector2f Vu( v/magV );
    float vxL = cross( Vu, Lu );
    if( fabs( vxL ) < 0.01f ) return false;// moving parallel to line
    float a = cross( Lu, sep )/(vxL*dt);
    if( a < 0.0f || a > magV )  return false;// passed already or not there yet
    float d = dotProduct( (sep+a*Vu*dt), Lu );
    if( d < 0.0f || d > magL )  return false;// missed already
    // Hit! handle it
    sf::Vector2f N( Lu.y, -Lu.x );
    cr += 1.0f;
    v -= cr*vxL*magV*N;// new velocity
    setPosition( getPosition() + dt*cr*( v - dotProduct( v, Lu )*Lu ) );// new position

 //   if( dotProduct(v,N) < 0.0f ) setPosition( pt1 + d*Lu + triW*N/2.0f );// new position
 //   else setPosition( pt1 + d*Lu - triW*N/2.0f );// new position

    return true;
}

/*
// xcode
bool spark::hitLine_2side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f pos = getPosition();// pt1 is left one
     sf::Vector2f sep = pos - pt1;
     sf::Vector2f L = pt2 - pt1;
     float magL = length(L);
     sf::Vector2f Lu( L/magL );
     sf::Vector2f N( Lu.y, -Lu.x );// N is LH norm to Lu

    float h = cross( Lu, sep );
 //   if( h < 0.0f ) return false;
    float magV = length(v);
    float d = dotProduct( sep, Lu ) - h*dotProduct( v/magV, Lu );
    if( d < 0.0f || d > magL ) return false;
    float vDotN = dotProduct( v, N );

    if( h > 0.0f && h < magV && vDotN < 0.0f )
    {
        v -= ( 1.0f + cr )*vDotN*N;
        pos = pt1 + d*Lu + triW*N/2.0f;
        setPosition( pos );
        return true;
    }
    else if( h <= 0.0f && h > -magV && vDotN > 0.0f )
    {
        v -= ( 1.0f + cr )*vDotN*N;
        pos = pt1 + d*Lu - triW*N/2.0f;
        setPosition( pos );
        return true;
    }

    return false;
}   */

/*
bool spark::hitLine_1side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )// incident from pt2-pt1 LH normal
{
     sf::Vector2f pos = getPosition();// pt1 is top one
    if( pos.x < pt1.x && pos.y > pt1.y && pos.y < pt2.y )
    {
        float sep = pt1.x - pos.x;
        float magV = sqrtf( dotProduct(v,v) );
        if( sep > magV )// release behind
        {
            v.x = 2.0f;
            v.y = 0.0f;
        }
        else v.x *= -cr;

        pos.x = pt1.x + triW;
        setPosition( pos );
    }
}   */
/*
bool spark::hitLine_1side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )// incident from pt2-pt1 LH normal
{
     sf::Vector2f pos = getPosition();// pt1 is left one
    if( pos.y > pt1.y && pos.x > pt1.x && pos.x < pt2.x )
    {
        float sep = pos.y - pt1.y;// > 0
        float magV = sqrtf( dotProduct(v,v) );
        if( sep > magV )// release behind
        {
            v.y = -2.0f;
            v.x = 1.0f;
        }
        else v.y *= -cr;

        pos.y = pt1.y - triW/2.0f;
        setPosition( pos );
        return true;
    }
    return false;
}   */


bool spark::hitLine_1side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )// incident from pt2-pt1 LH normal
{
     sf::Vector2f pos = getPosition();// pt1 is left one
     sf::Vector2f sep = pos - pt1;
     sf::Vector2f L = pt2 - pt1;
     float magL = length(L);
     sf::Vector2f Lu( L/magL );
     sf::Vector2f N( Lu.y, -Lu.x );// N is LH norm to Lu

    float h = cross( Lu, sep );
    if( h < 0.0f ) return false;
    float magV = length(v);
    float d = dotProduct( sep, Lu ) - h*dotProduct( v/magV, Lu );
    if( d < 0.0f || d > magL ) return false;

//    if( h < 1.5f*magV && dotProduct( v, N ) < 0.0f )
    if( h < magV && dotProduct( v, N ) < 0.0f )
    {
        v -= ( 1.0f + cr )*dotProduct( v, N )*N;
        pos = pt1 + d*Lu + triW*N/2.0f;
        setPosition( pos );
        return true;
    }

    return false;
}

bool spark::hitCircle( sf::Vector2f ctr, float R, float cr, float dt )
{
    sf::Vector2f sep = getPosition() - ctr;
    float sepSq = dotProduct(sep,sep);
    if( sepSq > R*R ) return false;
    float magV = sqrtf( dotProduct(v,v) );
    sf::Vector2f Vu(v/magV), Vn( -Vu.y, Vu.x );
    float cs = dotProduct( sep, Vn )/R;
    float sn = sqrtf( 1.0f - cs*cs );
    sf::Vector2f ru = cs*Vn - sn*Vu;
//    float a = dotProduct( Vu, R*ru - sep )/dt;

    setPosition( ctr + R*ru );
    if( dotProduct( Vu, ru ) < 0.0f ) v += (1.0f+cr)*magV*ru;
    else v *= 0.0f;

    return true;
}

void spark::emitRand( float vMin, float vMax, float tMin, float tMax, sf::Vector2f pos )
{
    static std::default_random_engine rand_gen;
    static std::uniform_real_distribution<float> v_dist(0.0f,1.0f);
    static std::uniform_real_distribution<float> ang_dist( -3.1416f, 3.1416f );;
    static std::uniform_real_distribution<float> t_dist(0.0f,1.0f);
    float spd = vMin + ( vMax - vMin )*v_dist(rand_gen);
    float ang = ang_dist(rand_gen);
    sf::Vector2f vel = sf::Vector2f( spd*cosf(ang), spd*sinf(ang) );
    float t_Life = tMin + (tMax - tMin)*t_dist(rand_gen);
    reset( t_Life, vel, pos );
}

/*
bool spark::hit( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f L( pt2 - pt1 ), sep( getPosition() - v*dt - pt1 );
    float magL = sqrtf( L.x*L.x + L.y*L.y );
    sf::Vector2f Lu( L/magL );
    float magV = sqrtf( v.x*v.x + v.y*v.y );
    sf::Vector2f Vu( v/magV );
    float vxL = cross( Vu, Lu );
    if( fabs( vxL ) < 0.01f ) return false;// moving parallel to line
    float a = cross( Lu, sep )/(vxL*dt);
    if( a < 0.0f || a > magV )  return false;// passed already or not there yet
    float d = dotProduct( (sep+a*Vu*dt), Lu );
    if( d < 0.0f || d > magL )  return false;// missed already

 //   std::cout << d << ' ';
    // response
 //   setPosition( getPosition() + ( a - magV )*Vu*dt );// steaming pile
 //   setPosition( getPosition() - v*dt + ( a - magV )*Vu*dt );// works - sort of
 //   setPosition( getPosition() + (a*Vu - v)*dt );// not working. Believe this is correct.
    sf::Vector2f N( -Lu.y, Lu.x );
    cr += 1.0f;
    v += cr*vxL*magV*N;

 //   setPosition( getPosition() + v*dt*cr - cr*dotProduct( v*dt, Lu )*Lu );// nailed it!!!
    setPosition( getPosition() + dt*cr*( v - dotProduct( v, Lu )*Lu ) );// nailed it. Workd perfectly
//    setPosition( getPosition() + v*dt - 2.0f*dotProduct( v*dt, Lu )*Lu );// best

    return true;
}
*/

// lvl_spark mfs
bool lvl_spark::init( )
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
    std::function<void(float)> p_SetFunc_sz = [this]( float x ) { for( spark& spk : sparkVec ) spk.triW = x; };
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
    emitterList.pButtVec.push_back( new buttonRect( posX, posY + 8.0f*(buttH+dy), buttW, buttH, [this](){ for( auto& spk : sparkVec ) spk.tLife = -1.0f; }, label ) );
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
    fin >> diff_ds;
 //   fin >> applySpring;

    label.setString("cross");
    crossList.init( 600.0f, 10.0f, 50.0f, 20.0f, label );
    label.setString("k value");
    float iVal; fin >> minVal >> maxVal >> iVal;
    crossStrip.init( 670.0f, 25.0f, 120.0f, 20.0f, label, minVal, maxVal, iVal, nullptr );
    crossList.pButtVec.push_back( &crossStrip );
    crossList.persist = 3;
    crossList.ownButts = false;
    button::RegisteredButtVec.push_back( &crossList );

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
        switch( type )
        {
            case 'S':
            for( size_t i = 0; i < numForces; ++i )
                pCFvec.push_back( new springForce( fin1 ) );
            break;
            case 'I':
            for( size_t i = 0; i < numForces; ++i )
                pCFvec.push_back( new inverseSquare( fin1 ) );
            break;
        }
        fin1.close();
    };

    // forces list
    posX = 500.0f; posY = 10.0f;
    buttW = 55.0f, buttH = 22.0f; dy = 2.0f;
    label.setString("Forces");
    forceList.init( posX, posY, buttW, buttH, label );

    std::function<void(void)> pFunc_clr = [this]() { for( auto& cf : pCFvec ) { button::unRegisterMe( &cf->fs ); delete cf; } pCFvec.clear(); };
    std::function<void(void)> pFunc_add = [this]() { for( auto& cf : pCFvec ) button::registerUnique( &cf->fs ); };

    label.setString( "spring" );
    std::function<void(void)> pF = [pFunc_clr, pFunc_add, pFunc_LdFce, this](){ pFunc_clr(); pFunc_LdFce("include/level/lvl_spark_data/spring_data.txt", 'S');  pFunc_add(); };
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, pF, label ) );
    label.setString( "inv sqr" );
    pF = [pFunc_clr, pFunc_add, pFunc_LdFce, this](){ pFunc_clr(); pFunc_LdFce("include/level/lvl_spark_data/invSq_data.txt", 'I');  pFunc_add(); };
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, pF, label ) );
    label.setString( "OFF" );
    forceList.pButtVec.push_back( new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [pFunc_clr, this]() { pFunc_clr(); }, label ) );
    forceList.persist = 3;
    forceList.ownButts = true;
    button::RegisteredButtVec.push_back( &forceList );

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
    if( !button::pButtMse && button::clickEvent_Lt() == 1 ) fireSpot.setPosition( sf::Vector2f(button::mseX, button::mseY) );

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
                sparkIt->setColor( clr );
                sparkIt->reset( tLife, sparkVel, fireSpot.getPosition() );
                clr = (clr == sf::Color::Red) ? sf::Color::Green : sf::Color::Red;
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
        }

        if( crossList.sel )
        {
            // a = k*( v.y, -v.x )
            acc.x += crossStrip.fVal*spk.v.y;
            acc.y -= crossStrip.fVal*spk.v.x;
        }

        spk.update( acc, dt );
    }

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

void lvl_spark::cleanup()
{
    for( auto pCF : pCFvec ) delete pCF;
    return;
}

/*
// springForce struct
sf::Vector2f springForce::F( sf::Vector2f pos )
{
    sf::Vector2f r = ctrPos - pos;
    float rMag = length(r);
    return k*r*( rMag - r0 )/rMag;
}

float springForce::U( sf::Vector2f pos )
{
    sf::Vector2f r = ctrPos - pos;
    float disp = length(r) - r0;
    return 0.5f*k*disp*disp;
}   */

//utility
// emit on demand
void lineSource( sf::Vector2f a, sf::Vector2f b, float speedDown, float speedSide, sf::Vector2f& pos, sf::Vector2f& vel )
{
    sf::Vector2f L( b.x-a.x, b.y-a.y ), N( a.y-b.y, b.x-a.x );
    float magN = sqrtf( N.x*N.x + N.y*N.y );

    static std::default_random_engine rand_gen;
    static std::uniform_real_distribution<float> len_dist(0.0f,1.0f);

    float s = len_dist(rand_gen);// 0 to magL

    pos.x = a.x + s*( b.x - a.x );
    pos.y = a.y + s*( b.y - a.y );
    vel = speedDown*N/magN;
    vel += speedSide*L/magN;
    return;
}
// returns true when flip occurs
bool flipX( sf::Vector2f& v, float dt, float period )// assign v.x *= -1.0f; each period. update function
{
    static float tElap = 0.0f;
    tElap += dt;
    if( tElap >= period )
    {
        tElap = 0.0f;
        v.x *= -1.0f;
        return true;
    }
    return false;
}

void spin( sf::Vector2f& v, float dt, float period )// assign v.x *= -1.0f; each period. update function
{
    float dAng = 6.283185f*dt/period;
    float vx = v.x*cosf(dAng) - v.y*sinf(dAng);
    v.y = v.x*sinf(dAng) + v.y*cosf(dAng);
    v.x = vx;
}


