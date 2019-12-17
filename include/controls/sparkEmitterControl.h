#ifndef SPARKEMITTERCONTROL_H_INCLUDED
#define SPARKEMITTERCONTROL_H_INCLUDED

#include "../button_types/controlSurface.h"
#include "../spark.h"// includes many button classes via #include "forceType.h"
#include "Control.h"

struct fireFunc
{
    // 1 parameter variation
    float x, dx, xLow, xHigh;
    float tFire, tLife = 200.0f;// constant period
    int Nsteps = 1;// integer steps from lo to hi so dx = +-(xHigh-xLow)/Nsteps
    int stepCnt = 0;// accum
    sf::Vector2f vel;// varies with x ?
    float vMag;// constant unless varied
    char mode = 'C';// C=cycle, 'O'=oscillate (back&forth), '1'= once (return nullptr until reset?)
    bool goUp = true;// increment arg. dx > 0

    float dtLife = 0.0f, dvMag = 0.0f;// vary tLife and vMag

    // assign botn
    fireFunc* nextFF = nullptr;// unused yet
    std::function<spark*(float&)>* pGetSpark_emit = nullptr;

    int delay, delayCnt;// limit, accumulator #times to fire in same direction. No dx increment

    void init( std::istream& is, std::function<spark*(void)> GetRawSpark );
    void reInit( std::istream& is );
    void reInitHelp();// called by above once dms reassigned
    void init( char Mode, bool iterUp, float Vel, float t_fire, float t_life, float x_low, float x_high, int N_steps, int Delay, std::function<spark*(void)> GetRawSpark );

    fireFunc() {}
    fireFunc( std::istream& is, std::function<spark*(void)> GetRawSpark ) { init( is, getRawSpark ); }

    spark* getSpark( float& per );// emitter calls this
    void reset() { delayCnt = 0; stepCnt = 1; if( dx > 0.0f ) x = xLow; else x = xHigh; }
    std::function<spark*(void)> getRawSpark = nullptr;// fireFunc obtains sparks from here
};

class sparkEmitterControl : public Control
{
    public:
 //   controlSurface Csurf;
    std::function<spark*(void)> pGetSpark = nullptr;
    std::function<void(float)> pSetSparkSz = nullptr;
    emitter* pEmitter = nullptr;// better tie to emitter
    std::vector<fireFunc> fireFuncVec;
    fireFunc* pFireFunc0 = nullptr;

    bool doEmit = false;// use on/offButt.sel instead
    sf::Vector2f fireDir;// maintain
 //   bool* pDoEmit = nullptr;// tie to emitter
 //   std::vector<buttonRect> emitFuncButtVec;
    buttonRect doEmitButt;
    buttonRect fireFuncButt;
    multiSelector ffMS;
    buttonValOnHit ffStrip;
    buttonValOnHit sparkSzStrip, tLifeStrip, firePeriodStrip;//, motionPeriodStrip;//, spinPeriodStrip;
    slideBar fireDirSlider, speedSlider;// gravitySlider;

    bool init( sf::Vector2f pos, const char* fName, emitter& rEmit, std::function<spark*(void)> p_GetSpark, std::function<void(float)> p_SetSparkSz, const char* fNameFirePatt = nullptr );
    spark* getReadySpark( float& fireDelay )const;// for emitter.getSpark pointer member
};

//bool hitRectA( sf::FloatRect Fr );

#endif // SPARKEMITTERCONTROL_H_INCLUDED
