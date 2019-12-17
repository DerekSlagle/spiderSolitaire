#include "sparkEmitterControl.h"

bool sparkEmitterControl::init( sf::Vector2f pos, const char* fName, std::function<spark*(void)> p_GetSpark, std::function<void(float)> p_SetSparkSz )
{
    pGetSpark = p_GetSpark;
    pSetSparkSz = p_SetSparkSz;

    std::ifstream fin( fName );
    if( !fin ) { std::cout << "no SA control data\n"; return false; }

    fin >> sz.x >> sz.y;
    bkRect.setPosition( pos );
    bkRect.setSize( sz );
    bkRect.setFillColor( sf::Color( 20,20,20) );


    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    float buttHgt = 25.0f;
    float dy = 32.0f;

    sf::Text label("sparks", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    homeButt.init( pos.x, pos.y-buttHgt-5.0f, 80.0f, buttHgt, nullptr, label );
    pos.x += 10.0f; pos.y += 10.0f;// offset to 1st control
    label.setString("on/off");
    doEmitButt.init( pos.x, pos.y, 50.0f, buttHgt, [this](){ if( pDoEmit ) *pDoEmit = doEmitButt.sel; }, label );
    pos.y += buttHgt + 20.0f;
    label.setString("spark size");
    float minVal, maxVal, iVal; fin >> minVal >> maxVal >> iVal;
    sparkSzStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, pSetSparkSz );

    label.setString("fire direction");
    pos.y += stripHgt + dy;
    std::function<void(float)> p_SetFunc = [this]( float x )
    {
        fireDir.x = cosf(x); fireDir.y = sinf(x);
    };
    float iAngle; fin >> iAngle;
    fireDirSlider.init( pos.x, pos.y, stripLen, stripHgt, label, -4.0f, 4.0f, iAngle, p_SetFunc );
    p_SetFunc( iAngle );

    label.setString("spark speed"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    speedSlider.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, nullptr );

    label.setString("life time"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    tLifeStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, nullptr );

    label.setString("fire delay"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    firePeriodStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, nullptr );

    label.setString("flip period"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    flipPeriodStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, nullptr );

    label.setString("spin period"); pos.y += stripHgt + dy;
    fin >> minVal >> maxVal >> iVal;
    spinPeriodStrip.init( pos.x, pos.y, stripLen, stripHgt, label, minVal, maxVal, iVal, nullptr );
    return true;
}

spark* sparkEmitterControl::getReadySpark( float& fireDelay )const
{
    spark* pSpk = nullptr;
    if( pGetSpark ) pSpk = pGetSpark();
    if( pSpk )// got one
    {
        pSpk->init( 1.0f, speedSlider.fVal*fireDir, sf::Vector2f(300.0f,300.0f), pSpk->frIdx, tLifeStrip.fVal, sparkSzStrip.fVal );
    }
    fireDelay = firePeriodStrip.fVal;
    return pSpk;
}

// button mfs
void sparkEmitterControl::setSel( bool Sel ) { sel = Sel; }// over in buttonList to open/close list, okBox, colorPicker

void sparkEmitterControl::setMO( bool MO ) { mseOver = MO; }// over in buttonRect, colorPicker

bool sparkEmitterControl::hit()const// PV
{
    return false;
}

void sparkEmitterControl::draw( sf::RenderTarget& RT )const// PV
{
    homeButt.draw(RT);
    if( homeButt.sel )
    {
        RT.draw( bkRect );
        doEmitButt.draw(RT);
        sparkSzStrip.draw(RT); tLifeStrip.draw(RT); firePeriodStrip.draw(RT);
        flipPeriodStrip.draw(RT); spinPeriodStrip.draw(RT);
        fireDirSlider.draw(RT); speedSlider.draw(RT);
    }
}

bool sparkEmitterControl::MseOver()
{
    button* pButt_mseLast = pButt_mse;
    if( pButt_mse && pButt_mse->MseOver() ) return true;// still over
    pButt_mse = nullptr;

    if( homeButt.MseOver() )
    {
        pButt_mse = &homeButt;
    }
    else if( homeButt.sel && hitRectA( bkRect.getGlobalBounds() ) )// all the rest
    {
    //    pButt_mse = nullptr;
        if( doEmitButt.MseOver() ) pButt_mse = &doEmitButt;
        else if( sparkSzStrip.MseOver() ) pButt_mse = &sparkSzStrip;
        else if( tLifeStrip.MseOver() ) pButt_mse = &tLifeStrip;
        else if( firePeriodStrip.MseOver() ) pButt_mse = &firePeriodStrip;
        else if( flipPeriodStrip.MseOver() ) pButt_mse = &flipPeriodStrip;
        else if( spinPeriodStrip.MseOver() ) pButt_mse = &spinPeriodStrip;
        else if( fireDirSlider.MseOver() ) pButt_mse = &fireDirSlider;
        else if( speedSlider.MseOver() ) pButt_mse = &speedSlider;
    }

    if( pButt_mseLast != pButt_mse ) button::delayCnt = 0;

    return pButt_mse != nullptr;
}

bool sparkEmitterControl::hitLeft()
{
 //   if( sel && pButt_mse )// all open
    if( pButt_mse )// all open
    {
        if( button::clickEvent_Lt() != 0 ) pButt_mse->hitLeft();
        else if( button::clickEvent_Rt() != 0 ) pButt_mse->hitRight();
        else return false;// no click event

        return true;
    }

    return false;
}

bool sparkEmitterControl::hitRight() { return mseOver; }// overload in joyButton

void sparkEmitterControl::setPosition( sf::Vector2f Pos )// PV
{
    pos = Pos;
    bkRect.setPosition( Pos );
    homeButt.setPosition( Pos );
    doEmitButt.setPosition( Pos );
    sparkSzStrip.setPosition( Pos );
    tLifeStrip.setPosition( Pos );
    firePeriodStrip.setPosition( Pos );
    flipPeriodStrip.setPosition( Pos );
    spinPeriodStrip.setPosition( Pos );
    fireDirSlider.setPosition( Pos );
    speedSlider.setPosition( Pos );
}

void sparkEmitterControl::init_delayBox()
 {
    if( pButt_mse )
    {
        pButt_mse->init_delayBox();
        moValBox[0].color = moValBox[1].color = moValBox[2].color = moValBox[3].color = bkRect.getFillColor();
    }
    return;
 }

 // non members
 bool hitRectA( sf::FloatRect Fr )
 {
    sf::Vector2f pos = button::msePos();
    if( pos.x < Fr.left || pos.x > Fr.left + Fr.width ) return false;
    if( pos.y < Fr.top || pos.y > Fr.top + Fr.height ) return false;
    return true;
 }
