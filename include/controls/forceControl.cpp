#include "forceControl.h"

bool forceControl::init( sf::Vector2f pos, const char* fName )
{
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "no Force control data\n"; return false; }

    sf::Vector2f Sz; fin >> Sz.x >> Sz.y;
    sf::Vector2f HBofst(0.0f,-30.0f);
    Csurf.init( pos+HBofst, pos, Sz, "Forces" );

    float stripLen, stripHgt; fin >> stripLen >> stripHgt;
    float dy = stripHgt + 10.0f;

    sf::Text label("on/off", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    sf::Vector2f ofst;

    fin >> Sz.x >> Sz.y; fin >> ofst.x >> ofst.y;
    subSurf1.init( pos+ofst+HBofst, pos+ofst, Sz, "other" );
    Csurf.pButtVec.push_back( &subSurf1 );

    fin >> ofst.x >> ofst.y;
    float minVal, maxVal, iVal; fin >> minVal >> maxVal >> iVal;
    crossBS.init( pos + ofst, sf::Vector2f(70.0f,8.0f), 120.0f, 20.0f, "cross", "k value", minVal, maxVal, iVal, nullptr );
 //   Csurf.pButtVec.push_back( &crossBS );
    subSurf1.pButtVec.push_back( &crossBS );
    fin >> ofst.x >> ofst.y;
    fin >> minVal >> maxVal >> iVal;
    dampBS.init( pos + ofst, sf::Vector2f(70.0f,8.0f), 120.0f, 20.0f, "Drag", "k value", minVal, maxVal, iVal, nullptr );
 //   Csurf.pButtVec.push_back( &dampBS );
    subSurf1.pButtVec.push_back( &dampBS );
    fin >> ofst.x >> ofst.y;
    fin >> minVal >> maxVal >> iVal;
    gravBS.init( pos + ofst, sf::Vector2f(70.0f,8.0f), 120.0f, 20.0f, "Gravity", "px/fr2", minVal, maxVal, iVal, nullptr );
 //   Csurf.pButtVec.push_back( &gravBS );
    subSurf1.pButtVec.push_back( &gravBS );

 //   LoadForces( "include/level/lvl_sparkAni_data/Forces.txt" );
    size_t Nforces; fin >> Nforces;
    CFvec.reserve( Nforces );
    centralForce tmpForce;
    for( size_t i = 0; i < Nforces; ++i )
    {
        if( tmpForce.init(fin) ) CFvec.push_back( tmpForce );
        else break;
    }

    // force list
    fin >> ofst.x >> ofst.y;
    float buttW = 55.0f, buttH = 22.0f; dy = 2.0f;
    sf::CircleShape tmpCircle;
    tmpCircle.setFillColor( sf::Color::Yellow );
    tmpCircle.setRadius(5.0f);
    tmpCircle.setOrigin(5.0f,5.0f);
    label.setString("Fce List");
    forceList.init( pos.x + ofst.x, pos.y + ofst.y, buttW, buttH, label );

    if( CFvec.size() == 1 )// no sub list for 1 force
    {
        CFvec[0].makeListControl( forceList );
        tmpCircle.setPosition( CFvec[0].ctrPos );
        forceSpotVec.push_back( tmpCircle );
    }
    else
    {
        for( centralForce& rCF : CFvec )
        {
            pos.y += buttH + dy;
            label.setString( "fce" );
            buttonList* pBL = new buttonList( pos.x+ofst.x, pos.y+ofst.y, buttW, buttH, label );
            rCF.makeListControl( *pBL );
            forceList.pButtVec.push_back( pBL );
            tmpCircle.setPosition( rCF.ctrPos );
            forceSpotVec.push_back( tmpCircle );
            pBL->ownButts = true;
        }
    }

    label.setString("MY AREA");
    pos.y += buttH + dy;
    pFocusButt = new buttonRect( pos.x+ofst.x, pos.y+ofst.y, buttW, buttH, nullptr, label );
    forceList.pButtVec.push_back( pFocusButt );

    forceList.persist = 3;
    forceList.ownButts = true;

    Csurf.pButtVec.push_back( &forceList );
    Csurf.homeButt.pHitFunc = [this](){ if( pFocusButt ) pFocusButt->setSel(false); };

    return true;
}

bool forceControl::LoadForces( const char* fName )
{
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "no force data\n"; return false; }

    centralForce tmpForce;
    while( tmpForce.init(fin) && fin )
        CFvec.push_back( tmpForce );

    std::cout << "\nLoaded " << CFvec.size() << " forces";

    return true;
}

sf::Vector2f forceControl::totalForce( sf::Vector2f pos, sf::Vector2f v, float mass )
{
    sf::Vector2f acc(0.0f,0.0f);
    if( forceList.sel )for( auto& fv : CFvec ) acc += fv.F( pos );

    if( subSurf1.sel )
    {
        if( crossBS.sel ) { acc.x += crossBS.fVal()*v.y/mass; acc.y -= crossBS.fVal()*v.x/mass; }
        if( dampBS.sel ) acc -= dampBS.fVal()*v/mass;
        if( gravBS.sel ) acc.y += gravBS.fVal();
    }
    return acc;
}

void forceControl::dragForceCenters()
{
 //   if( !( Csurf.sel && forceList.sel && pFocusButt && pFocusButt->sel ) ) return;
    if( !dragCondition() ) return;

    static sf::Vector2f grabPos;
    static int grabIdx = -1;
    if( button::clickEvent_Lt() == 1 )// grab
    {
        for( int i = 0; i < (int)forceSpotVec.size(); ++i )
        {
            float R = forceSpotVec[i].getRadius();
            sf::Vector2f sep = button::msePos() - forceSpotVec[i].getPosition();
            if( dotProduct(sep,sep) < R*R )
            {
                grabIdx = i;
                grabPos = button::msePos() - forceSpotVec[i].getPosition();
                break;
            }
        }
    }
    else if( grabIdx >= 0 )
    {
        if( button::clickEvent_Lt() == -1 )// release
        {
            std::cout << "\nCFvec[" << grabIdx << "].ctrPos = " << CFvec[grabIdx].ctrPos.x << ", " << CFvec[grabIdx].ctrPos.y;
            grabIdx = -1;// release
        }
        else if( button::mseDnLt && button::mseMoved )// drag
        {
            CFvec[grabIdx].ctrPos = button::msePos() - grabPos;
            forceSpotVec[grabIdx].setPosition( button::msePos() - grabPos );
        }
    }
}
