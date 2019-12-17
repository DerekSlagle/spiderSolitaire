#include "regPolygon.h"
#include "ball.h"

bool regPolygon::hit( mvHit& mh ) { return mh.hit( *static_cast<regPolygon*>(this) ); }
bool regPolygon::intersect( mvHit& mh ) { return mh.intersect( *static_cast<regPolygon*>(this) ); }

bool regPolygon::intersect( ball& rB )
{
    if( ( pos - rB.pos ).mag() < r ) return true;// may be entirely within.

    vec2d sep, N;
    float a;
    return is_inMe( rB, sep, N, a );
}

bool regPolygon::intersect( regPolygon& py )
{
    vec2d sep, N;
    float a;
    return is_inMe( py, sep, N, a );
}

/*
bool regPolygon::hit( regPolygon& rpg )
{
    bool Hit = false;
    vec2d S1, N;
    float a;

    Hit = is_inMe( rpg, S1, N, a );
    if( Hit )
    {
        vec2d S2 = S1 + pos - rpg.pos;
        vec2d S1n(S1.y,-S1.x), S2n(S2.y,-S2.x);
        v -= S1n*angVel;
        rpg.v -= S2n*rpg.angVel;
        handleImpact( rpg, S1, N, a );
        v += S1n*angVel;
        rpg.v += S2n*rpg.angVel;
    }

    return Hit;
}   */


bool regPolygon::hit( regPolygon& rpg )
{
    bool Hit = false;
    vec2d S1, N;
    float a;

    Hit = is_inMe( rpg, S1, N, a );
    if( Hit ) handleImpact( rpg, S1, N, a );

    return Hit;
}


bool regPolygon::hit( ball& rB )
{
    bool Hit = false;
    vec2d sep, N;
    float a;

    Hit = is_inMe( rB, sep, N, a );
    if( Hit ) handleImpact( rB, sep, N, a );

    return Hit;
}

regPolygon::regPolygon() {}// default

regPolygon::regPolygon( std::istream& fin ) { init(fin); }// from file data

void regPolygon::init( std::istream& fin )// from file data
{
    float iAngle = 0.0f;
    fin >> nSides >> r >> iAngle;
    fin >> pos.x >> pos.y >> v.x >> v.y;
    fin >> m >> Cr;// new
 //   sf::Uint8 rd,gn,bu;
    unsigned int rd,gn,bu;
    fin >> rd >> gn >> bu;

    ptVec.reserve(nSides);
    vtxVec.reserve(nSides+1);
    vec2d pt0( r, 0.0f );// starter side
 //   pt0 = pt0.Rotate( iAngle );
    float dAngle = 6.283185f/nSides;
    for( size_t i=0; i<nSides; ++i)
    {
        ptVec.push_back( pt0 );
        vtxVec.push_back( sf::Vertex( sf::Vector2f(pos.x+pt0.x, pos.y+pt0.y), sf::Color(rd,gn,bu) ) );
        pt0 = pt0.Rotate( dAngle );
    }
    vtxVec.push_back( sf::Vertex( sf::Vector2f(pos.x+ptVec[0].x, pos.y+ptVec[0].y), sf::Color(gn,rd,bu) ) );
    setRotation( iAngle );
    I = 1.57f*r*r*r*r*m;
  //  I = 10.0f;
}

const float myPI = 3.1415927f;
const float myPIx2 = 2.0f*myPI;

void regPolygon::update( float dt )
{
    if( !is_free ) return;
    pos += v*dt;

    angPos += angVel*dt;
    while( angPos > myPIx2 ) angPos -= myPIx2;
    while( angPos < -myPIx2 ) angPos += myPIx2;
    setRotation( angPos );

    float dr = 0.0f;
    vec2d N(-1.0f,0.0f);
    bool Hit = false;

    if( pos.x < r )
    {
        N.x = 1.0f; N.y = 0.0f;
        dr = project(-1.0f*N);
        if( pos.x < dr )
        {
            pos.x = dr;
            Hit = true;
        }

    }
    else if( pos.x > regPolygon::wdwW - r )
    {
        N.x = -1.0f; N.y = 0.0f;
        dr = project(-1.0f*N);
        if( pos.x > regPolygon::wdwW - dr )
        {
            pos.x = regPolygon::wdwW - dr;
            Hit = true;
        }
    }

    if( Hit ) bounce( regPolygon::wdwCf, N, true );
    Hit = false;

    if( !windowTopOpen && pos.y < r )
    {
        N.x = 0.0f; N.y = 1.0f;
        dr = project(-1.0f*N);
        if( pos.y < dr )
        {
            pos.y = dr;
            Hit = true;
        }
    }
    else if( pos.y > regPolygon::wdwH - r )
    {
        N.x = 0.0f; N.y = -1.0f;
        dr = project(-1.0f*N);
        if( pos.y > regPolygon::wdwH - dr )
        {
            pos.y = regPolygon::wdwH - dr;
            Hit = true;
        }
    }

    if( Hit ) { bounce( regPolygon::wdwCf, N, true ); }

    for( size_t i=0; i < ptVec.size(); ++i )
    {
        vtxVec[i].position.x = pos.x + ptVec[i].x;
        vtxVec[i].position.y = pos.y + ptVec[i].y;
    }
    vtxVec[ ptVec.size() ].position.x = pos.x + ptVec[0].x;
    vtxVec[ ptVec.size() ].position.y = pos.y + ptVec[0].y;

    return;
}

float regPolygon::project2( vec2d vUnit, vec2d& z )const
{
    float xMax = ptVec[0].dot(vUnit);// initial max
    size_t iMax = 0;// index to max
    for( size_t i = 1; i < ptVec.size(); ++i )
    {
        float x = ptVec[i].dot(vUnit);
        if( x > xMax ){ xMax = x; iMax = i; }
    }

    z = ptVec[iMax];
    return xMax;
}

bool regPolygon::bounce( float Cf, vec2d N, bool isFric ) { return mvHit::bounce( Cf, N, isFric ); }
/*
bool regPolygon::bounce( float Cf, vec2d N, bool isFric )
{
    vec2d zHit;
    project2(-1.0f*N,zHit);
    vec2d zPerp( zHit.y, -zHit.x );
    v -= zPerp*angVel;
    bool Hit = mvHit::bounce( Cf, N, isFric );
    v += zPerp*angVel;
    return Hit;
}   */

void regPolygon::draw( sf::RenderTarget& rRW )const { rRW.draw( &(vtxVec[0]), vtxVec.size(), sf::LinesStrip ); }

//bool regPolygon::Float( vec2d Nsurf, vec2d Npen, float penAmt )
bool regPolygon::Float( vec2d Nsurf, vec2d Npen, float penAmt, float grav_N, float airDensity, float fluidDensity )
{
    float belowSurface = (Npen*penAmt).mag();
    if( Nsurf.dot(Npen) < 0.0f ) belowSurface = 2.0f*r - belowSurface;

    if( belowSurface > 0.0f && belowSurface < 2.0f*r )// partially immersed
    {
        v = v.to_base(Nsurf);
        float Fbuoy = 2.0f*r*grav_N*( belowSurface*fluidDensity + (2.0f*r-belowSurface)*airDensity );
        v.x += Fbuoy/m;
        if( v.x < 0.0f ) v.x -= drag*fluidDensity*v.x*r*2.0f/m;
        v.y -= drag*fluidDensity*v.y*belowSurface/m;
        v = v.from_base(Nsurf);
        return true;
    }

    return false;
}

bool regPolygon::Float( vec2d Nsurf, float grav_N, float Density )
{
    v = v.to_base(Nsurf);
    float Fbuoy = 3.1416f*r*r*grav_N*Density;
    v.x += Fbuoy/m;
    v -= drag*Density*v*r*2.0f/m;
    v = v.from_base(Nsurf);
    return true;
}

bool regPolygon::hit( const vec2d& pt )
{
    vec2d sep;
    vec2d N;
    float a;

    if( is_inMe( pt, sep, N, a ) )
    {
        v = v.to_base(N);
        if( v.x < 0.0f )
        {
            v.x *= -Cr;
        }
        v = v.from_base(N);
        pos += N*a;
        return true;
    }

    return false;
}

bool regPolygon::is_thruMe( vec2d pt1, vec2d pt2, vec2d& Pimp, float& fos )const// for bulletproofing, laser sighting, etc.
{
    vec2d L = pt1 - pt2;
    float magL = L.mag();
    if( magL < 1.0f ) return false;
    vec2d Lu = L/magL;
    vec2d S = pt1 - pos;
    vec2d N = vec2d( Lu.y, -Lu.x );
    float b = Lu.cross(S);
    if( b < 0.0f && b < -r ) return false;// missed
    if( b > 0.0f && b > r )  return false;// missed
    float a = sqrtf( r*r - b*b );
    Pimp = pos + b*N - a*Lu;
    fos = ( pt1 - Pimp ).dot(Lu)/magL;

    return true;
}

bool regPolygon::is_inMe( const regPolygon& rpg, vec2d& sep, vec2d& N, float& dSep )const
{
    float sepSq = ( pos - rpg.pos ).dot( pos - rpg.pos );
    if( sepSq > (r + rpg.r)*(r + rpg.r) ) return false;// not touching

    bool Hit = false;

    for( auto& P : rpg.ptVec )
        if( is_inMe( P + rpg.pos, rpg.pos-pos, sep, N, dSep ) )
        {
            Hit = true;
            break;
        }

    if( !Hit )
        for( auto& P : ptVec )
            if( rpg.is_inMe( P + pos, pos-rpg.pos, sep, N, dSep ) )
            {
                N *= -1.0f;
                Hit = true;
                break;
            }

    return Hit;
}

bool regPolygon::is_inMe( const ball& rB, vec2d& sep, vec2d& N, float& dSep )const
{
    // crude check for collision. Check distance between centers.
    sep = pos - rB.pos;
    float sepMag = sep.mag();
    float sepSq = ( pos - rB.pos ).dot( pos - rB.pos );
    if( sepSq > (r + rB.r)*(r + rB.r) ) return false;// not touching

    // is a point stuck in the ball?
    const vec2d *pPtMin1 = &(ptVec[0]), *pPtMin2 = &(ptVec[1]);// save the 2 closest in case a face hit test follows
    float sSqMin1 = ( rB.r + 2.0f*r )*( rB.r + 2.0f*r );// highest possible value
    float sSqMin2 = sSqMin1;
    bool Hit = false;

    for( const vec2d& P : ptVec )
    {
        vec2d s = pos + P - rB.pos;
        sepMag = s.mag();
        float sSq = s.dot(s);
        if( sSq < rB.r*rB.r )// hit!
        {
            sep = s;
            dSep = rB.r - sepMag;
            N = sep/sepMag;
        //    std::cerr << "point hit\n";
            Hit = true;
        }

        if( sSq < sSqMin1 )
        {
            pPtMin2 = pPtMin1;
            sSqMin2 = sSqMin1;
            pPtMin1 = &P;
            sSqMin1 = sSq;
       //     std::cerr << "new pPtMin1->x = " << pPtMin1->x << '\n';
       //     std::cerr << "new pPtMin2->x = " << pPtMin2->x << '\n';
        }
        else if( sSq < sSqMin2 )
        {
            pPtMin2 = &P;
            sSqMin2 = sSq;
       //     std::cerr << "just new pPtMin2->x = " << pPtMin2->x << '\n';
        }
    }

    // perhaps a face to face hit?
    if( pPtMin1 && pPtMin2 )
    {
        vec2d T = *pPtMin2 - *pPtMin1;
        T /= T.mag();
        vec2d b = *pPtMin1 + pos - rB.pos;
        float bDotT = b.dot(T);
        if( bDotT > 0.0f ) return Hit;
        sep = b - T*bDotT;
        sepMag = sep.mag();
        sepSq = sep.dot(sep);

        if( sepSq < rB.r*rB.r )
        {
          //  std::cerr << "face hit\n";
            dSep = rB.r - sepMag;
            N = sep/sepMag;
            return true;
        }
    }

    return false;
}

bool regPolygon::is_inMe( vec2d pt, vec2d& sep, vec2d& N, float& dSep )const// writes qtys needed for collision response
{
    sep = pt - pos;
    float sepMag = sep.mag();
    if( sepMag > r ) return false;

    // find which vtx the pt is between
    const vec2d *pPt0 = nullptr, *pPt1 = nullptr;
    const vec2d* ppt_1 = nullptr;
 //   vec2d N;
    size_t i=0;

    for( i=0; i< ptVec.size(); ++i )
    {
        ppt_1 = &(ptVec[0]);
        if( i < ptVec.size()-1 ) ppt_1 = &(ptVec[i+1]);

        float cross0 = sep.cross( ptVec[i] );
        float cross1 = sep.cross( *ppt_1 );
 //       if( cross0 == 0.0f ) { N = ptVec[i]; N /= N.mag(); break; }

        if(  cross0 < 0.0f && cross1 > 0.0f )// correct? i just right
        {
            pPt0 = &(ptVec[i]);
            pPt1 = ppt_1;
        //    std::cerr << "i = " << i << '\n';
            break;
        }
    }

    if( pPt0 && pPt1 )// wedge found
    {
        // verify hit
        vec2d u = sep - *pPt0;
        float cross0 = u.cross( *pPt0*-1.0f );
        float cross1 = u.cross( *pPt1 - *pPt0 );

        if( (cross0 > 0.0f && cross1 < 0.0f) )// nailed it !?!
        {
            // hit
        //    std::cout << "i = " << i << '\n';
            N = *pPt0 + *pPt1;
            N /= N.mag();
            N *= -1.0f;// new
            sep = pt;// new
            dSep = u.dot(N);
            return true;
        }
    }

    return false;
}

bool regPolygon::is_inMe( vec2d pt, vec2d ctr, vec2d& sep, vec2d& N, float& dSep )const// writes qtys needed for collision response toward ctr
{
    sep = pt - pos;
    float sepMag = sep.mag();
    if( sepMag > r ) return false;

    // find which vtx the ctr is between
    const vec2d *pPt0 = nullptr, *pPt1 = nullptr;
    const vec2d* ppt_1 = nullptr;
    size_t i=0;

    for( i=0; i< ptVec.size(); ++i )
    {
        ppt_1 = &(ptVec[0]);
        if( i < ptVec.size()-1 ) ppt_1 = &(ptVec[i+1]);

        if( (sep - ptVec[i]).cross( *ppt_1 - ptVec[i] ) > 0.0f ) return false;// pt is outside of polygon

        float cross0 = ctr.cross( ptVec[i] );
        float cross1 = ctr.cross( *ppt_1 );

        if(  cross0 < 0.0f && cross1 > 0.0f )// correct? i just right
        {
            pPt0 = &(ptVec[i]);
            pPt1 = ppt_1;
        //    std::cerr << "i = " << i << '\n';
        //    break;
        }
    }

    if( pPt0 && pPt1 )// wedge found
    {
        // verify hit
        vec2d u = sep - *pPt0;
        //    std::cout << "i = " << i << '\n';
        N = *pPt0 + *pPt1;
        N /= N.mag();
        N *= -1.0f;// new
        sep = pt;// new
        dSep = u.dot(N);
        return true;
    }

    return false;
}

float regPolygon::project( vec2d vUnit )const// max projection along vUnit
{
    float x = 0.0f;
    for( vec2d P : ptVec )
        if( P.dot(vUnit) > x ) x = P.dot(vUnit);

    return x;
}

void regPolygon::setPosition( vec2d Pos )
{
    pos = Pos;
    for( size_t i=0; i < ptVec.size(); ++i )
    {
        vtxVec[i].position.x = pos.x + ptVec[i].x;
        vtxVec[i].position.y = pos.y + ptVec[i].y;
  //      equate( vtxVec[i].position, pos + ptVec[i] );
    }

    vtxVec[ ptVec.size() ].position.x = pos.x + ptVec[0].x;
    vtxVec[ ptVec.size() ].position.y = pos.y + ptVec[0].y;
 //   equate( vtxVec[ ptVec.size() ].position, pos + ptVec[0] );
}

float regPolygon::getRotation()const
{
    return atan2f( ptVec[0].y, ptVec[0].x );
}

void regPolygon::setRotation( float angle )
{
    angPos = angle;
    vec2d pt0( r, 0.0f );// starter side
    pt0 = pt0.Rotate( angle );
    float dAngle = 6.283185f/nSides;
    for( size_t i=0; i<nSides; ++i)
    {
        ptVec[i] = pt0;
//        equate( vtxVec[i], pos + pt0 );
        vtxVec[i].position.x = pos.x + pt0.x;
        vtxVec[i].position.y = pos.y + pt0.y;
        pt0 = pt0.Rotate( dAngle );
    }
    vtxVec.back().position = vtxVec.front().position;
}

void regPolygon::setRadius( float R )
{
    r = R;
    setRotation( getRotation() );
}
