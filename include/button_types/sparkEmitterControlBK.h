#ifndef SPARKEMITTERCONTROL_H_INCLUDED
#define SPARKEMITTERCONTROL_H_INCLUDED

#include "../spark.h"// includes many button classes via #include "forceType.h"

class sparkEmitterControl : public button
{
    public:
    std::function<spark*(void)> pGetSpark = nullptr;
    std::function<void(float)> pSetSparkSz = nullptr;
    sf::Vector2f sz;
    sf::RectangleShape bkRect;// control surface

    button* pButt_mse = nullptr;

    bool doEmit = false;// use on/offButt.sel instead
    sf::Vector2f fireDir;// maintain
    bool* pDoEmit = nullptr;// tie to emitter
    std::vector<buttonRect> emitFuncButtVec;
    buttonRect homeButt, doEmitButt;
    buttonValOnHit sparkSzStrip, tLifeStrip, firePeriodStrip, flipPeriodStrip, spinPeriodStrip;
    slideBar fireDirSlider, speedSlider;// gravitySlider;

    bool init( sf::Vector2f pos, const char* fName, std::function<spark*(void)> p_GetSpark, std::function<void(float)> p_SetSparkSz );
    spark* getReadySpark( float& fireDelay )const;// for emitter.getSpark pointer member

    // required button functions
    virtual void setSel( bool Sel );// over in buttonList to open/close list, okBox, colorPicker
    virtual void setMO( bool MO );// over in buttonRect, colorPicker
    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& RT )const;
    virtual bool MseOver();// assigns mseOver - virtual due to ValOnHit live display feature
    virtual bool hitLeft();// alters sel - overload in ValOnHit
    virtual bool hitRight();// overload in joyButton
    virtual void setPosition( sf::Vector2f Pos );
    virtual void init_delayBox();
};

bool hitRectA( sf::FloatRect Fr );

#endif // SPARKEMITTERCONTROL_H_INCLUDED
