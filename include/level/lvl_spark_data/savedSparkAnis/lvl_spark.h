#ifndef LVL_SPARK_H_INCLUDED
#define LVL_SPARK_H_INCLUDED

#include<random>

#include "Level.h"
#include "../button_types/buttonList.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/floatSpot.h"
#include "../forceType.h"

struct spark
{
    sf::Vertex vtx[3];
    float triW = 4.0f;
    float m = 1.0f;
    sf::Vector2f v;
    float tLife = -1.0f;

    void init( float mass, sf::Vector2f vel, sf::Vector2f pos, sf::Color clr, float t_Life = -1.0f );
    void reset( float t_Life, sf::Vector2f vel, sf::Vector2f pos );
    void setPosition( sf::Vector2f pos );
    void setPosition( float X, float Y ) { setPosition( sf::Vector2f(X,Y) ); }
    void setColor( sf::Color clr ) { vtx[0].color = vtx[1].color = vtx[2].color = clr; }
    sf::Vector2f getPosition() const;
    void update( float dt );// free
    void update( sf::Vector2f acc, float dt ) { v += acc*dt; update(dt); }// accelerated
 //   void update( sf::Vector2f acc, float dt ) { update(dt); v += acc*dt; }// accelerated
 //   void update( sf::Vector2f acc, float dt ) { acc /= 2.0f; v += acc*dt; update(dt); v += acc*dt; }// accelerated
    void draw( sf::RenderTarget& RT ) const { if( tLife > 0.0f ) RT.draw( vtx, 3, sf::Triangles ); }
    bool hitLine_2side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt );
    bool hitLine_1side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt );// incident from pt2-pt1 LH normal
    bool hitCircle( sf::Vector2f ctr, float R, float cr, float dt );// one side

    void emitRand( float vMin, float vMax, float tMin, float tMax, sf::Vector2f pos );
};

class lvl_spark : public Level
{
    public:
    std::vector<spark> sparkVec;
    std::vector<spark>::iterator sparkIt;
    floatSpot fireSpot;// speed or fire rate?

    float tFire = 5.0f, tLife = 20.0f;
    float grav = 0.0f;
    sf::Vector2f sparkVel;
    // some ui
    buttonList sparkList, emitterList;
    bool flipOn = false, spinOn = false, lineOn = false, randOn = false, oneOn = false;
    bool streamOn = false;
    bool* pCurrOn = nullptr;
    slideBar* pSB_angle = nullptr;// used to link mouse scroll wheel
    slideBar* pSB_grav = nullptr;// used to link up/down keys
    buttonRect hitButt;
    float flipPeriod = 20.0f;
    sf::Vertex lineSceVtx[2];// for lineButt emitter
    sf::Vertex line1Vtx[8];// for 1st 1 sider

    // obstacles
    std::vector<sf::CircleShape> circleVec;
    std::vector<sf::Vertex> lineVec;
    float cr = 1.0f, crBall = 1.0f;// coef restitution
    float wallCr = 0.7f;

    // forces
    buttonList forceList;
    std::vector<consForce*> pCFvec;
 //   bool applySpring = true;
 //   bool forceOn = false;
    float diff_ds = 0.01f;

    buttonList crossList;
    buttonValOnHit crossStrip;

    // functions
    lvl_spark():Level() { std::cout << "Hello from lvl_spark ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void cleanup();
    virtual ~lvl_spark() { cleanup(); }
};


// emitter functions
void lineSource( sf::Vector2f a, sf::Vector2f b, float speedDown, float speedSide, sf::Vector2f& pos, sf::Vector2f& vel );
bool flipX( sf::Vector2f& v, float dt, float period );// assign v.x *= -1.0f; each period. update function
void spin( sf::Vector2f& v, float dt, float period );// assign v.x *= -1.0f; each period. update function

#endif // LVL_SPARK_H_INCLUDED
