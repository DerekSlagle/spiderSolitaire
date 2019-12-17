#ifndef FORCECONTROL_H_INCLUDED
#define FORCECONTROL_H_INCLUDED

//#include "../button_types/controlSurface.h"
#include "Control.h"
#include "../button_types/buttStrip.h"
#include "../forceType.h"

class forceControl : public Control
{
    public:
    // stuff on Csurf
    buttonList forceList;
    buttonRect* pFocusButt;// last button on forceList
    controlSurface subSurf1;// for the buttStrips below

    // stuff on subSurf1
    buttStrip crossBS, dampBS, gravBS;

    // central force storage and image spot
    std::vector<sf::CircleShape> forceSpotVec;// for each central force
    std::vector<centralForce> CFvec;


    bool init( sf::Vector2f pos, const char* fName );
    bool LoadForces( const char* fName );
    void dragForceCenters();
    bool dragCondition() const { return Csurf.sel && forceList.sel && pFocusButt && pFocusButt->sel; }

    sf::Vector2f totalForce( sf::Vector2f pos, sf::Vector2f v, float mass = 1.0f );
//    void setPositionCond( sf::Vector2f Pos ) { for( auto& s : forceStripVec ) s.setPosition( Pos ); }
};

#endif // FORCECONTROL_H_INCLUDED
