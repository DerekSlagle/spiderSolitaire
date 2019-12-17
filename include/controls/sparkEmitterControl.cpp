#include "sparkEmitterControl.h"

void fireFunc::init( char Mode, bool iterUp, float Vel, float t_fire, float t_life, float x_low, float x_high, int N_steps, int Delay, std::function<spark*(void)> GetRawSpark )
{
    getRawSpark = GetRawSpark;
    mode = Mode; vMag = Vel; tFire = t_fire; tLife = t_life;
    xLow = x_low; xHigh = x_high; Nsteps = N_steps; delay = Delay;
    dx = ( xHigh - xLow )/Nsteps;
    if( !iterUp ) dx *= -1.0f;

    if( iterUp ) x = xLow;
    else x = xHigh;
    delayCnt = 0;
    stepCnt = 0;
    dtLife = dvMag = 0.0f;
}

void fireFunc::init( std::istream& is, std::function<spark*(void)> GetRawSpark )
{
    getRawSpark = GetRawSpark;
    reInit(is);
}

void fireFunc::reInit( std::istream& is )
{
    bool iterUp;
    is >> mode >> iterUp >> vMag >> tFire >> tLife;
    is >> xLow >> xHigh >> Nsteps >> delay >> dtLife >> dvMag;

  //  std::cout << "\nmode= " << mode << " iterUp= " << iterUp << " vMag= " << vMag << " tFire= " << tFire;
  //  std::cout << "\ntLife= " << tLife << " xLow= " << xLow << " xHigh= " << xHigh << " Nsteps= " << Nsteps << " delay= " << delay;
    dx = iterUp ? 1.0f : -1.0f;
    reInitHelp();
}

void fireFunc::reInitHelp()
{
    if( dx > 0.0f )
    {
        dx = ( xHigh - xLow )/Nsteps;
        x = xLow;
    }
    else
    {
        dx = ( xLow - xHigh )/Nsteps;
        x = xHigh;
    }

    delayCnt = 0;
    stepCnt = 0;
}

spark* fireFunc::getSpark( float& per )
{
    if( ++delayCnt > delay )// was >=
    {
        delayCnt = 1;// was 0
        if( ++stepCnt > Nsteps )
        {
            stepCnt = 0;
            if( mode == '1' ) { std::cout << "\nmode1"; x = dx > 0.0f ? xHigh : xLow; return nullptr; }
            if( nextFF )
            {
                *pGetSpark_emit = [this](float& t){ return nextFF->getSpark(t); };
                nextFF->reset();
                return nextFF->getSpark(per);

            }
            else if( mode == 'C' )// repeat (cycle) motion
            {
                if( dx > 0.0f ) x = xLow;
                else x = xHigh;
                stepCnt = 1;// new ?
            }
            else if( mode == 'O' )
            {
                if( dx > 0.0f ) x = xHigh;
                else x = xLow;
                dx *= -1.0f;
                stepCnt = 1;// new ?
            }
        }
    }

    if( mode == 'R' )// random direction
    {
        static std::default_random_engine rand_gen;
        static std::uniform_real_distribution<float> x_dist(0.0f,1.0f);
        if( delayCnt == 1 ) x = xLow + ( xHigh - xLow )*x_dist(rand_gen);// allow in same direction
    }
    else if( dx > 0.0f ) x = xLow + dx*stepCnt;
    else x = xHigh + dx*stepCnt;

    float stepsHalf = (delay*Nsteps)/2.0f;
    per = tFire;// + abs( stepsHalf - stepsNow )*dtFire; vary Per some other time

    float xr = (x-xLow)*3.14159f/(xHigh-xLow);
    float tLifeNow = tLife + abs( stepsHalf*cosf(xr) )*dtLife;
    float vNow = vMag + abs( stepsHalf*cosf(xr) )*dvMag;

    spark* pSpk = getRawSpark();// get a spark
    if( pSpk ) pSpk->init( 1.0f, sf::Vector2f( vNow*cosf(x), vNow*sinf(x) ), sf::Vector2f(300.0f,300.0f), pSpk->frIdx, tLifeNow, pSpk->quadW );

    return pSpk;
}

// emitter control
bool sparkEmitterControl::init( sf::Vector2f pos, const char* fName, emitter& rEmit, std::function<spark*(void)> p_GetSpark, std::function<void(float)> p_SetSparkSz, const char* fNameFirePatt )
{
    pGetSpark = p_GetSpark;
    pSetSparkSz = p_SetSparkSz;
    pEmitter = &rEmit;

    std::ifstream fin( fName );
    if( !fin ) { std::cout << "no SE control data\n"; return false; }

    sf::Vector2f Sz; fin >> Sz.x >> Sz.y;
    sf::Vector2f HBofst(0.0f,-30.0f);
    Csurf.init( HBofst+pos, pos, Sz, "sparks" );

    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    float buttHgt = 25.0f;
    float dy = 32.0f;

    sf::Text label("sparks", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    pos.x += 10.0f; pos.y += 10.0f;// offset to 1st control
    label.setString("on/off");
    doEmitButt.init( pos.x, pos.y, 50.0f, buttHgt, [this](){ pEmitter->doEmit = doEmitButt.sel; }, label );
    pos.y += buttHgt + 20.0f;
    label.setString("spark size");
    float minVal, maxVal, iVal; fin >> minVal >> maxVal >> iVal;
    sparkSzStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, pSetSparkSz );

    label.setString("fire direction");
    pos.y += stripHgt + dy;
    std::function<void(float)> p_SetFunc = [this]( float x )
    {
        fireDir.x = cosf(x); fireDir.y = sinf(x);
    };
    float iAngle; fin >> iAngle;
    fireDirSlider.init( pos.x, pos.y, stripLen, stripHgt, label, -4.0f, 4.0f, iAngle, p_SetFunc );
    p_SetFunc( iAngle );

    label.setString("spark speed"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    speedSlider.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, [this](float x){ if( pFireFunc0 ) pFireFunc0->vMag = x; } );

    label.setString("life time"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    tLifeStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, [this](float x){ if( pFireFunc0 ) pFireFunc0->tLife = x; } );

    label.setString("fire delay"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    firePeriodStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, [this](float x){ if( pFireFunc0 ) pFireFunc0->tFire = x; } );

    pos.y += stripHgt + dy;// to fireFuncButt
    std::function<spark*(float&)> gSpkFuncA = [this](float& t){ if( pFireFunc0 ) return pFireFunc0->getSpark(t); return (spark*)nullptr; };

    std::function<spark*(float&)> gSpkFuncB = [this](float& t){ return getReadySpark(t); };

    std::function<void(void)> pF = [this,gSpkFuncA,gSpkFuncB,fNameFirePatt]()
    {
        pEmitter->getSpark = fireFuncButt.sel ? gSpkFuncA : gSpkFuncB;
        if( fireFuncButt.sel && pFireFunc0 )
        {
            ffMS.sel = true;
            pEmitter->getSpark = gSpkFuncA;
            std::ifstream Fin( fNameFirePatt );
            size_t Nfp=1; Fin >> Nfp;
            for( auto& ff : fireFuncVec )
                ff.init( Fin, pGetSpark );

            tLifeStrip.set_fVal( pFireFunc0->tLife );
            firePeriodStrip.set_fVal( pFireFunc0->tFire );
            speedSlider.set_fVal( pFireFunc0->vMag );
            if( ffMS.sdataVec.size() >= 4 )
            {
                ffMS.sdataVec[0].xCurr = pFireFunc0->xLow;
                ffMS.sdataVec[1].xCurr = pFireFunc0->xHigh;
                ffMS.sdataVec[2].xCurr = pFireFunc0->dtLife; std::cout << "\ndtLife= " << pFireFunc0->dtLife;
                ffMS.sdataVec[3].xCurr = pFireFunc0->dvMag;
                if( ffMS.idx_curr > -1 ) ffMS.pFS->set_fVal( ffMS.sdataVec[ffMS.idx_curr].xCurr );
            }
        }
        else { pEmitter->getSpark = gSpkFuncB; ffMS.sel = false; }
    };

    label.setString("FireFunc");
    fireFuncButt.init( pos.x, pos.y, 50.0f, buttHgt, pF, label );
    pos.y += 30.0f;
    ffMS.init(pos, &ffStrip, fin);
    ffMS.sel = false;
    speedSlider.tuneFactor = 0.01f;

    Csurf.pButtVec.reserve(8);
    Csurf.pButtVec.push_back( &doEmitButt );
    Csurf.pButtVec.push_back( &sparkSzStrip );
    Csurf.pButtVec.push_back( &fireDirSlider );
    Csurf.pButtVec.push_back( &speedSlider );
    Csurf.pButtVec.push_back( &tLifeStrip );
    Csurf.pButtVec.push_back( &firePeriodStrip );
    Csurf.pButtVec.push_back( &fireFuncButt );
    Csurf.pButtVec.push_back( &ffMS );

    fin.close();

    fin.open( fNameFirePatt );
    if( !fin ) { std::cout << "no fire pattern data\n"; return false; }
    size_t Nfp=1; fin >> Nfp;
    fireFuncVec.reserve(Nfp);
    std::vector<fireFunc>& ffv = fireFuncVec;// for notational convenience
    // all fireFuncs in 1 file are linked
    for( size_t i = 0; i < Nfp; ++i )
    {
        ffv.push_back( fireFunc( fin, pGetSpark ) );
        fireFunc* pFF = &ffv.back();
        pFF->pGetSpark_emit = &pEmitter->getSpark;
    }

    pFireFunc0 = nullptr;
    if( !ffv.empty() )
    {
        pFireFunc0 = &ffv[0];
        for( size_t i = 0; i + 1 < ffv.size(); ++i )
            ffv[i].nextFF = &ffv[i+1];

        if( ffv.size() > 1 )ffv.back().nextFF = &ffv[0];
    }

    if( pFireFunc0 && ffMS.sdataVec.size() >= 4 )
    {
        stripData* pSD = &ffMS.sdataVec[0];
        // xLow
        pSD->pSetFunc = [this,pSD](float x){ pFireFunc0->xLow = x; pFireFunc0->reInitHelp(); };
        ffStrip.reInit( *pSD );
        pSD->pSetFunc( pSD->xCurr );
        ffMS.selButtVec[0].setSel(true);
        ++pSD;// xHigh
        pSD->pSetFunc = [this,pSD](float x){  pFireFunc0->xHigh = x;  pFireFunc0->reInitHelp(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dtFire
        pSD->pSetFunc = [this,pSD](float x){ pFireFunc0->dtLife = x; };
        pFireFunc0->dtLife = pSD->xCurr;
        ++pSD;// dvMag
        pSD->pSetFunc = [this,pSD](float x){ pFireFunc0->dvMag = x; };
        pFireFunc0->dvMag = pSD->xCurr;
    }
    else { std::cout << "\nemitControl.init() : sdataVec.size() < 4"; return false; }

    return true;
}

spark* sparkEmitterControl::getReadySpark( float& fireDelay )const
{
    spark* pSpk = nullptr;
    if( pGetSpark ) pSpk = pGetSpark();
    if( pSpk )// got one
        pSpk->init( 1.0f, speedSlider.fVal*fireDir, sf::Vector2f(300.0f,300.0f), pSpk->frIdx, tLifeStrip.fVal, sparkSzStrip.fVal );

    fireDelay = firePeriodStrip.fVal;
    return pSpk;
}
