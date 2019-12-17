#include "ballSpin.h"

ballSpin::ballSpin( std::ifstream& fin )// from file data
{
    init(fin);
}

void ballSpin::init( std::ifstream& fin )// from file data
{
    ball::init(fin);
    fin >> I >> Cfs >> Cfk >> angVel;// value given for I is actually k of I = k*m*r*r
    I *= m*r*r;

    sf::Vertex line[2];// for drawing spin line
    angPos.x = 0.8f*r; angPos.y = 0.0f;
    line[0].color = line[1].color = sf::Color(255,255,255);
    line[0].position.x = pos.x + angPos.x; line[0].position.y = pos.y + angPos.y;
    line[1].position.x = pos.x - angPos.x; line[1].position.y = pos.y - angPos.y;
}

void ballSpin::update(vec2d g)// same as base
{
    v += g;
    update();
}

void ballSpin::update()
{
    pos += v;
    angPos = angPos.Rotate( angVel );

     ball::update();

    img.setPosition(pos.x, pos.y);
    line[0].position.x = pos.x + angPos.x; line[0].position.y = pos.y + angPos.y;
    line[1].position.x = pos.x - angPos.x; line[1].position.y = pos.y - angPos.y;
}

//float ballSpin::vf() { return r*angVel; }// ball only slides against friction. Full stop is "final" speed

void ballSpin::respond( float dV, bool isFric )// assumes velocity components are T(x) and N(y) to obstacle
{

 //   float  dV = inCf*v.y*( 1.0f + Cr );// change in T(x) component available
    if( dV < 0.0f ) dV *= -1.0f;// assure dV is positive

 //   v.y *= -Cr;// normal component response

    if( locked )
    {
        angVel = 0.0f;
        // just slide
        if( v.x == 0.0f ) return;// stopped
        if( v.x > 0.0f ){ v.x -= dV; if( v.x < 0.0f ) v.x = 0.0f; }
        else { v.x += dV; if( v.x > 0.0f ) v.x = 0.0f; }
        return;
    }
    if( !isFric ) return;// no change in x component can occur
    if( v.x == r*angVel ) return;// rolling motion acheived

    // speed needs adjustment
    float invK = m*r*r/I;
    float dV_toRoll = ( r*angVel - v.x )/( 1.0f + invK );

    if( dV*dV > dV_toRoll*dV_toRoll )// dV is enough to stop slide
    {
        v.x += dV_toRoll;
        angVel = v.x/r;
    }
    else// sliding continues
    {
        if( dV_toRoll < 0.0f ) dV *= -1.0f;// back to negative
        v.x += dV;
        angVel -= m*r*dV/I;
    }

 //   if( v.x > r*angVel ){ v.x -= dV; if( v.x < r*angVel ) v.x = r*angVel; }
//    else { v.x += dV; if( v.x > r*angVel ) v.x = r*angVel; }

    return;
}

/*
void ballSpin::respond( float inCf, float inCr, bool isFric )// assumes velocity components are T(x) and N(y) to obstacle
{

    float  dV = inCf*v.y*( 1.0f + Cr );// change in T(x) component available
    v.y *= -Cr;
    if( !isFric ) return;// no change in x component can occur
    if( v.x == r*angVel ) return;// target speed acheived

    // speed needs adjustment
    if( dV < 0.0f ) dV *= -1.0f;// assure dV is positive

    float invK = m*r*r/I;
    float dV_toRoll = ( r*angVel - v.x )/( 1.0f + invK );

    if( dV*dV > dV_toRoll*dV_toRoll )// dV is enough to stop slide
    {
        v.x += dV_toRoll;
        angVel = v.x/r;
    }
    else// sliding continues
    {
        if( dV_toRoll < 0.0f ) dV *= -1.0f;// back to negative
        v.x += dV;
        angVel -= m*r*dV/I;
    }

 //   if( v.x > r*angVel ){ v.x -= dV; if( v.x < r*angVel ) v.x = r*angVel; }
//    else { v.x += dV; if( v.x > r*angVel ) v.x = r*angVel; }

    return;
}
*/

void ballSpin::draw( sf::RenderWindow& rRW )const
{
//    rRW.draw(img);
    ball::draw(rRW);
    rRW.draw( line, 2, sf::Lines );
}

void ballSpin::setPosition( vec2d Pos )
{
    pos = Pos;
    img.setPosition(pos.x, pos.y);
    line[0].position.x = pos.x + angPos.x; line[0].position.y = pos.y + angPos.y;
    line[1].position.x = pos.x - angPos.x; line[1].position.y = pos.y - angPos.y;
}

/*
void ballSpin::update()
{
    pos += v;
    angPos = angPos.Rotate( angVel );

    ball::update();

    // bound check the window
    if( pos.x < r )
    {
        pos.x = r;
    //    if( v.x < 0.0f ) v.x *= -Cr;
        if( v.x < 0.0f )
        {
             v.x *= -Cr;
             float vRim = angVel*r;
             float dv = (vRim - v.y)*0.2f;
             v.y += dv;
             angVel -= dv/r;
        }
    }
    else if( pos.x > ball::wdwW - r )
    {
        pos.x = ball::wdwW - r;
   //     if( v.x > 0.0f ) v.x *= -Cr;
        if( v.x > 0.0f )
        {
             v.x *= -Cr;
             float vRim = angVel*r;
             float dv = (vRim + v.y)*0.2f;
             v.y -= dv;
             angVel -= dv/r;
        }
    }

    if( pos.y < r )
    {
        pos.y = r;
        if( v.y < 0.0f )
        {
         v.y *= -Cr;
         float vRim = angVel*r;
             float dv = (vRim + v.x)*0.2f;
             v.x -= dv;
             angVel -= dv/r;
         }
    }
    else if( pos.y > ball::wdwH - r )
    {
        pos.y = ball::wdwH - r;
        if( v.y > 0.0f )
        {
             v.y *= -Cr;
             float vRim = angVel*r;
             float dv = (vRim - v.x)*0.2f;
             v.x += dv;
             angVel -= dv/r;
        }
    }

    img.setPosition(pos.x, pos.y);
    line[0].position.x = pos.x + angPos.x; line[0].position.y = pos.y + angPos.y;
    line[1].position.x = pos.x - angPos.x; line[1].position.y = pos.y - angPos.y;
}
*/
