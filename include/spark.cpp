#include "spark.h"

//sf::Texture* spark::pTxt = nullptr;

void spark::init( float mass, sf::Vector2f vel, sf::Vector2f pos, int FrIdx, float t_Life, float QuadW )
{
    m = mass; v = vel; tLife = t_Life;
    frIdx = FrIdx;
    quadW = QuadW;
    setPosition( pos );// quadW must be assigned 1st
    vtx[0].color = vtx[1].color = vtx[2].color = vtx[3].color = sf::Color::White;
}

void spark::reset( float t_Life, sf::Vector2f vel, sf::Vector2f pos )
{
    tLife = t_Life;
    v = vel;
    setPosition( pos );
    vtx[0].color.a = vtx[1].color.a = vtx[2].color.a = vtx[3].color.a = 255;
}

void spark::draw( sf::RenderTarget& RT, const sf::Texture* pTxt ) const
{
    if( tLife > 0.0f )
    {
        if( pTxt ) RT.draw( vtx, 4, sf::Quads, pTxt );
        else RT.draw( vtx, 4, sf::Quads );
    }
}

void spark::setPosition( sf::Vector2f pos )
{
    float ds = quadW/2.0f;
    vtx[0].position = pos + sf::Vector2f( -ds, -ds );// up lt
    vtx[1].position = pos + sf::Vector2f( ds, -ds );// up rt
    vtx[2].position = pos + sf::Vector2f( ds, ds );// dn rt
    vtx[3].position = pos + sf::Vector2f( -ds, ds );// dn lt
}

sf::Vector2f spark::getPosition() const
{
    return ( vtx[0].position + vtx[2].position )/2.0f;
}

void spark::update( float dt )// free
{
    if( tLife > 0.0f )
    {
        setPosition( getPosition() + v*dt );
        if( fadeRate > 1.0f && tLife*fadeRate < 256.0f ) { unsigned int Alpha = floor( abs(tLife*fadeRate)); vtx[0].color.a = vtx[1].color.a = vtx[2].color.a = vtx[3].color.a = Alpha; }
        tLife -= dt;
        // no fade yet
    }
}
/*
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

    return true;
}   */

bool spark::hitLine_2side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f L( pt2 - pt1 );
    sf::Vector2f pos( getPosition() );
    sf::Vector2f Sf( pos - pt1 );
    if( dotProduct(Sf,Sf) >  dotProduct(L,L) ) return false;// too far away
    float magL = length(L);
    sf::Vector2f Lu(L/magL);
    sf::Vector2f Nu(Lu.y,-Lu.x);
    sf::Vector2f S0(Sf-v*dt);
    float hf = dotProduct(Sf,Nu), h0 = dotProduct(S0,Nu);
    float vN = dotProduct(v,Nu), vL = dotProduct(v,Lu);
    if( vN == 0.0f || hf == 0.0f || h0/hf > 0.0f ) return false;// parallel or no pen yet or didn't cross
    float d = dotProduct(Sf,Lu);
    float y = d - hf*vL/vN;
    if( y < 0.0f || y > magL ) return false;// missed ends

    if( hf*vN > 0.0f )
    {
     //   std::cout << (hf<0.0f?" +N":" -N");
        pos = pt1 + y*Lu;
        if( hf < 0.0f ) pos += Nu;
        else pos -= Nu;
        setPosition( pos );
        v -= ( 1.0f + cr )*vN*Nu;
        return true;
    }
    return false;
}

// condition: pt1.x < pt2.x
bool spark::hitLine_2sideA( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f Pf = getPosition() - pt1;
    sf::Vector2f P0 = Pf - v*dt; if( P0.x < 0.0f && Pf.x < 0.0f ) return false;// miss left
    float L = pt2.x - pt1.x; if( P0.x > L && Pf.x > L ) return false;// miss right
    float H = pt2.y - pt1.y, slope = H/L;
    std::function<float(float)> fy = [slope]( float x ){ return slope*x; };
    float a = fy(P0.x) - P0.y;// distance above graph at P0
    if( a < 0.0f && v.y > v.x*slope )  return false; // below and going down
    if( a > 0.0f && v.y < v.x*slope ) return false;// above and going up
    float b = Pf.y - fy(Pf.x);// distance below graph at Pf
    if( a*b < 0.0f ) return false;// didn't cross
    // handle hit
    float xh = (a*Pf.x + b*P0.x)/( a + b ), yh = fy(xh);
    sf::Vector2f N( slope, -1.0f ); N /= sqrtf( 1.0f + slope*slope );// normalize
    if( v.y < v.x*slope ) N *= -1.0f;// face incident direction
    sf::Vector2f Ph = sf::Vector2f(xh,yh) + N;// N = 1 pixel offset from curve to incident side
    setPosition( Ph + pt1 );
    v -= ( 1.0f + cr )*dotProduct(v,N)*N;// reflect to incident side
    return true;
}

/*
// condition: pt1.x < pt2.x
bool spark::hitLine_2sideA( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt )
{
    sf::Vector2f Pf = getPosition(), P0 = Pf - v*dt;
    if( P0.x < pt1.x && Pf.x < pt1.x ) return false;//{ std::cout << " missLt"; return false; }// miss left
    if( P0.x > pt2.x && Pf.x > pt2.x ) return false;//{ std::cout << " missRt"; return false; }// miss right
    float slope = (pt2.y - pt1.y)/(pt2.x - pt1.x );
    std::function<float(float)> fy = [slope,pt1]( float x ){ return pt1.y + slope*( x - pt1.x ); };
    float a = fy(P0.x) - P0.y;
    if( a < 0.0f && v.y > 0.0f ) return false;//{ std::cout << " bgd"; return false; }// below and going down
    if( a > 0.0f && v.y < 0.0f ) return false;//{ std::cout << " agu"; return false; }// above and going up
    float b = Pf.y - fy(Pf.x);
    if( a*b < 0.0f ) return false;//{ std::cout << " noCross"; return false; }// didn't cross

    // handle hit
    float xh = (a*Pf.x + b*P0.x)/( a + b );
    float yh = fy(xh);
    sf::Vector2f N( slope, -1.0f );
    N /= sqrtf( 1.0f + slope*slope );
    if( v.y < 0.0f ) N *= -1.0f;
    sf::Vector2f Ph = sf::Vector2f(xh,yh) + N;
    setPosition( Ph );
    v -= ( 1.0f + cr )*dotProduct(v,N)*N;
    return true;
}
*/

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
        pos = pt1 + d*Lu + quadW*N/2.0f;
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
    float vDotR = dotProduct( v, ru );
    if( vDotR < 0.0f ) v -= (1.0f+cr)*vDotR*ru;
 //   if( dotProduct( Vu, ru ) < 0.0f ) v += (1.0f+cr)*magV*ru;
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

void spark::setTxtCoords( sf::IntRect tr )// left, top, width, height
{
    vtx[0].texCoords.x = tr.left;            vtx[0].texCoords.y = tr.top;// up lt
    vtx[1].texCoords.x = tr.left + tr.width; vtx[1].texCoords.y = tr.top;// up rt
    vtx[2].texCoords.x = tr.left + tr.width; vtx[2].texCoords.y = tr.top + tr.height;// dn rt
    vtx[3].texCoords.x = tr.left;             vtx[3].texCoords.y = tr.top + tr.height;// dn lt
}

// emitter

bool emitter::update( float dt )
{
    if( !doEmit ) return false;
    // fire a spark?
    tElapEmit += dt;
    bool didEmit = false;
    while( tElapEmit > emitPeriod )
    {
        tElapEmit -= emitPeriod;// reset
        spark* pSpk = getSpark( emitPeriod );// until next call
        if( pSpk )// modify spark?
        {
            sf::Vector2f ePos = emitPos + ( tElapEmit-emitPeriod )*pSpk->v;
            if( tElapEmit < emitPeriod ) { tElapEmit = 0.0f; pSpk->setPosition( emitPos ); }
            else pSpk->setPosition( ePos );
            didEmit = true;
        }
        else// no sparks
        {
            tElapEmit = 0.0f;
            return false;
        }

      //  tElapEmit -= emitPeriod;// reset
    }
    return didEmit;
}

/*
bool emitter::update( float dt )
{
    if( !doEmit ) return false;
    // fire a spark?
    tElapEmit += dt;
    if( tElapEmit > emitPeriod )
    {
        tElapEmit = 0.0f;// reset
        spark* pSpk = getSpark( emitPeriod );// until next call
        if( pSpk )// modify spark?
        {
            pSpk->setPosition( emitPos );
            return true;
        }
    }
    return false;
}   */


// sparkAni mfs
bool sparkAni::init( sf::Vector2f pos, spriteSheet* p_SS, const char* fName )
{
    std::ifstream fin( fName );
    if( !fin ) return false;
    pSS = p_SS;
    emitPos = pos;
    init( fin );
    return true;
}

void sparkAni::setPosition( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - emitPos;
    emitPos = pos;
    for( centralForce& rCF : CFvec ) rCF.ctrPos += dPos;
    for( spark& spk : spkVec ) if( spk.tLife > 0.0f ) spk.setPosition( spk.getPosition() + dPos );
}

sparkAni& sparkAni::operator=( const sparkAni& sa )
{
    if( this == &sa ) return *this;

    // the forces
    CFvec = sa.CFvec;
    // everything else
    spkVec = sa.spkVec;
    if( !sa.spkVec.empty() )// copy spkIt shift
    {
        spkIt = spkVec.begin() + ( sa.spkIt - sa.spkVec.begin() );
    }
    else spkIt = spkVec.begin();

    pSS = sa.pSS;
    doCross = sa.doCross; doDrag = sa.doDrag;// force mods. Velocity depend
    crossK = sa.crossK; dragK = sa.dragK;
    gravityOn = sa.gravityOn;
    gravity = sa.gravity;
    // emitter params
    emitPos = sa.emitPos;
    emitPeriod = sa.emitPeriod; tElapEmit = sa.tElapEmit;
    tLife_all = sa.tLife_all; mass_all = sa.mass_all; charge_all = sa.charge_all;
    quadW_all = sa.quadW_all;
 //   clr_all = sa.clr_all;
    fireSpeed = sa.fireSpeed; fireAngle = sa.fireAngle;
    fireDir = sa.fireDir;
    doFlip = sa.doFlip; doSpin = sa.doSpin;
    flipPeriod = sa.flipPeriod; tElapFlip = sa.tElapFlip; spinPeriod = sa.spinPeriod; tElapSpin = sa.tElapSpin;
    frIdxVec = sa.frIdxVec;
    frIdx_curr = sa.frIdx_curr;
    xLt = sa.xLt; xRt = sa.xRt; yTop = sa.yTop; yBot = sa.yBot;
    return *this;
}

void sparkAni::to_stream( std::ostream& os )
{
    os << spkVec.size() << '\n';// loaded sparkAni reqd here
    os << quadW_all << '\n';// loaded sparkAni reqd here
    os << doCross << ' ' << crossK << '\n';
    os << doDrag << ' ' << dragK << '\n';
    os << gravityOn << ' ' << gravity.x << ' ' << gravity.y << '\n';
    os << emitPeriod << '\n';
    os << tLife_all << ' ' << mass_all << ' ' << charge_all << '\n';
    os << fireSpeed << ' ' << fireAngle << '\n';
    os << doFlip << ' ' << flipPeriod << '\n';
    os << doSpin << ' ' << spinPeriod << "\n\n";

    os << frIdxVec.size() << '\n';// numColors + rd gn bu
    for( int i : frIdxVec ) os << i << ' ';
    os << "\n\n";

    // forces
    os << xLt << ' ' << xRt << ' ' << yTop << ' ' << yBot << '\n';
    os << CFvec.size() << '\n';
    for( auto& cf : CFvec )
    {
        cf.ctrPos -= emitPos;
        cf.to_stream(os);
        cf.ctrPos += emitPos;
    }
}

void sparkAni::init( std::istream& is )
{
    size_t Nspk = 0; is >> Nspk;
    spkVec.reserve(Nspk);// container for animation

 //   is >> emitPos.x >> emitPos.y >> quadW_all;
    is >> quadW_all;

    is >> doCross >> crossK;
    is >> doDrag >> dragK;
    is >> gravityOn >> gravity.x >> gravity.y;
    is >> emitPeriod;
    is >> tLife_all >> mass_all >> charge_all;
    is >> fireSpeed >> fireAngle;
    fireDir.x = cosf(fireAngle);
    fireDir.y = sinf(fireAngle);
    is >> doFlip >> flipPeriod;
    is >> doSpin >> spinPeriod;

    int tmpIdx = 0;
    size_t numIndexVals; is >> numIndexVals;
    frIdxVec.reserve( numIndexVals );
    for( size_t i = 0; i < numIndexVals; ++i )
    {
        is >> tmpIdx;
        frIdxVec.push_back( tmpIdx );
    }

    is >> xLt >> xRt >> yTop >> yBot;

    CFvec.clear();
    size_t Nfcs; is >> Nfcs;
    CFvec.reserve( Nfcs );
    for( size_t i=0; i<Nfcs; ++i )
        CFvec.push_back( centralForce(is, emitPos) );


  //  if( pSS ) spark::pTxt = &pSS->txt;

    // populate spkVec
    for( size_t i=0; i<Nspk; ++i )
    {
        spark tmpSpk;
    //    tmpSpk.init( mass_all, sf::Vector2f(0.0f,0.0f), emitPos, clr_all, tLife_all );
        size_t idx = i%frIdxVec.size();
        tmpSpk.init( mass_all, sf::Vector2f(0.0f,0.0f), emitPos, idx, -1.0f );
        tmpSpk.quadW = quadW_all;
        tmpSpk.setPosition( emitPos );
        if( pSS ) tmpSpk.setTxtCoords( pSS->getFrRect( frIdxVec[idx], 0 ) );
        spkVec.push_back( tmpSpk );
    }

    // after populating spkVec
    spkIt = spkVec.begin();
    tElapEmit = tElapFlip = tElapSpin = 0.0f;
    frIdx_curr = 0;
}

// write to file
bool sparkAni::saveSparks( const char* fName )
{
    std::ofstream fout( fName );
    if( !fout ) return false;
    sf::Vector2f pos;
  //  for( const spark& spk : spkVec )
  //  if( spk.tLife > 0.0f )
  //  {
  //      pos = spk.getPosition() - emitPos;
  //      fout << pos.x << ' ' << pos.y << ' ' << spk.v.x << ' ' << spk.v.y << ' ' << spk.tLife << '\n';
  //  }

    auto it0 = spkIt;
    do
    {
        getNextSpark();
        if( spkIt->tLife > 0.0f )
        {
            pos = spkIt->getPosition() - emitPos;
            fout << pos.x << ' ' << pos.y << ' ' << spkIt->v.x << ' ' << spkIt->v.y << ' ' << spkIt->tLife << '\n';
        }

    } while( spkIt != it0 );

    return true;
}

// from file
bool sparkAni::loadSparks( const char* fName )
{
    std::ifstream fin( fName );
    if( !fin ) return false;

    sf::Vector2f pos;
    for( spkIt = spkVec.begin(); spkIt != spkVec.end() && ( fin >> pos.x >> pos.y >> spkIt->v.x >> spkIt->v.y >> spkIt->tLife ); ++spkIt )
        spkIt->setPosition( pos + emitPos );
    return true;
}

void sparkAni::reset()// start ani from 0
{
    spkIt = spkVec.begin();
    tElapEmit = tElapFlip  = tElapSpin = 0.0f;
    frIdx_curr = 0;// int
    for( spark& spk : spkVec ) spk.tLife = -1.0f;
}

void sparkAni::update( float dt )
{
 //   std::cout << " hi";
   // apply forces
    const sf::Vector2f zero(0.0f,0.0f);
    sf::Vector2f acc;
    for( spark& spk : spkVec )
    {
        acc = gravityOn ? gravity : zero;
        if( !CFvec.empty() )
        {
            sf::Vector2f pos = spk.getPosition();
            for( auto& cf : CFvec ) acc += cf.F( pos );
        }

        if( doCross )
        {
            // a = k*( v.y, -v.x )
            acc.x += crossK*spk.v.y;
            acc.y -= crossK*spk.v.x;
        }

        if( doDrag ) acc -= dragK*spk.v;

        spk.update( acc, dt );
    }

    // update emitter
    if( doSpin ) spin( fireDir, dt, spinPeriod );
    if( doFlip ) flipX( fireDir, dt, flipPeriod, tElapSpin );

    // fire a spark?
    tElapEmit += dt;
    if( tElapEmit > emitPeriod )
    {
        tElapEmit = 0.0f;// reset
        if( frIdxVec.size() > 0 ) frIdx_curr = ( 1 + frIdx_curr )/frIdxVec.size();
        getNextSpark();
     //   clr_all = getNextColor();
        spkIt->init( mass_all, fireSpeed*fireDir, emitPos, frIdx_curr, tLife_all );
    }
}

bool sparkAni::getNextSpark()
{
 //   spkIt = spkIt+1 == spkVec.end() ? spkVec.begin() : spkIt+1;
    if( ++spkIt == spkVec.end() ) spkIt = spkVec.begin();
    return true;
}

/*
sf::Color sparkAni::getNextColor()
{
    if( clrVec.size() < 2 ) return clr_all;

    ++tCntClr;
    if( tCntClr >= clrPeriodVec[clrIdx] )
    {
        tCntClr = 0;
        clrIdx = ( 1 + clrIdx )%clrVec.size();
    //    std::cout << " clrIdx=" << clrIdx << " clrVec sz=" << clrVec.size();
    }

    return clrVec[clrIdx];
}   */

bool sparkAni::getNextDeadSpark()
{
    return false;
}

bool sparkAni::hit( sf::Vector2f pt )const
{
    pt -= emitPos;
    if( pt.x < xLt || pt.x > xRt ) return false;
    if( pt.y < yTop || pt.y > yBot ) return false;
    return true;
}

bool sparkAni::findBounds()
{
    bool iValsFound = false;
    for( const spark& spk : spkVec )
        if( spk.tLife > 0.0f )
        {
            sf::Vector2f spkPos = spk.getPosition() - emitPos;
            if( !iValsFound )
            {
                iValsFound = true;
                xLt = xRt = spkPos.x;
                yTop = yBot = spkPos.y;
                continue;
            }
            // bounds exist
            if( spkPos.x < xLt ) xLt = spkPos.x;
            else if( spkPos.x > xRt ) xRt = spkPos.x;

            if( spkPos.y < yTop ) yTop = spkPos.y;
            else if( spkPos.y > yBot ) yBot = spkPos.y;
        }

        return iValsFound;
}

void sparkAni::copyToCurve( std::vector<sf::Vertex>& vtxVec, sf::Vector2f pos, sf::Color clr, float dt )
{
    bool wasEmpty = false;
    if( spkVec.empty() )
    {
        wasEmpty = true;
        spark tmpSpk;
        tmpSpk.init( mass_all, fireSpeed*fireDir, emitPos, 0, tLife_all );
        tmpSpk.quadW = quadW_all;
        tmpSpk.setPosition( emitPos );
        spkVec.push_back( tmpSpk );
    }
    else spkVec[0].init( mass_all, fireSpeed*fireDir, emitPos, 0, tLife_all );

    tElapEmit = 0.0f;// reset
    float emitPer0 = emitPeriod;
    sf::Vector2f fireDir0 = fireDir;
    emitPeriod = 1e7;// prvent more emissions
    spark& rs = spkVec[0];
    vtxVec.clear();
    if( doFlip ) vtxVec.reserve( 2.0f*tLife_all/dt + 10 );// 10 is fudge factor
    else vtxVec.reserve( tLife_all/dt + 10 );

    for( float t = 0.0f; t <= tLife_all; t += dt )
    {
        sf::Vertex tmpVtx;
        tmpVtx.position = rs.getPosition() + pos - emitPos;
        tmpVtx.color = clr;
        vtxVec.push_back( tmpVtx );
        update(dt);
    }
    if( doFlip )
    {
        fireDir = fireDir0;
        fireDir.x *= -1.0f;
        rs.init( mass_all, fireSpeed*fireDir, emitPos, 0, tLife_all );
        tElapEmit = 0.0f;
        for( float t = 0.0f; t <= tLife_all; t += dt )
        {
            sf::Vertex tmpVtx;
            tmpVtx.position = rs.getPosition() + pos - emitPos;
            tmpVtx.color = clr;
            vtxVec.push_back( tmpVtx );
            update(dt);
        }
    }

    if( wasEmpty ) spkVec.clear();// restore state
    emitPeriod = emitPer0;
    fireDir = fireDir0;
    tElapEmit = 0.0f;
    frIdx_curr = 0;
}

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

bool flipX( sf::Vector2f& v, float dt, float period, float& tElap )// assign v.x *= -1.0f; each period. update function
{
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

// fireWork type
void fireWork::init( sf::Vector2f Pos, std::istream& is, std::function<spark*(void)> GetSpark, std::function<sf::IntRect(size_t)> GetFrRect, bool* p_anyLive )// fire shell then handle burst
{
    if( !( is && GetSpark && GetFrRect && p_anyLive ) ) return;// shouldn't caller deal w/this ?

    getSpark = GetSpark;
    getFrRect = GetFrRect;
    pAnyLive = p_anyLive;
    pShell = nullptr;
    firePos = Pos;
    is >> frmidShell >> tLifeShell >> fwGrav;
    float angle, spd; is >> angle >> spd;
    velShell0.x = spd*cosf(angle);
    velShell0.y = spd*sinf(angle);

    size_t numLayers; is >> numLayers;
    if( !NfragsVec.empty() ) NfragsVec.clear(); NfragsVec.reserve(numLayers);
    if( !frmidVec.empty() ) NfragsVec.clear(); frmidVec.reserve(numLayers);
    if( !tLifeVec.empty() ) NfragsVec.clear(); tLifeVec.reserve(numLayers);
    size_t NfragTmp, frmidTmp;
    float spdTmp, tLifeTmp;
    for( size_t i = 0; i < numLayers; ++i )
    {
        is >> NfragTmp >> frmidTmp >> spdTmp >> tLifeTmp;
        NfragsVec.push_back( NfragTmp );
        frmidVec.push_back( frmidTmp );
        speedVec.push_back( spdTmp );
        tLifeVec.push_back( tLifeTmp );
    }
    return;
}

void fireWork::update( float dt )
{
    if( pShell )
    {
        if( pShell->tLife <= 0.0f )
        {
            *pAnyLive = true;
            sf::Vector2f spkPos = pShell->getPosition();
            for( size_t i = 0; i < NfragsVec.size(); ++i )// each layer
            {
                sf::Vector2f vLayer( cos( (float)i*3.1416f/NfragsVec[i] ), sin( (float)i*3.1416f/NfragsVec[i] ) );
                vLayer *= speedVec[i];
                for( size_t j = 0; j < NfragsVec[i]; ++j )// each spark in layer
                {
                    pShell = getSpark();
                    spin( vLayer, dt, NfragsVec[i]*dt );
                    pShell->init( 1.0f, vLayer, spkPos, frmidVec[i], tLifeVec[i], 6.0f );
                    pShell->setTxtCoords( getFrRect( frmidVec[i] ) );
                    pShell->fadeRate = 16.0f;
                }
            }
            pShell = nullptr;
        }
    }
    else if( doFire )// fire shell
    {
        doFire = false;
        pShell = getSpark();
        *pAnyLive = true;
        pShell->init( 1.0f, velShell0, firePos, frmidShell, tLifeShell, 12.0f );
        pShell->setTxtCoords( getFrRect( frmidShell ) );
    }
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
}


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
}

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
}

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
