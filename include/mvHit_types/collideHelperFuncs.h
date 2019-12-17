#ifndef COLLIDEHELPERFUNCS_H_INCLUDED
#define COLLIDEHELPERFUNCS_H_INCLUDED

// mvHit types
#include "ball.h"
#include "regPolygon.h"

bool load_mvHits( std::istream&, std::vector<mvHit*>& );
void hitTestAll( std::vector<mvHit*>& );


#endif // COLLIDEHELPERFUNCS_H_INCLUDED
