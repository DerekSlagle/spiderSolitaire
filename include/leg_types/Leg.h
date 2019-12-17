#ifndef LEG_H
#define LEG_H

#include<cmath>
#include<functional>

class Leg
{
    public:
        float x0, y0;
        float len;
        Leg *prev, *next;
        Leg( float X0, float Y0, float Len );
        virtual ~Leg();

        virtual float x( float s )const = 0;
        virtual float y( float s )const = 0;
        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s
        const Leg* update( float& rx, float& ry, float& s, float v, float dt )const;
   //     virtual const Leg* update( float& rx, float& ry, float& s, float v, float dt );
        const Leg* updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const;

    protected:

    private:
};

class LegAdapter
{
    public:
    const Leg* pLeg = nullptr;
    std::function<void(float,float)> pSetPosition = nullptr;
    float x=0.0f, y=0.0f, s=0.0f, v=0.0f, v1=0.0f;// v1 = vel on const speed segment
    float xa=0.0f, v0=0.0f, ka=0.0f;
    float xb=0.0f, vf=0.0f, kb=0.0f;
    float offX = 0.0f, offY = 0.0f;
    void update( float dt );
    void init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x=0.0f, float off_y=0.0f  );
    void init_ab( float Xa, float V0, float Xb, float Vf );// defaults disable ab segment motion
};

#endif // LEG_H
