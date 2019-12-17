#include "forceType.h"

// NOTE: no explicit mode dependence
sf::Vector2f consForce::gradU( sf::Vector2f pos, float ds )const
{
    float Uxy = U( pos );// at (x,y)
    sf::Vector2f dx( ds, 0.0f ), dy( 0.0f, ds );
    sf::Vector2f gdU;
    gdU.x = ( U( pos + dx ) - Uxy )/ds;
    gdU.y = ( U( pos + dy ) - Uxy )/ds;
    return gdU;
}

float centralForce::minVal = 0.01f, centralForce::maxVal = 0.1f;

bool centralForce::init( std::istream& is, sf::Vector2f ofst )
{
    is >> mode;
    if( mode == 'S' )
        is >> ctrPos.x >> ctrPos.y >> r0 >> k;
    else if( mode == 'I' )
        is >> ctrPos.x >> ctrPos.y >> k;

    ctrPos += ofst;
    return true;
}

void centralForce::to_stream( std::ostream& os )const
{
    os << mode << ' ';
    if( mode == 'S' )
        os << ctrPos.x << ' ' << ctrPos.y << ' ' << r0 << ' ' << k << '\n';
    else if( mode == 'I' )
        os << ctrPos.x << ' ' << ctrPos.y  << ' ' << k << '\n';
}

sf::Vector2f centralForce::F( sf::Vector2f pos )const
{
    sf::Vector2f r = ctrPos - pos;
    float rMag = length(r);

    // mode S
    if( mode == 'S' ) return k*r*( rMag - r0 )/rMag;
    // mode I
    float den =  rMag*rMag*rMag;// 1 factor to make r unit length
    return k*r/den;
}

float centralForce::U( sf::Vector2f pos )const
{
    if( mode == 'I' ) return -k/length( ctrPos - pos );

    sf::Vector2f r = ctrPos - pos;
    float disp = length(r) - r0;
    return 0.5f*k*disp*disp;
}

void centralForce::makeFloatSpot_k( floatSpot& fs )
{
    fs.init( ctrPos, sf::Vector2f(15.0f,-8.0f), 4.0f, "spring", 14, sf::Color::Yellow );
    fs.init2( minVal, maxVal, k, "k value", &k );
}

void centralForce::makeListControl( buttonList& BL )
{
    sf::Text label("spring k", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    BL.pButtVec.push_back( new buttonValOnHit( BL.pos.x + BL.sz.x+2.0f, BL.pos.y, 140.0f, 20.0f, label, k/10.0f, k*10.0f, k, &k ) );
    if( mode == 'S' )
    {
        label.setString("spring r0");
        BL.pButtVec.push_back( new buttonValOnHit( BL.pos.x + BL.sz.x+2.0f, BL.pos.y + 40.0f, 140.0f, 20.0f, label, r0/5.0f, r0*5.0f, r0, &r0 ) );
    }
    BL.persist = 3;
    BL.ownButts = true;
}
/*
float inverseSquare::minVal = -10000.0f, inverseSquare::maxVal = 10000.0f;

void inverseSquare::init( std::istream& is, sf::Vector2f ofst ) { is >> ctrPos.x >> ctrPos.y >> k; ctrPos += ofst; }
void inverseSquare::to_stream( std::ostream& os )const { os << "S " << ctrPos.x << ' ' << ctrPos.y << ' ' << k << '\n'; }

sf::Vector2f inverseSquare::F( sf::Vector2f pos )const
{
    sf::Vector2f r = ctrPos - pos;
    float rMag = length(r);
    float den =  rMag*rMag*rMag;// 1 factor to make r unit length
    return k*r/den;
}

float inverseSquare::U( sf::Vector2f pos )const
{
    return -k/length( ctrPos - pos );
}

void inverseSquare::makeFloatSpot_k( floatSpot& fs )
{
    fs.init( ctrPos, sf::Vector2f(15.0f,-8.0f), 4.0f, "invSq", 14, sf::Color::Yellow );
    fs.init2( minVal, maxVal, k, "k value", &k );
}

void inverseSquare::makeListControl( buttonList& BL )
{
    sf::Text label("inv square k", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    BL.pButtVec.push_back( new buttonValOnHit( BL.pos.x + BL.sz.x+2.0f, BL.pos.y, 140.0f, 20.0f, label,  k/10.0f, k*10.0f, k, &k ) );
    BL.persist = 3;
    BL.ownButts = true;
}   */

// ForceMap
void ForceMap::init( std::vector<consForce*>& pCFvec, sf::Vector2f pos, sf::Vector2f sz, float resolution )
{
    width = sz.x;
    height= sz.y;
    ds = resolution;
    cols = 1 + floor(width/ds); rows = 1 + (height/ds);
    forceVec.clear();
    forceVec.resize( rows*cols );// for rows-1 x cols-1 region within
    std::cout << "\nrows_map = " << rows << " cols_map = " << cols << " map sz = " << sizeof( sf::Vector2f )*forceVec.size();
    sf::Vector2f pos_map;
    for( int r = 0; r < rows; ++r )
    {
        for( int c = 0; c < cols; ++c )
        {
            pos_map.x = pos.x + c*ds;
            pos_map.y = pos.y + r*ds;
            forceVec[r*cols+c] *= 0.0f;
            for( auto& fv : pCFvec ) forceVec[r*cols+c] += fv->F( pos_map );
        }
    }

 //   xMin_map -= fireSpot.pos.x; yMin_map -= fireSpot.pos.y;
 //   xMax_map -= fireSpot.pos.x; yMax_map -= fireSpot.pos.y;
}

sf::Vector2f ForceMap::getForce( sf::Vector2f pos )const// given relative to map origin
{
    const sf::Vector2f zero;
    if( pos.x < 0.0f || pos.x > width ) return zero;
    if( pos.y < 0.0f || pos.y > height ) return zero;

    int r = floor( pos.y/ds ), c = floor( pos.x/ds );
    if( r < 0 || r + 2 > rows || c < 0 || c + 2 > cols ) { return zero; }

    float dx = pos.x - ds*c;
    float dy = pos.y - ds*r;
    sf::Vector2f dVx = forceVec[r*cols+c+1] - forceVec[r*cols+c], dVy = forceVec[(r+1)*cols+c] - forceVec[r*cols+c];

    return forceVec[r*cols+c] + ( dx*dVx + dy*dVy )/ds;
}

// math utility
float cross( sf::Vector2f a, sf::Vector2f b ) { return a.x*b.y - a.y*b.x; }
float dotProduct( sf::Vector2f a, sf::Vector2f b ) { return a.x*b.x + a.y*b.y; }
float length( sf::Vector2f a ) { return sqrtf( a.x*a.x + a.y*a.y ); }

sf::Vector2f normal( sf::Vector2f a ) { return a/length(a); }
sf::Vector2f normalRH( sf::Vector2f a ) { return sf::Vector2f(a.y,-a.x); }
sf::Vector2f normalLH( sf::Vector2f a ) { return sf::Vector2f(-a.y,a.x); }

const float myPI = 3.1415927f;
sf::Vector2f Rotate( sf::Vector2f a, float angle )
{
    float ax = a.x*cosf(angle) - a.y*sinf(angle);
    a.y = a.x*sinf(angle) + a.y*cosf(angle);
    a.x = ax;
    return a;
}

sf::Vector2f toBase( sf::Vector2f a, sf::Vector2f tu )// tv = LHnorm in function (x,y) -> (tu,tv)
{
    sf::Vector2f tv = normalRH(tu);
    sf::Vector2f ret;
    ret.x = a.x*tu.x + a.y*tu.y;
    ret.y = a.x*tv.x + a.y*tv.y;
    return ret;
 //   return sf::Vector2f( dotProduct(a,tu), dotProduct(a,tv) );
}

sf::Vector2f fromBase( sf::Vector2f a, sf::Vector2f tu )// tv = LHnorm in function (tu,tv) -> (x,y)
{
    sf::Vector2f tv = normalRH(tu);
    sf::Vector2f ret;
    ret.x = a.x*tu.x + a.y*tv.x;
    ret.y = a.x*tu.y + a.y*tv.y;
    return ret;
}
