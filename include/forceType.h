#ifndef FORCETYPE_H_INCLUDED
#define FORCETYPE_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

#include "button_types/buttonList.h"
#include "button_types/buttonValOnHit.h"
#include "button_types/slideBar.h"
#include "button_types/floatSpot.h"

struct consForce
{
    float k;
  //   sf::Vector2f ctrPos;
    virtual consForce* clone() const = 0;
    virtual void to_stream( std::ostream& os )const = 0;
    virtual sf::Vector2f F( sf::Vector2f pos )const = 0;
    virtual float U( sf::Vector2f pos )const = 0;
    virtual ~consForce(){}
    sf::Vector2f gradU( sf::Vector2f pos, float ds )const;
    virtual void makeFloatSpot_k( floatSpot& fs ) = 0;
    virtual void makeListControl( buttonList& BL ) = 0;
};

struct centralForce : public consForce
{
    char mode = 'S';// 'S' = spring, 'I' = inverse square
    float r0;
     sf::Vector2f ctrPos;
    centralForce() {}
    void init( sf::Vector2f CtrPos, float K, char Mode, float R0 = -1.0f ) { ctrPos = CtrPos; k = K; mode = Mode; r0 = R0; }
    centralForce( sf::Vector2f CtrPos, float K, char Mode, float R0 ) { init( CtrPos, K, Mode, R0 ); }
    virtual centralForce* clone() const { return new centralForce(*this); }
    virtual void to_stream( std::ostream& os )const;
    virtual sf::Vector2f F( sf::Vector2f pos )const;
    virtual float U( sf::Vector2f pos )const;
    bool init( std::istream& is, sf::Vector2f ofst = sf::Vector2f(0.0f,0.0f) );
    centralForce( std::istream& is, sf::Vector2f ofst = sf::Vector2f(0.0f,0.0f) ) { init( is, ofst ); }
    virtual ~centralForce(){}
    static float minVal, maxVal;
    virtual void makeFloatSpot_k( floatSpot& fs );
    virtual void makeListControl( buttonList& BL );
};

/*
struct inverseSquare : public consForce
{
    inverseSquare(){}
    void init( sf::Vector2f CtrPos, float K ) { ctrPos = CtrPos; k = K; }
    inverseSquare( sf::Vector2f CtrPos, float K ){ init( CtrPos, K ); }
    virtual consForce* clone() const { return new inverseSquare(*this); }
    virtual void to_stream( std::ostream& os )const;
    virtual sf::Vector2f F( sf::Vector2f pos )const;
    virtual float U( sf::Vector2f pos )const;
    void init( std::istream& is, sf::Vector2f ofst = sf::Vector2f(0.0f,0.0f) );
    inverseSquare( std::istream& is, sf::Vector2f ofst = sf::Vector2f(0.0f,0.0f) ) { init( is, ofst ); }
    virtual ~inverseSquare(){}
    virtual void makeFloatSpot_k( floatSpot& fs );
    static float minVal, maxVal;
    virtual void makeListControl( buttonList& BL );
};  */

struct ForceMap
{
    std::vector<sf::Vector2f> forceVec;
    float width, height, ds;
    int rows, cols;// bounding both edges: cols = width/ds + 1, rows = height/ds + 1
    void init( std::vector<consForce*>& pCFvec, sf::Vector2f pos, sf::Vector2f sz, float resolution );
    sf::Vector2f getForce( sf::Vector2f pos )const;
};

//utility
float cross( sf::Vector2f a, sf::Vector2f b );
float dotProduct( sf::Vector2f a, sf::Vector2f b );
float length( sf::Vector2f a );
sf::Vector2f normal( sf::Vector2f a );
sf::Vector2f normalRH( sf::Vector2f a );
sf::Vector2f normalLH( sf::Vector2f a );
sf::Vector2f Rotate( sf::Vector2f a, float angle );
sf::Vector2f toBase( sf::Vector2f a, sf::Vector2f tu );// tv = LHnorm in function (x,y) -> (tu,tv)
sf::Vector2f fromBase( sf::Vector2f a, sf::Vector2f tu );// tv = LHnorm in function (tu,tv) -> (x,y)


#endif // FORCETYPE_H_INCLUDED
