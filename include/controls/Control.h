#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include "../button_types/controlSurface.h"

class Control
{
    public:
    controlSurface Csurf;
    bool open = false;

    Control() { Csurf.homeButt.pHitFunc = [this](){ open = Csurf.homeButt.sel; }; }
    virtual void update( float dt ) { (void)dt; return; }
};


#endif // CONTROL_H_INCLUDED
