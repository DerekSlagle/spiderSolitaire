#ifndef LVL_SPARK_H_INCLUDED
#define LVL_SPARK_H_INCLUDED

#include<random>

#include "Level.h"
#include "../button_types/buttonList.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/floatSpot.h"
#include "../spark.h"
#include "../spriteSheet.h"

#include "../button_types/controlSurface.h"

class lvl_spark : public Level
{
    public:

    sparkAni theSA;// simulator runs on this instance
    floatSpot fireSpot;// speed. ************** place with sparkAni.emitPos
    spriteSheet dotSS;
//    buttonRect txtOnOffButt;

    buttonList emitterList;// stream flip spin sparkle rain one OFF Clear
    bool lineOn = false, randOn = false, oneOn = false;// emitter usage here
    bool streamOn = false;// just stream. No flip, spin etc
    bool forceTestOn = false;
    bool* pCurrOn = nullptr;// which bool emitter property is true
    void makeSparkAniControl( sf::Vector2f pos, buttonList& BL, sparkAni& sa );// from fully initialized sparkAni
    bool applyForce = true;

    buttonList sparkList;// 6x strip for: spark size, fireDirection, fireSpeed, firePeriod, gravity, flipPeriod
  //  slideBar* pSB_angle = nullptr;// used to link mouse scroll wheel
 //   slideBar* pSB_grav = nullptr;// used to link up/down keys
 //   slideBar* pSB_spd = nullptr;// used to link up/down keys

    // total fire control
    slideBar angleSB, speedSB;// fire angle spark speed
    controlSurface fireCsurf;// replaces sparkList
    multiSelector fireMS;// size tLife fireDelay gravity
    buttonValOnHit fireVoh;
    buttonRect onOffButt;// toggle value of streaming
    controlSurface emitCsurf;// replaces emitterList
    std::vector<buttonRect> emitButtVec;// clear one test spin flip sparkle rain
    buttonRect* pSelButt = nullptr;
    controlSurface settingsCsurf;// emit parameters
    multiSelector emitMS;// for spin flip sparkle rain map_ds
    buttonValOnHit emitVoh;
    void makeFireControl(void);

    buttonRect hitButt;// toggle colission testing
//    float flipPeriod = 20.0f;//************** sparkAni dm
    sf::Vertex lineSceVtx[2];// for lineButt emitter
    sf::Vertex line1Vtx[8];// for 1st 1 sider
    bool LoadBarriers();

    // obstacles
    std::vector<sf::CircleShape> circleVec;
    std::vector<sf::Vertex> lineVec;
    float cr = 1.0f, crBall = 1.0f;// coef restitution
    float wallCr = 0.7f;

    // forces
    buttonList forceList;
 //   std::vector<consForce*> pCFvec;//************** sparkAni dm
 //   std::vector<floatSpot> FSvec;// for each central force in pCFvec
    std::vector<sf::CircleShape> forceSpotVec;// for each central force in pCFvec
    buttonList crossList;// to open/close 1 strip below
    buttonValOnHit crossStrip;// force perpendicular to velocity
    buttonList dragList;// to open/close 1 strip below
    buttonValOnHit dragStrip;// force perpendicular to velocity
//    buttonValOnHit dragStrip;// force perpendicular to velocity
    float diff_ds = 0.01f;// for partial derivative in update()
    int frIdxFce = 0, frIdxGrad = 0, frIdxMap = 0;
    bool LoadForceControls();

    // alternate masses
    buttonList massList;// to open/close 1 strip below
    buttonValOnHit massStrip;

    buttonRect saveSAbutt;
    buttonList SAsparkLoad;

    // map force in ani area
    sf::Vertex aniArea[5];// box when mapReady
    bool showArea = false;
    buttonRect aniAreaButt;
    void findAniArea();// assigns to above 2 members
    std::vector<sf::Vector2f> forceMap;
    void findForceMap();
    sf::Vector2f getAcc_map( sf::Vector2f pos );
    float xMin_map = 0.0f, xMax_map = 0.0f, yMin_map = 0.0f, yMax_map = 0.0f;// use in findArea, findMap
    float ds_map = 5.0f;// cell dimension
    int rows_map = 0, cols_map = 0;
    bool mapReady = false;
    buttonValOnHit ds_mapStrip;

    // functions
    lvl_spark():Level() { std::cout << "Hello from lvl_spark ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void cleanup();
    virtual ~lvl_spark() { cleanup(); }

    void save_sparkAni_toFile( const char* fname, const char* fnameSparks = nullptr );//const;
};

#endif // LVL_SPARK_H_INCLUDED
