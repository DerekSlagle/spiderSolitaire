#include "buttonRect.h"

buttonRect::buttonRect(): sz(10.0f,10.0f)
{
    //ctor
}

buttonRect::~buttonRect()
{
    //dtor
}

bool buttonRect::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void buttonRect::draw( sf::RenderTarget& rRW )const
{
    size_t idx = 0;
    if( sel ) idx = 2;
    if( mseOver ) ++idx;

    rRW.draw( R[idx] );
    rRW.draw(title);
}

void buttonRect::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    title.setPosition( title.getPosition() + dPos );

    for( size_t i=0; i<4; ++i )
        R[i].setPosition(pos);
}

/*
void buttonRect::setPosition( sf::Vector2f Pos )
{
    pos = Pos;
    sf::Vector2f dPos;
    sf::FloatRect titleSz = title.getLocalBounds();
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
        R[i].setPosition(pos);
}
*/

void buttonRect::init( float x, float y, float W, float H, const sf::Text& Title )
{
    sf::RectangleShape pR[4];
    initButtRects( pR, W, H );
    buttonRect::init( pR, x, y, Title );
}

void buttonRect::init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title )
{

    pos.x = x;
    pos.y = y;
    sz = pR->getSize();

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
    {
        R[i] = pR[i];
        R[i].setPosition(pos);
    }
}

// **** helpers
void initButtRects( sf::RectangleShape* R, float W, float H, sf::Vector2f Pos, sf::Color unSelClr, sf::Color unSelMoClr, sf::Color selClr, sf::Color selMoClr )
{
    for( size_t i=0; i<4; ++i )
    {
        R[i].setSize( sf::Vector2f(W,H) );
        R[i].setPosition( Pos );
        R[i].setOutlineThickness( -3.0f );
    }

    // unsel
    R[0].setFillColor( unSelClr );
    R[0].setOutlineColor( unSelMoClr );
    // unsel - mo
    R[1].setFillColor( unSelClr );
    R[1].setOutlineColor( selMoClr );

    // sel
    R[2].setFillColor( selClr );
    R[2].setOutlineColor( unSelMoClr );
    // sel - mo
    R[3].setFillColor( selClr );
    R[3].setOutlineColor( selMoClr );
}

void initButtRects( sf::RectangleShape* R, float W, float H )
{
//    initButtRects( R, W, H, sf::Vector2f(50.0f,80.0f), sf::Color(255,242,0), sf::Color(0,0,255), sf::Color(255, 0, 0), sf::Color(0, 255, 0) );
    initButtRects( R, W, H, sf::Vector2f(0.0f,0.0f), sf::Color(200,200,200), sf::Color(0,0,255), sf::Color(255, 255, 255), sf::Color(0, 255, 0) );
 /*   for( size_t i=0; i<4; ++i )
    {
        R[i].setSize( sf::Vector2f(W,H) );
        R[i].setPosition( 50.0f, 80.0f );
        R[i].setOutlineThickness( -3.0f );
    }

    // unsel
    R[0].setFillColor( sf::Color(0, 255, 0) );// green
    R[0].setOutlineColor( sf::Color(0,0,255) );// blue
    // unsel - mo
    R[1].setFillColor( sf::Color(0, 255, 0) );// green
    R[1].setOutlineColor( sf::Color(255,242,0) );// yellow

    // sel
    R[2].setFillColor( sf::Color(255, 0, 0) );// red
    R[2].setOutlineColor( sf::Color(0,0,255) );// blue
    // sel - mo
    R[3].setFillColor( sf::Color(255, 0, 0) );// red
    R[3].setOutlineColor( sf::Color(255,242,0) );// yellow  */
}
