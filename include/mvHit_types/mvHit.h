#ifndef MVHIT_H_INCLUDED
#define MVHIT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
//#include <string>
#include "vec2d.h"

class ball;// fwd declare
class regPolygon;

class mvHit
{
    public:
    static float wdwW, wdwH;
    static float wdwCf;// friction coeff
    static bool windowTopOpen;// allow passage above top of window

    static float drag;
    vec2d pos, v;// position, velocity
    float r;// now a mvHit property
    float m, Cr, Cf = 0.5f;// mass and coefficient of restitution
    bool frictionOn = false;
    bool is_free = true;// new functionality: mvHits as fixed obstacles to other mvHits

    // rotation
    float angPos = 0.0f, angVel = 0.0f;
    float I = 1.0f;// moment of inertia

    // funcs
    mvHit( std::istream& fin ) { init(fin); }
    mvHit() {}
    virtual mvHit* clone() const = 0;
    virtual const char* myName() const = 0;

    virtual ~mvHit() {}
    virtual void init( std::istream& fin );

    virtual void update( float dt ) = 0;
    void update( float dt, vec2d g );
    virtual void draw( sf::RenderTarget& rRW )const = 0;
    virtual void setPosition( vec2d Pos ) = 0;
    void setPosition( float X, float Y ) { setPosition( vec2d(X,Y) ); }
    void setPosition( sf::Vector2f Pos ) { setPosition( vec2d(Pos.x,Pos.y) ); }
    virtual void setRotation( float angle ) { (void)angle; }
    virtual float getRotation()const { return 0.0f; }
    void setState( vec2d Pos, vec2d Vel ) { v = Vel; setPosition(Pos); }
    void setState( sf::Vector2f Pos, sf::Vector2f Vel ) { v = vec2d(Vel.x,Vel.y); setPosition( vec2d(Pos.x,Pos.y) ); }

    virtual void respond( float dV, bool isFric );
    virtual bool hit( const vec2d& pt ) = 0;
    // new funcs
    virtual bool hit( mvHit& ) = 0;
    virtual bool hit( ball& ) = 0;
    virtual bool hit( regPolygon& ) = 0;

    virtual bool intersect( mvHit& ) = 0;
    virtual bool intersect( ball& ) = 0;
    virtual bool intersect( regPolygon& ) = 0;

    virtual bool is_inMe( vec2d pt, vec2d& sep, vec2d& N, float& dSep )const = 0;// writes qtys needed for collision response
    bool is_inMe( vec2d pt )const { vec2d sep, N; float dSep; return is_inMe( pt, sep, N, dSep ); }

    virtual bool is_thruMe( vec2d pt1, vec2d pt2, vec2d& Pimp, float& fos )const = 0;// for bulletproofing, laser sighting, etc.

    virtual bool bounce( float Cf, vec2d N, bool isFric );// base: rigid bounce. N is line of action
    void handleImpact( mvHit& mh, vec2d sep, vec2d N, float dSep );

    virtual bool Float( vec2d Nsurf, vec2d Npen, float penAmt, float grav_N, float airDensity, float fluidDensity ) = 0;
    virtual bool Float( vec2d Nsurf, float grav_N, float Density ) = 0;
    virtual float project( vec2d vUnit )const = 0;
    virtual bool onFire() { return true; }
};

    // utility
    vec2d velCm( const mvHit& A, const mvHit& B );
    float Ek( const mvHit& A, const mvHit& B );
    float Ek_Cm( const mvHit& A, const mvHit& B );
    sf::Vector2f Equate( vec2d v2d );

#endif // MVHIT_H_INCLUDED
