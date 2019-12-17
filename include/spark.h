#ifndef SPARK_H_INCLUDED
#define SPARK_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "forceType.h"
#include "spriteSheet.h"

struct spark
{
 //   static sf::Texture* pTxt;
    sf::Vertex vtx[4];
    float quadW = 4.0f;
    float m = 1.0f, q = 0.0f;// mass, charge
    sf::Vector2f v;// velocity
    float tLife = -1.0f;
    float fadeRate = 0.0f;// no fade. Assign opacity = tLife*fadeRate if < 256
    int frIdx = 0;// index to frame on spriteSheet

    void init( float mass, sf::Vector2f vel, sf::Vector2f pos, int FrIdx, float t_Life = -1.0f, float QuadW = 4.0f );
    void reset( float t_Life, sf::Vector2f vel, sf::Vector2f pos );
    void setPosition( sf::Vector2f pos );
    void setPosition( float X, float Y ) { setPosition( sf::Vector2f(X,Y) ); }
    void setColor( sf::Color clr ) { vtx[0].color = vtx[1].color = vtx[2].color = vtx[3].color = clr; }
    sf::Vector2f getPosition() const;
    void setSize( float QuadW ) { quadW = QuadW; setPosition( getPosition() ); }
    void update( float dt );// free
    void update( sf::Vector2f acc, float dt ) { v += acc*dt; update(dt); }// accelerated
 //   void update( sf::Vector2f acc, float dt ) { update(dt); v += acc*dt; }// accelerated
 //   void update( sf::Vector2f acc, float dt ) { acc /= 2.0f; v += acc*dt; update(dt); v += acc*dt; }// accelerated
    void draw( sf::RenderTarget& RT, const sf::Texture* pTxt ) const;// { if( tLife > 0.0f ) RT.draw( vtx, 4, sf::Quads ); }
    bool hitLine_2side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt );
    bool hitLine_2sideA( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt );
    bool hitLine_1side( sf::Vector2f pt1, sf::Vector2f pt2, float cr, float dt );// incident from pt2-pt1 LH normal
    bool hitCircle( sf::Vector2f ctr, float R, float cr, float dt );// one side

    void emitRand( float vMin, float vMax, float tMin, float tMax, sf::Vector2f pos );
    void setTxtCoords( sf::IntRect tr );
};

class emitter
{
    public:
    spriteSheet* pSS = nullptr;
    bool doEmit = false;
    sf::Vector2f emitPos;
    float emitPeriod = 50.0f, tElapEmit = 0.0f;
    std::function<spark*(float&)> getSpark = nullptr;// get a fully initialized spark
    bool update( float dt );// true when spark emitted

    emitter(){};
    void init( sf::Vector2f EmitPos, bool DoEmit, std::function<spark*(float&)> GetSpark, spriteSheet* p_SS )
    { emitPos = EmitPos; doEmit = DoEmit; emitPeriod = 1.0f; getSpark = GetSpark; pSS = p_SS; tElapEmit = 0.0f; }
    emitter( sf::Vector2f EmitPos, float EmitPeriod, std::function<spark*(float&)> GetSpark, spriteSheet* p_SS )
    { init( EmitPos, EmitPeriod, GetSpark, p_SS ); }
};

class sparkAni
{
    public:
    spriteSheet* pSS = nullptr;
    std::vector<spark> spkVec;// container for animation
    std::vector<spark>::iterator spkIt;// will use round-robin for emitting
    std::vector<centralForce> CFvec;// the forces
    bool doCross = false, doDrag = false;// force mods. Velocity depend
    float crossK = 0.0f, dragK = 0.0f;
    bool gravityOn = false;
    sf::Vector2f gravity;
    // emitter params
    sf::Vector2f emitPos;
    float emitPeriod = 50.0f, tElapEmit = 0.0f;
    float tLife_all = 20.0f, mass_all = 1.0f, charge_all = 1.0f;
    float quadW_all = 5.0f;
 //   int frIdx_all = -1;// sentinal value means use vector of values
    float fireSpeed = 10.0f, fireAngle = 0.0f;
    sf::Vector2f fireDir;// maintain at length = 1.0f
    bool doFlip = false, doSpin = false;
    float flipPeriod = 100.0f, tElapFlip = 0.0f, spinPeriod = 100.0f, tElapSpin = 0.0f;
    // spark color pattern
 //   std::vector<sf::Color> clrVec;
    std::vector<int> frIdxVec;// sequence of frames on spriteSheet
    int frIdx_curr = 0;

    // hit and force map
    float xLt, xRt, yTop, yBot;// bounds of ani relative to emitPos

    // mfs
    bool init( sf::Vector2f pos, spriteSheet* p_SS, const char* fName );
    void init( std::istream& is );// from stream
    void to_stream( std::ostream& os );// sb const but must offset force ctrPos before + after
    sparkAni(){}
 //   sparkAni( const char* fName ) { init(fName); }
    sparkAni( sf::Vector2f pos, spriteSheet* p_SS, const char* fName ) { init(pos,p_SS,fName); }

 //   sparkAni( const sparkAni& ) = delete;// copy
    sparkAni( const sparkAni& sa ) { *this = sa; }// copy
    sparkAni& operator=( const sparkAni& );// = delete;// assign

    bool loadSparks( const char* fName );
    bool saveSparks( const char* fName );

    void draw( sf::RenderTarget& RT )const { for( const spark& spk : spkVec ) if( pSS ) spk.draw(RT,&pSS->txt); else spk.draw(RT,nullptr); }
    void update( float dt );
    void setPosition( sf::Vector2f pos );
    sf::Vector2f getPosition()const { return emitPos; }
    bool getNextSpark();
 //   sf::Color getNextColor();// bye!
    bool getNextDeadSpark();
    bool hit( sf::Vector2f pt )const;
    bool findBounds();// iterate over sparks to find xMin, xMax, yMin, yMax. False if no live sparks
    ~sparkAni() {}
    void reset();// start ani from 0

    // utility
    void copyToCurve( std::vector<sf::Vertex>& vtxVec, sf::Vector2f pos, sf::Color clr, float dt = 1.0f );// 1 vertex at eachh dt for tLife_all
};

struct fireWork
{
    spark* pShell = nullptr;
    size_t frmidShell = 0;
    std::vector<size_t> NfragsVec, frmidVec;
    std::vector<float> speedVec, tLifeVec;
    float tLifeShell;
    float fwGrav = 0.16f;
    sf::Vector2f velShell0, firePos;
    std::function<spark*(void)> getSpark = nullptr;// get raw spark
    std::function<sf::IntRect(size_t)> getFrRect = nullptr;
    bool doFire = false;
    bool *pAnyLive = nullptr;

    void init( sf::Vector2f Pos, std::istream& is, std::function<spark*(void)> GetSpark, std::function<sf::IntRect(size_t)> GetFrRect, bool* p_anyLive );// fire shell then handle burst
    void update( float dt );// fire shell then handle burst
  //  void draw( sf::RenderTarget& RT ) const;
};

// emitter functions
void lineSource( sf::Vector2f a, sf::Vector2f b, float speedDown, float speedSide, sf::Vector2f& pos, sf::Vector2f& vel );
bool flipX( sf::Vector2f& v, float dt, float period );// assign v.x *= -1.0f; each period. update function
bool flipX( sf::Vector2f& v, float dt, float period, float& tElap );// assign v.x *= -1.0f; each period. update function
void spin( sf::Vector2f& v, float dt, float period );// assign v.x *= -1.0f; each period. update function

#endif // SPARK_H_INCLUDED
