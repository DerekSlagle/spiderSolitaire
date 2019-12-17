#include "mvHitPlay.h"

const float myPI = 3.1415927f;
const float myPIx2 = 2.0f*myPI;

bool mvHitPlay::init()
{
    mvHit::wdwW = Level::winW;
    mvHit::wdwH = Level::winH;
    mvHit::wdwCf = 0.0f;// friction coeff. Read from file below
    mvHit::windowTopOpen = false;

    Level::goto_MMButt.setPosition( {Level::winW-160.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW-80.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
 //   Level::clearColor = sf::Color(50,50,50);

// load some mvHits
    std::ifstream fin( "include/level/lvl_mvHitPlay_data/mvHit_data.txt" );
    if( !fin ) { std::cout << "no mvHit data\n"; return false; }
    if( !load_mvHits( fin, pMvHitVec ) ) return false;

    bool fricOn; float CF; fin >> fricOn >> CF;
    float Ifactor; fin >> Ifactor;
    if( fricOn )
        for( auto& pmh : pMvHitVec )
        {
            pmh->frictionOn = true;
            pmh->Cf = CF;
            pmh->I *= Ifactor;
            std::cout << "\nFriction on! CF = " << CF;
        }

    fin.close();

    fin.open( "include/level/lvl_mvHitPlay_data/spinControl_data.txt" );
    if( !fin ) { std::cout << "no spin control data\n"; return false; }

    sf::Text label("S", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    sf::Vector2f Pos, Sz; fin >> Pos.x >> Pos.y >> Sz.x >> Sz.y;
    sf::Vector2f HBofst(0.0f,-30.0f);
    mhSpinSfc.init( HBofst+Pos, Pos, Sz, "Spin" );
    size_t Nmh = pMvHitVec.size();
    buttVec.reserve( Nmh );
    mhSpinSfc.pButtVec.reserve( Nmh + 2 );// +2 for the strips
    float buttW = 50.0f, buttH = 25.0f, spcX = 10.0f;
    fin >> buttW >> buttH >> spcX; spcX += buttW;
    sf::Vector2f ofst; fin >> ofst.x >> ofst.y;
    Pos += ofst;
 //   label.setString("S");
    idx_curr = 0;
    // manage selected state + assign spin addy
    std::function<void(size_t)> pFi = [this](size_t i)
    {
        buttVec[idx_curr].setSel(false);
        idx_curr = i;
        spinStrip.pVal = nullptr;
        spinStrip.set_fVal( pMvHitVec[i]->angVel );
        spinStrip.pVal = &pMvHitVec[i]->angVel;
    };

    for( size_t i = 0; i < Nmh; ++i )
    {
        buttVec.push_back( buttonRect(Pos.x +i*spcX, Pos.y, buttW, buttH, [pFi,i](){ pFi(i); }, label) );
        mhSpinSfc.pButtVec.push_back( &buttVec.back() );
    }

    label.setString("reInit");
    std::function<void(void)> pFreInit = [this]()
    {
        std::ifstream fin( "include/level/lvl_mvHitPlay_data/mvHit_reInit.txt" );
        if( !fin || pMvHitVec.size() < 2 ) return;
        float angle;
        vec2d Pos, Vel;
        fin >> Pos.x >> Pos.y >> Vel.x >> Vel.y >> angle;// >> pMvHitVec[0]->angVel;
        pMvHitVec[0]->setState(Pos,Vel); pMvHitVec[0]->setRotation(angle);
        fin >> Pos.x >> Pos.y >> Vel.x >> Vel.y >> angle;// >> pMvHitVec[1]->angVel;
        pMvHitVec[1]->setState(Pos,Vel); pMvHitVec[1]->setRotation(angle);
      //  if( idx_curr < 2 ) spinStrip.set_fVal( pMvHitVec[idx_curr]->angVel );
    };
    reInitButt.init(Pos.x +Nmh*spcX, Pos.y, 2.0f*buttW, buttH, pFreInit, label);
    mhSpinSfc.pButtVec.push_back( &reInitButt );

    buttVec[idx_curr].setSel(true);
    float stripLen = 100.0f, stripHgt = 22.0f; fin >> stripLen >> stripHgt >> ofst.x >> ofst.y;
    label.setString("angVel"); Pos += ofst;
    float val_min, val_max, init_val; fin >> val_min >> val_max >> init_val;
    spinStrip.init( Pos.x, Pos.y, stripLen, stripHgt, label, val_min, val_max, init_val, &pMvHitVec[idx_curr]->angVel );
    mhSpinSfc.pButtVec.push_back( &spinStrip );
    fin >> ofst.x >> ofst.y;
    label.setString("Gravity"); Pos += ofst;
    fin >> val_min >> val_max >> gravity.y;
    fin >> mvHit::wdwCf;
    fin.close();
    gravStrip.init( Pos.x, Pos.y, stripLen, stripHgt, label, val_min, val_max, gravity.y, &gravity.y );
    mhSpinSfc.pButtVec.push_back( &gravStrip );
    mhSpinSfc.bkRect.setFillColor( sf::Color(128,0,50) );
    button::RegisteredButtVec.push_back( &mhSpinSfc );

    // graphFuncs
    fin.open( "include/level/lvl_mvHitPlay_data/graphFunc.txt" );
    if( !fin ) { std::cout << "\nno gFunc data\n"; return false; }
    poly1.init(fin);
    float angFreq = 0.01f; fin >> angFreq;
    std::function<float(float)> Fyt = [](float t) { return 1.0f; };// static
    std::function<float(float,float)> pFy = nullptr;//[this,Fyt](float x,float t) { return Fyt(t)*poly1.y(x); };
    std::function<float(float,float)> Fdydt = nullptr;
    std::cout << "\nAF = " << angFreq;
    if( angFreq > 0.001f || angFreq < -0.001f )// abs() not working???
    {
        gfPoly.tPeriod = myPIx2/angFreq;
        Fyt = [this,angFreq](float t) { return sinf( angFreq*t ); };
        pFy = [this,Fyt](float x,float t) { return Fyt(t)*poly1.y(x); };
        Fdydt = [this,angFreq,pFy](float x,float t) { return angFreq*cosf(angFreq*t)*poly1.y(x); };
        std::cout << "\nAF!";
    }

    std::function<float(float,float)> pFyPm = [this,Fyt](float x,float t) { return Fyt(t)*poly1.yPm(x); };
    std::function<void(float&,float&,float,float)> Fy_yPm = [this,Fyt](float& y, float& ypm, float x, float t) { poly1.y_yPm(y,ypm,x); y *= Fyt(t); ypm *= Fyt(t); };

    gfPoly.init( fin, Fy_yPm, Fdydt );
    fin.close();

    Pos.x = 270.0f; Pos.y = 20.0f;// gfSurf position
    gfcCsurf.init( Pos, Pos+sf::Vector2f(0.0f,30.0f), sf::Vector2f(236.0f,150.0f), "Poly" );
    gfcCsurf.pButtVec.push_back( &gfcMs );
    gfcCsurf.pButtVec.push_back( &gfcSb );
    fin.open( "include/level/lvl_mvHitPlay_data/gfPolyMS.txt" );
    if( !fin ) { std::cout << "\nno gfPolyMS data\n"; return false; }
    gfcMs.init( Pos+sf::Vector2f(10.0f,40.0f), &gfcVoh, fin );
    fin.close();
    label.setString("rotate");
    gfcSb.init( Pos.x+10.0f, 150.0f, 160.0f, 20.0f, label, -3.1416f, 3.1416f, atan2f( gfPoly.Tu.y, gfPoly.Tu.x), [this](float x){ gfPoly.setRotation(x); }  );
    button::RegisteredButtVec.push_back( &gfcCsurf );

    stripData* pSD = &gfcMs.sdataVec.front();
    if( pSD && gfcMs.sdataVec.size() >= 5 )
    {
        gfcVoh.reInit( *pSD );// length
        pSD->pSetFunc = [this,pSD](float x)
        {
            gfPoly.setLength(x);
            (pSD+1)->xMax = gfPoly.length;// (pSD+1)->xCurr = gfPoly.x0;
            (pSD+1)->xInit = gfPoly.length/2.0f;
            gfPoly.setGraph();
        };
        // x0 in range 0 to length
        (++pSD)->pSetFunc = [this](float x){ if( x > gfPoly.xRt ) x = gfPoly.xRt; gfPoly.x0 = x; gfPoly.setGraph(); };
        pSD->xMax = gfPoly.length;

        for( size_t i = 1; i < poly1.cVec.size(); ++i )// each Ci in cVec
            (++pSD)->pSetFunc = [this,i](float x){ poly1.cVec[i] = x; gfPoly.setGraph(); };
    }
    else { std::cout << "\ngfPoly control : sdataVec.size() < 3"; return false; }

    return true;
}

bool mvHitPlay::handleEvent( sf::Event& rEvent )
{

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::G ) { gravStrip.set_fVal(0.0f); }
        else if ( rEvent.key.code == sf::Keyboard::A ) { spinStrip.set_fVal(0.0f); }
    }

    return true;
}

void mvHitPlay::update( float dt )
{
    if( !button::pButtMse && !pMvHitVec.empty() )
    {
        static bool held = false;
        static sf::Vector2f msePosGrab;
        static float tElapDrag = 0.0f;
        if( button::clickEvent_Lt() == 1 )
        {
            msePosGrab = button::msePos();
            pMvHitVec[idx_curr]->setPosition( button::msePos().x, button::msePos().y );
            pMvHitVec[idx_curr]->v *= 0.0f;
            held = true;
            tElapDrag = 0.0f;
        }
        else if( held )
        {
            if( button::mseDnLt )// drag
                { pMvHitVec[idx_curr]->setPosition( button::msePos().x, button::msePos().y ); tElapDrag += dt; }
            else
            {
            //    sf::Vector2f posLast = sf::Vector2f( pMvHitVec[idx_curr]->pos.x, pMvHitVec[idx_curr]->pos.y );
            //    pMvHitVec[idx_curr]->setPosition( button::msePos().x, button::msePos().y );
            //    sf::Vector2f Vel = ( button::msePos() - posLast )/dt;
                sf::Vector2f Vel = ( button::msePos() - msePosGrab )/( tElapDrag + dt );
                pMvHitVec[idx_curr]->v.x = Vel.x; pMvHitVec[idx_curr]->v.y = Vel.y;
                held = false;
            }
        }
    }

    if( gfcCsurf.isOpen() ) gfPoly.update(dt);
    for( auto& pMH : pMvHitVec ) pMH->update(dt,gravity);
    hitTestAll( pMvHitVec );

    if( gfcCsurf.isOpen() )
    for( auto& pMH : pMvHitVec )
    {
        sf::Vector2f Pos( pMH->pos.x, pMH->pos.y ), Vel( pMH->v.x, pMH->v.y );
        if( gfPoly.hit( Pos, Vel, 0.8f, dt, pMH->r ) ) pMH->setState( Pos, Vel );
    }


    return;
}

void mvHitPlay::draw( sf::RenderTarget& RT ) const
{
    if( gfcCsurf.sel ) gfPoly.draw(RT);
    for( auto& pMH : pMvHitVec ) pMH->draw(RT);
    return;
}

void mvHitPlay::cleanup()
{
    for( auto& pMH : pMvHitVec ) delete pMH;
    pMvHitVec.clear();
    return;
}
