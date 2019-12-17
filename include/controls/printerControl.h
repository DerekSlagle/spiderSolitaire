#ifndef PRINTERCONTROL_H_INCLUDED
#define PRINTERCONTROL_H_INCLUDED

//#include "../button_types/controlSurface.h"
#include "Control.h"
#include "../button_types/buttStrip.h"
#include "../spark.h"

class sparkPrinter
{
    public:

    sf::Image img;
    sf::Texture txt;
    std::vector<spark> spkVec;
    std::vector<spark>::iterator spkIt;
    bool anyLive = false;// new
    sf::Vector2f pos;// top of vertical print track
    spriteSheet* pSS_spk = nullptr;// for the sparks

    sf::Vertex box[5];

    float dx = 5.0f, dy = 5.0f;// spark spacing. dy along head, dx along 'paper'
    float xLt, xRt, yTop, yBot;// texture bounds
    float printSpeed = 1.0f;
    sf::Vector2u imgSz;
    sf::Vector2f dir;// unit length
    float s = 0.0f;// position along texture. s += printSpeed*dt
    float tElap = 0.0f;
    float tLife_all = 200.0f;
    float Zoom = 1.0f;// applied in update. multiplies printspeed, dy
 //   float szAll = 3.0f;
    bool isPrinting = false;
    bool makeStill = false;// head fixed by default
    bool hiDef = false;
    void startPrinting();
    void makeBox();
    void setPosition( sf::Vector2f Pos ) { pos = Pos; makeBox(); }

    bool init( sf::Vector2f Pos, const char* fname, sf::Vector2f Dir,  float Speed , float dX, float dY, float tLifeAll, float SzAll, size_t Nsparks = 1000 );
    void update( float dt );
    void draw( sf::RenderTarget& RT, bool showBox, bool headOnly ) const;
};

class printerControl : public Control
{
    public:
    sparkPrinter* pSpkPt = nullptr;
    buttonRect printButt, repeatButt, showHeadButt, stillButt, hiDefButt;
    buttonValOnHit theStrip;
    multiSelector theMS;

    bool init( sf::Vector2f pos, sparkPrinter& SpkPt, const char* fName );
    virtual void update( float dt );
};

#endif // PRINTERCONTROL_H_INCLUDED
