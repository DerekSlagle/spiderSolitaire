#include "ball.h"

bool ball::hit( regPolygon& py ) { return py.hit( *static_cast<ball*>(this) ); }
bool ball::hit( mvHit& mh ) { return mh.hit( *static_cast<ball*>(this) ); }

bool ball::intersect( regPolygon& py ) { return py.intersect( *static_cast<ball*>(this) ); }
bool ball::intersect( mvHit& mh ) { return mh.intersect( *static_cast<ball*>(this) ); }

bool ball::intersect( ball& rB ) { return ( pos - rB.pos ).dot( pos - rB.pos ) <= (r + rB.r)*(r + rB.r); }// touching

ball::ball() {}// default

ball::ball( std::istream& fin ) { init(fin); }// from file data

void ball::init( std::istream& fin )// from file data
{
    fin >> pos.x >> pos.y >> v.x >> v.y >> r;
    fin >> m >> Cr;// new
    unsigned int red,g,b;
    fin >> red >> g >> b;
    img.setRadius(r);
    img.setPosition(pos.x, pos.y);
    img.setFillColor( sf::Color(red,g,b) );
    img.setOrigin( r,r );
}

void ball::init( vec2d Pos, vec2d vel, float R, float Mass, float cr, sf::Color clr )
{
    pos = Pos; v = vel; r = R; m = Mass; Cr = cr;
    img.setRadius(r);
    img.setPosition(pos.x, pos.y);
    img.setFillColor( clr );
    img.setOrigin( r,r );
}

const float myPI = 3.1415927f;
const float myPIx2 = 2.0f*myPI;

void ball::update( float dt )// virtual
{
    if( !is_free ) return;// new

    vec2d N(1.0f,0.0f);
    bool Hit = false;

    pos += v*dt;

    angPos += angVel*dt;
    while( angPos > myPIx2 ) angPos -= myPIx2;
    while( angPos < -myPIx2 ) angPos += myPIx2;

    if( pos.x < r )
    {
        pos.x = r;
        Hit = true;
        N.x = 1.0f; N.y = 0.0f;
    }
    else if( pos.x > ball::wdwW - r )
    {
        pos.x = ball::wdwW - r;
        Hit = true;
        N.x = -1.0f; N.y = 0.0f;
    }

    if( Hit ) bounce( ball::wdwCf, N, true );

    Hit = false;

    if( !windowTopOpen && pos.y < r )
    {
        pos.y = r;
        Hit = true;
        N.x = 0.0f; N.y = 1.0f;
    }
    else if( pos.y > ball::wdwH - r )
    {
        pos.y = ball::wdwH - r;
        Hit = true;
        N.x = 0.0f; N.y = -1.0f;
    }

    if( Hit ) bounce( ball::wdwCf, N, true );
    img.setPosition(pos.x, pos.y);
}

void ball::draw( sf::RenderTarget& rRW )const { rRW.draw(img); }

bool ball::hit( ball& rB )
{
// check for collision. Check distance between centers.
    float sepSq = ( pos - rB.pos ).dot( pos - rB.pos );
    if( sepSq > (r + rB.r)*(r + rB.r) ) return false;// not touching

    vec2d sep = rB.pos - pos;// from this center to rB center
    float sepMag = sep.mag();// take the sqrt 1 time
    vec2d N = vec2d(1.0f,0.0f);// unit length. First of local base set. 2nd is LH normal
    if( sepMag > 1.0f ) N = -sep/sepMag;
    handleImpact( rB, sep, N, r + rB.r - sepMag );
    return true;
}

// accepted version works best so far
bool ball::Float( vec2d Nsurf, vec2d Npen, float penAmt, float grav_N, float airDensity, float fluidDensity )
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


bool ball::Float( vec2d Nsurf, float grav_N, float Density )// fully immersed
{
    v = v.to_base(Nsurf);
    float Fbuoy = 3.1416f*r*r*grav_N*Density;
    v.x += Fbuoy/m;
    v -= drag*Density*v*r*2.0f/m;
    v = v.from_base(Nsurf);
    return true;
}

bool ball::hit( const vec2d& pt )
{
    vec2d sep, N;
    float dSep;
    if( is_inMe( pt, sep, N, dSep ) )
    {
        v = v.to_base(N);
        if( v.x < 0.0f ) v.x *= -Cr;// reverse component along sep
        v = v.from_base(N);
        pos += N*dSep;
        return true;
    }

    return false;
}

void ball::setPosition( vec2d Pos )
{
    pos = Pos;
    img.setPosition(pos.x, pos.y);
}

bool ball::is_inMe( vec2d pt, vec2d& sep, vec2d& N, float& dSep )const// sep is relative to ball center
{
    sep = pt - pos;
    float sepMag = sep.mag();
    if( sepMag > r ) return false;

    N = -1.0f*sep/sepMag;
    sep = pt;// new
    dSep = r - sepMag;
    return true;
}

bool ball::is_thruMe( vec2d pt1, vec2d pt2, vec2d& Pimp, float& fos )const// for bulletproofing, laser sighting, etc.
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
