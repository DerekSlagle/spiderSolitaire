#ifndef BALLSPIN_H_INCLUDED
#define BALLSPIN_H_INCLUDED

#include "ball.h"

class ballSpin : public ball
{
    public:
    float I, angVel;// moment of inertia, angular velocity
    float Cfs, Cfk;// static, kinetic friction coefficients

    sf::Vertex line[2];// for drawing spin line
    vec2d angPos;

    bool locked = false;

    // funcs
    ballSpin( std::ifstream& fin );
    ballSpin() {}
    virtual ~ballSpin() {}

    virtual void init( std::ifstream& fin );
    virtual void update();
    virtual void update(vec2d g);// with gravity
    virtual void draw( sf::RenderWindow& rRW )const;
    virtual void setPosition( vec2d Pos );

//    virtual float vf();// ball only slides against friction. Full stop is "final" speed
//    virtual float acc( float accAvail );// returns lesser. For ball this is the max
    virtual void respond( float dV, bool isFric );

    // special funcs
    void eStop() { angVel = 0.0f; }// slide toward a stop
};


#endif // BALLSPIN_H_INCLUDED
