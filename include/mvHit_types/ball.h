#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include "vec2d.h"
//#include "mvHit.h"
#include "regPolygon.h"

class ball : public mvHit
{
    public:
    sf::CircleShape img;// image

    ball( std::istream& fin );
    ball();
    void init( vec2d Pos, vec2d vel, float R, float Mass, float cr, sf::Color clr = sf::Color::Blue );
    virtual mvHit* clone() const { return new ball(*this); }
    virtual ~ball() {}
    virtual void init( std::istream& fin );
    virtual const char* myName() const { return "ball"; }

    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& rRW )const;
    virtual void setPosition( vec2d Pos );
 
    virtual bool hit( const vec2d& pt );
    virtual bool hit( ball& rB );
    virtual bool hit( regPolygon& py );// { return py.hit( *static_cast<ball*>(this) ); }
    virtual bool hit( mvHit& mh );// { return mh.hit( *static_cast<ball*>(this) ); }

    virtual bool intersect( mvHit& mh );
    virtual bool intersect( ball& rB );
    virtual bool intersect( regPolygon& py );

    virtual bool is_inMe( vec2d pt, vec2d& sep, vec2d& N, float& dSep )const;// writes qtys needed for collision response
    bool is_inMe( vec2d pt )const { vec2d sep, N; float dSep; return is_inMe( pt, sep, N, dSep ); }

    virtual bool is_thruMe( vec2d pt1, vec2d pt2, vec2d& Pimp, float& fos )const;// for bulletproofing, laser sighting, etc.

    virtual bool Float( vec2d Nsurf, vec2d Npen, float penAmt, float grav_N, float airDensity, float fluidDensity );
    virtual bool Float( vec2d Nsurf, float grav_N, float Density );
    virtual float project( vec2d vUnit )const { (void)vUnit; return r; }// max projection along vUnit
};


#endif // BALL_H_INCLUDED
