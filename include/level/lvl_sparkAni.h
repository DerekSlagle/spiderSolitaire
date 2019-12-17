#ifndef LVL_SPARKANI_H_INCLUDED
#define LVL_SPARKANI_H_INCLUDED

#include "Level.h"
#include "../spark.h"
#include "../button_types/buttStrip.h"
#include "../controls/sparkEmitterControl.h"
#include "../controls/forceControl.h"
#include "../controls/printerControl.h"
#include "../graphFuncs.h"


class lvl_sparkAni : public Level
{
    public:
    std::vector<sparkAni*> pSpkAniVec;
    std::vector<sf::Vertex> vtxVec;// test sparkAni::copyToCurve
    spriteSheet dotSS;
    sf::Image BkgdImg;
    sf::Texture BkgdTxt;
    sf::Sprite BkgdSpr;

    sparkPrinter spkPrnt;
    emitter spkEmitter;
    sf::CircleShape emitMark;
    sparkEmitterControl emitControl;
    std::vector<spark> sparkVec;
    std::vector<spark>::iterator sparkIt;
    spark* getNextSpark();
    void updateSparks( std::vector<spark>& spkVec, bool& anyLive, float dt );
    bool anyLive_emit = false;
 //   bool anyLive_print = false;

    // obstacles
    std::vector<sf::CircleShape> circleVec;
    std::vector<sf::Vertex> lineVec;
    bool LoadBarriers();
    float cr = 0.8f;// coeff rest

    // graphFuncs controls for them
    graphFuncs gfPoly, gfSine;
    float Amp_s, wvLen_s;
    polyNomial poly1;
    float angFreq_p = 0.01f;

    graphFuncs gfHyper;// hyperbola
    float hypA = 1.0f, hypB = 1.0f;// y*y = hypB*x*x + hypA
    bool topBranch = true;

    graphFuncs gfFlower;
    int numPetals = 3;
    float R_Flwr = 100.0f, Rin_Flwr = 100.0f;//, angFreq_Flwr = 0.0f;

    graphFuncs gfEllip, gfEllip2;
    float elip_a = 1.0f, elip_b = 1.0f;// no explicit time variation dydt = 0
    float elip_a2 = 1.0f, elip_b2 = 1.0f;
    float Amp_e2 = 0.0f, angFreq_e2 = 0.0f, pulseTime_e2 = 0.0f;// modulates as Amp_e2*cosf(angFreq_e2*t)

    graphFuncs gfPolygon;
    polygonReg pgReg;

    polygonIrreg pgIrreg;
    graphFuncs gfPolyIR;
 //   float angFreq_pgn;

    std::vector<graphFuncs*> pGfVec;// so can process all above in loops

    sf::Vertex Nline[2];// view normal to gf curve when key N is held
    bool seeLine = false;

    // controls for them
    buttonList hitList;// list
    std::vector<controlSurface> CsurfVec;
    std::vector<multiSelector> MsVec;
    std::vector<buttonValOnHit> VohVec;
    std::vector<slideBar> SbVec;

    buttonRect showAxesButt;// toggle drawing of coordinate axis
    // emit from graphFuncs
    gfEmitter gfEmit;
    buttonRect gfEmitButt;
    buttonValOnHit hitOfstStrip;// vary the offset from curve when hit

    // vary mass
    buttStrip massBS;

    // forces
    forceControl theFC;
    printerControl thePrinter;
    // special functions
 //   buttonRect LoadSparksNowButt;
    buttonRect fireWorkButt;

    // firework
    fireWork fw1, fw2, fw3;
    sf::Vector2f fwPos1, fwPos2, fwPos3;
    sf::Vector2f fireOfst;

    // functions
    lvl_sparkAni():Level() { std::cout << "Hello from lvl_sparkAni ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void dragSparkAnis();


    void cleanup();
    virtual ~lvl_sparkAni() { cleanup(); }
};

#endif // LVL_SPARKANI_H_INCLUDED
