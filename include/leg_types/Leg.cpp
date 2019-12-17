#include "Leg.h"

Leg::Leg( float X0, float Y0, float Len ): x0(X0), y0(Y0), len(Len), prev(nullptr), next(nullptr)
{
    //ctor
}

Leg::~Leg()
{
    //dtor
}

void Leg::T( float s, float& Tx, float& Ty )const// providde unit tangent to curve at s
{
    static const float ds = 0.01f;
    Tx = x(s+ds) - x(s-ds);
    Ty = y(s+ds) - y(s-ds);
    float magT = sqrtf( Tx*Tx + Ty*Ty );
    Tx /= magT;
    Ty /= magT;
}

const Leg* Leg::updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const
{
    float dy = y(s+v) - y(s);
    float dv = g*dy/v;
//    if( v < g && v > -g ) dv = g;
//    v += dv;
    if( v < 0.0f && v > -g*dt ) v = g*dt;
    else if( v > 0.0f && v < g*dt ) v = -g*dt;
    else v += dv*dt;
    return update( rx, ry, s, v, dt );
}

const Leg* Leg::update( float& rx, float& ry, float& s, float v, float dt )const
{
    s+= v*dt;
    const Leg* pNewLeg = this;
    if( v < 0.0f )
    {
        if( s < 0.0f )
        {
            pNewLeg = prev;
            if( prev )
                s += pNewLeg->len;
            else s = 0.0f;
        }

    }
    else// v > 0
    {
        if( s > len )
        {
            pNewLeg = next;
            if( next )
                s -= len;
            else s = len;
        }
    }

    if( pNewLeg )
    {
        rx = pNewLeg->x(s);
        ry = pNewLeg->y(s);
    }
    else// rest at end of current Leg
    {
        rx = x(s);
        ry = y(s);
    }

    return pNewLeg;
}

// LegAdapter
/*
void LegAdapter::update( float dt )
{
    if( !( pLeg && pSetPosition ) ) return;
    pLeg = pLeg->update( x, y, s, v, dt );
    pSetPosition( x + offX, y + offY );
    v += a*dt;
}   */

void LegAdapter::update( float dt )
{
    if( !( pLeg && pSetPosition ) ) return;
    pLeg = pLeg->update( x, y, s, v, dt );
    pSetPosition( x + offX, y + offY );

    if( s < xa )// 1st accel portion
    {
        v = sqrtf( ka*s + v0*v0 );
    }
    else if( s > xb )// 1st accel portion
    {
        v = sqrtf( kb*(s-xb) + v1*v1 );
    }
    else v = v1;
}

void LegAdapter::init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x, float off_y )
{
    pLeg = p_Leg;
    pSetPosition = p_SetPosition;
    v = v1 = vel;
    offX = off_x;
    offY = off_y;
    s = 0.0f;
    if( pLeg && pSetPosition )
    {
        x = pLeg->x0 + offX;
        y = pLeg->y0 + offY;
        pSetPosition( x, y );
    }

    // default vals for accel segments
    xa = -1.0f;
    xb = pLeg ? 2.0f*pLeg->len : 1000.0f;
    ka = kb = 0.0f;
    v0 = vf = v;
}

// call after init
void LegAdapter::init_ab( float Xa, float V0, float Xb, float Vf )// xa, xb as fraction of leg length
{
    if( pLeg )
    {
        xa = pLeg->len*Xa; v0 = V0;// now xa, xb are absolute length
        xb = pLeg->len*Xb; vf = Vf;
        if( xa > 0.0f && xa <= pLeg->len ) { ka = ( v1*v1 - v0*v0 )/xa; v = v0; }
        if( xb >= xa && xb <= pLeg->len ) kb = ( vf*vf - v1*v1 )/( pLeg->len - xb );
    }


}
