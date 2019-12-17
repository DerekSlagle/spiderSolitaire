#ifndef BUTTONRECT_H
#define BUTTONRECT_H

#include "button.h"


class buttonRect : public button
{
    public:
        sf::Vector2f sz;
        sf::RectangleShape R[4];
        sf::Text title;

        buttonRect();
        virtual ~buttonRect();
        virtual bool hit()const;
        virtual void draw( sf::RenderTarget& rRW )const;
        virtual void setPosition( sf::Vector2f Pos );
        void init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title = sf::Text() );
        void init( float x, float y, float W, float H, const sf::Text& Title = sf::Text() );
        buttonRect( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const sf::Text& Title = sf::Text() ) { init( x, y, W, H, Title ); pHitFunc = p_HitFunc; }
};

void initButtRects( sf::RectangleShape* R, float W, float H );// drawn
void initButtRects( sf::RectangleShape* R, float W, float H, sf::Vector2f Pos, sf::Color unSelClr, sf::Color unSelMoClr, sf::Color selClr, sf::Color selMoClr );// drawn

#endif // BUTTONRECT_H
