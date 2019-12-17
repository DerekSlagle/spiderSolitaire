#ifndef LVL_SPARK_H_INCLUDED
#define LVL_SPARK_H_INCLUDED

#include<random>

#include "Level.h"
#include "../button_types/buttonList.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/floatSpot.h"
#include "../spark.h"

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
    std::vector<floatSpot> FSvec;// for each central force in pCFvec
    buttonList crossList;// to open/close 1 strip below
    buttonValOnHit crossStrip;// force perpendicular to velocity
    buttonList dragList;// to open/close 1 strip below
    buttonValOnHit dragStrip;// force perpendicular to velocity
//    buttonValOnHit dragStrip;// force perpendicular to velocity
    buttonRect saveSAbutt;
    float diff_ds = 0.01f;// for partial derivative in update()

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

    void save_sparkAni_toFile( const char* fname )const;
};

#endif // LVL_SPARK_H_INCLUDED
