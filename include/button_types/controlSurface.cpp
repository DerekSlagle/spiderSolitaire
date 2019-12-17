#include "controlSurface.h"

bool controlSurface::init( sf::Vector2f PosHmButt, sf::Vector2f PosSurf, sf::Vector2f Sz, const char* label )
{
    pos = PosSurf;
    sz = Sz;
    bkRect.setPosition( pos );
    bkRect.setSize( sz );
    bkRect.setFillColor( sf::Color( 20,20,20) );
    bkRect.setOutlineThickness( 3.0f );
    bkRect.setOutlineColor( sf::Color(0,0,255) );

    float buttW = 50.0f, buttH = 25.0f;
    sf::Text title(label, *button::pFont, 12);
    title.setColor( sf::Color::Black );
    std::function<void(void)> pF = homeButt.pHitFunc;// as assigned in Control ctor
    homeButt.init( PosHmButt.x, PosHmButt.y, buttW, buttH, [this, pF](){ sel = homeButt.sel; if(pF) pF(); }, title );
    pButt_mse = nullptr;// new
    return true;
}

// button mfs
void controlSurface::setSel( bool Sel )
{
    sel = Sel;
    homeButt.setSel(sel);
    if( pButt_mse ) pButt_mse->setSel( Sel );
    if( !sel ) pButt_mse = nullptr;// new
}

void controlSurface::setMO( bool MO )
{
    mseOver = MO;
    if( pButt_mse ) pButt_mse->setMO( MO );
}

bool controlSurface::hit()const// PV
{
    if( homeButt.hit() ) return true;
    if( hitRect( bkRect.getGlobalBounds() ) ) return true;
    return false;
}

void controlSurface::draw( sf::RenderTarget& RT )const// PV
{
    homeButt.draw(RT);
    if( homeButt.sel )
    {
        RT.draw( bkRect );
     //   if( drawAll ) drawAll(RT);
        for( button* pButt : pButtVec ) pButt->draw(RT);
        for( auto& cond : condButtVec )
        {
            button* pButt = cond();
            if( pButt ) pButt->update();
        }
    }

    if( drawAll ) drawAll(RT);
}

bool controlSurface::MseOver()
{
    button* pButt_mseLast = pButt_mse;
    if( pButt_mse && pButt_mse->MseOver() ) return true;// still over
    pButt_mse = nullptr;

    if( homeButt.MseOver() )
        pButt_mse = &homeButt;
 //   else if( homeButt.sel && hitRect( bkRect.getGlobalBounds() ) && MseOverAll )// all the rest
    else if( homeButt.sel && ( !onSurfaceOnly || hitRect( bkRect.getGlobalBounds() ) ) )// all the rest
    {
        if( MseOverAll ) pButt_mse = MseOverAll();
        if( !pButt_mse )
            for( button* pButt : pButtVec )
                if( pButt->MseOver() ) { pButt_mse = pButt; break; }

        if( !pButt_mse )
            for( auto& cond : condButtVec )
            {
                button* pButt = cond();
                if( pButt && pButt->MseOver() ) { pButt_mse = pButt; break; }
            }
    }


    if( pButt_mseLast != pButt_mse ) button::delayCnt = 0;
 //   mseOver = ( pButt_mse != nullptr );

    mseOver = homeButt.mseOver || ( sel && hitRect( bkRect.getGlobalBounds() ) );

    return mseOver;
}

bool controlSurface::hitLeft()
{
 //   if( sel && pButt_mse )// all open
    if( pButt_mse )// all open
    {
        if( button::clickEvent_Lt() != 0 ) { pButt_mse->hitLeft(); sel = homeButt.sel; }
        else if( button::clickEvent_Rt() != 0 ) pButt_mse->hitRight();
        else return false;// no click event

        return true;
    }

    return false;
}

bool controlSurface::hitRight() { if( pButt_mse ) pButt_mse->hitRight(); return mseOver; }// overload in joyButton

void controlSurface::setPosition( sf::Vector2f Pos )// PV
{
    sf::Vector2f dPos = Pos - pos;
 //   pos = Pos;
    bkRect.setPosition( Pos );
    homeButt.setPosition( homeButt.pos + dPos );
    if( setPositionAll ) setPositionAll( Pos );// use for moving conditional buttons
    for( button* pButt : pButtVec ) pButt->setPosition( pButt->pos + dPos );
//    for( button* pButt : condButtVec ) pButt->setPosition( pButt->pos + dPos );
    pos = Pos;
}

void controlSurface::update()
{
    if( sel )
    {
        if( updateAll ) updateAll();
        for( button* pButt : pButtVec ) pButt->update();

        for( auto& cond : condButtVec )
        {
            button* pButt = cond();
            if( pButt ) pButt->update();
        }
    }
}

void controlSurface::init_delayBox()
 {
    if( pButt_mse )
    {
        moValBox[0].color = moValBox[1].color = moValBox[2].color = moValBox[3].color = bkRect.getFillColor();
        pButt_mse->init_delayBox();
    }
    return;
 }

 // non members
 bool hitRect( sf::FloatRect Fr )
 {
    sf::Vector2f pos = button::msePos();
    if( pos.x < Fr.left || pos.x > Fr.left + Fr.width ) return false;
    if( pos.y < Fr.top || pos.y > Fr.top + Fr.height ) return false;
    return true;
 }
