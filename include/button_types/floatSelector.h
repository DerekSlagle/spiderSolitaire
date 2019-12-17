#ifndef FLOATSELECTOR_H_INCLUDED
#define FLOATSELECTOR_H_INCLUDED

#include "buttonRect.h"
#include<iostream>
#include<vector>

struct stripData
{
    std::string label;
    float xMin, xMax, xCurr, xInit;
    std::function<void(float)> pSetFunc = nullptr;
    float* pSetVal = nullptr;
    void init( std::string Label, float Min, float Max, float curr, std::function<void(float)> p_SetFunc )
    { label = Label, xMin = Min; xMax = Max; xCurr = xInit = curr; pSetFunc = p_SetFunc; }
};

class floatSelector : public buttonRect
{
    public:
    sf::Text txtVal, txtVal_min, txtVal_max;// numeric labels
    bool isHoriz = true;// work for vertical too!
    float fVal = 0.0f, fVal_min = 0.0f, fVal_max = 0.0f;
    float iVal = 0.0f;
    float tuneFactor = 0.001f;// fine tune factor for mouse scrollover
    float* pVal = nullptr;// if to write value chosen directly - cast acrdng to is_float
    std::function<void(float)> pSetFunc;

    void scrollTune();

    floatSelector() {}
    virtual ~floatSelector() {}
    virtual void draw( sf::RenderTarget& rRW )const = 0;
    virtual void reInit( const stripData& sd ) {}
    virtual void set_fVal( float f_val ) { fVal = f_val; }
    virtual void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, float* p_val ) = 0;
    virtual void init( float x, float y, float W, float H, const sf::Text& Title, float val_min, float val_max, float init_val, std::function<void(float)> p_SetFunc ) = 0;
    virtual bool hitRight() { set_fVal(iVal); return true; }
};

class multiSelector : public button
{
    public:
    std::vector<buttonRect> selButtVec;
    floatSelector* pFS = nullptr;
    std::vector<stripData> sdataVec;

 //   buttonRect* pBR_curr = nullptr;
 //   buttonRect* pButt_mse = nullptr;

    int idx_mse = -1, idx_curr = 0;// using index into parallel vectors


    void init( sf::Vector2f pos, floatSelector* p_FS, std::istream& is );

    virtual void init_delayBox() { if( pFS ) pFS->init_delayBox(); return; }
    virtual void setSel( bool Sel ) { sel = Sel; }// over in buttonList to open/close list, okBox, colorPicker
    virtual void setMO( bool MO ) { mseOver = MO; }
    virtual bool hit()const;
    virtual void draw( sf::RenderTarget& RT )const;
    virtual bool MseOver();
    virtual bool hitLeft();
    virtual bool hitRight();
 //   virtual bool hitRight() { return mseOver; }// overload in joyButton
    virtual void setPosition( sf::Vector2f Pos );// { return; }
 //   virtual void update();
};

#endif // FLOATSELECTOR_H_INCLUDED
