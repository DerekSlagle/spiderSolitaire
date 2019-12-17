#include "collideHelperFuncs.h"

bool load_mvHits( std::istream& is, std::vector<mvHit*>& pMhVec )
{
    size_t nMH = 0;
    is >> nMH;

    for( size_t i=0; i<nMH; ++i )
    {
        char type = 'n';
        is >> type;
        switch( type )
        {
            case 'C' :// circle
            pMhVec.push_back( new ball(is) );
        //    std::cout << "ball\n";
            break;

            case 'R' :// regPolygon
            pMhVec.push_back( new regPolygon(is) );
         //   std::cout << "regPoly\n";
            break;

            default:
            std::cout << "bad mvHit i= " << i << "\n";
            break;
        }
    }

    return true;
}

void hitTestAll( std::vector<mvHit*>& pMhVec )
{
    for( size_t i = 0; i+1 < pMhVec.size(); ++i )
        for( size_t j = i+1; j < pMhVec.size(); ++j )
            pMhVec[i]->hit( *pMhVec[j] );
}
