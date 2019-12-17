#ifndef MVHITPLAY_H_INCLUDED
#define MVHITPLAY_H_INCLUDED

#include "Level.h"
#include "../mvHit_types/ball.h"
#include "../mvHit_types/regPolygon.h"
#include "../mvHit_types/collideHelperFuncs.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../graphFuncs.h"

class mvHitPlay : public Level
{
    public:
    std::vector<mvHit*> pMvHitVec;

    controlSurface mhSpinSfc;
    std::vector<buttonRect> buttVec;
    buttonValOnHit spinStrip, gravStrip;
    buttonRect reInitButt;// read init data for pMvHitVec[0], [1] for colission setup
    int idx_curr = 0;
    vec2d gravity;// assigned by gravStrip

    // stuff to hit
    graphFuncs gfPoly;
    polyNomial poly1;
    controlSurface gfcCsurf;
    multiSelector gfcMs;
    buttonValOnHit gfcVoh;
    slideBar gfcSb;

    // functions
    mvHitPlay():Level() { std::cout << "Hello from mvHitPlay ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~mvHitPlay() { cleanup(); }
};

#endif // MVHITPLAY_H_INCLUDED
