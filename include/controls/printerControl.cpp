#include "printerControl.h"

bool sparkPrinter::init( sf::Vector2f Pos, const char* fname, sf::Vector2f Dir,  float Speed , float dX, float dY, float tLifeAll, float SzAll, size_t Nsparks )
{
    dx = dX; dy = dY;// spark spacing. dy along head, dx along 'paper'
    pos = Pos;
 //   szAll = SzAll;
    tLife_all = tLifeAll;
    printSpeed = Speed;
    dir = Dir;// unit length
    isPrinting = false;
    hiDef = false;
    s = 0.0f;

    if( !img.loadFromFile( fname ) ) return false;
    if( !txt.loadFromImage( img ) ) return false;
    imgSz = img.getSize();

    xLt = 0.0f;
    xRt = (float)imgSz.x;
    yTop = 0.0f;
    yBot = (float)imgSz.y;

    // make box
    makeBox();
    box[0].color = box[1].color = box[2].color = box[3].color = box[4].color = sf::Color::White;

    std::cout << "\ncells to draw = " << (xRt/dx)*(yBot/dy) << " Nsparks = " << Nsparks;
    // load some sparks
    spkVec.reserve(Nsparks);

    for( size_t i=0; i<Nsparks; ++i )
    {
        spark tmpSpk;
        tmpSpk.init( 1.0f, sf::Vector2f(0.0f,0.0f), pos, 0, -1.0f );
        tmpSpk.quadW = SzAll;
        tmpSpk.setPosition( pos );
        tmpSpk.setColor( sf::Color::White );
        spkVec.push_back( tmpSpk );
    }

    spkIt = spkVec.begin();
    return true;
}

void sparkPrinter::makeBox()
{
    sf::Vector2f dirN( dir.y, -dir.x );// LH ?
    box[0].position = pos;// up rt
    box[1].position = pos + Zoom*yBot*dirN;// dn rt
    box[2].position = box[1].position + Zoom*printSpeed*tLife_all*dir;// dn lt
    box[3].position = pos + Zoom*printSpeed*tLife_all*dir;// up lt
    box[4].position = box[0].position;
}

void sparkPrinter::update( float dt )
{
    if( isPrinting )
    {
        sf::Vector2f dirN( dir.y, -dir.x );// LH ?
        s += printSpeed*dt;
        if( s >= xRt )// printing ends
        {
            isPrinting = false;
            s = tElap = 0.0f;
            box[0].position = pos;// up rt
            box[1].position = pos + Zoom*yBot*dirN;// dn rt
            return;
        }

        tElap += dt;
        if( tElap*printSpeed >= dx )// emit
        {
            tElap = 0.0f;
            unsigned int xc = floor( abs(s) );
            float y = 1.0f;// start 1 pixel into top edge of image
            while( y < yBot )
            {
                unsigned int yc = floor( abs(y) );
                sf::Color pxlClr = img.getPixel( xc, yc );
                if( pxlClr.r != 0 || pxlClr.g != 0 || pxlClr.b != 0 )
                {

                //    if( makeStill ) spkIt->reset( tLife_all, sf::Vector2f(0.0f,0.0f), pos + y*dirN - s*dir );
                    if( makeStill ) spkIt->reset( tLife_all, sf::Vector2f(0.0f,0.0f), pos + Zoom*( y*dirN - s*dir ) );
                    else spkIt->reset( tLife_all, dir*printSpeed*Zoom, pos + Zoom*y*dirN );

                    if( hiDef )
                    {
                        sf::IntRect tr;
                        tr.width = tr.height = floor( spkIt->quadW );
                        tr.left = xc - tr.width/2;
                        tr.top = yc - tr.height/2;
                        spkIt->setTxtCoords( tr );
                     //   spkIt->setColor( sf::Color::White );
                    }
                    else spkIt->setColor( pxlClr );

                    if( ++spkIt == spkVec.end() ) spkIt = spkVec.begin();
                    anyLive = true;
                }

                y += dy;
            }
        }

        if( makeStill )// move head
        {
            box[0].position = pos - Zoom*s*dir;// up rt
            box[1].position = pos + Zoom*( yBot*dirN - s*dir );// dn rt
        }
    }
    return;
}

void sparkPrinter::draw( sf::RenderTarget& RT, bool showBox, bool headOnly ) const
{
    if( anyLive )
    {
        if( hiDef ) for( auto& spk : spkVec ) spk.draw(RT,&txt);
        else for( auto& spk : spkVec ) spk.draw(RT,nullptr);
    }

    if( showBox ) RT.draw( box, (headOnly ? 2 : 5), sf::LinesStrip );
}

void sparkPrinter::startPrinting()
{
    s = tElap = 0.0f;
    isPrinting = true;
}

bool printerControl::init( sf::Vector2f pos, sparkPrinter& SpkPt, const char* fName )
{
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "no printer control data\n"; return false; }

    pSpkPt = &SpkPt;
    sf::Vector2f Sz; fin >> Sz.x >> Sz.y;
    sf::Vector2f HBofst(0.0f,-30.0f);
    Csurf.init( HBofst+pos, pos, Sz, "Printer" );

    float buttW = 50.0f, buttH = 25.0f, spcX = buttW + 10.0f;

    sf::Text label("Print", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    sf::Vector2f ofst; fin >> ofst.x >> ofst.y;
    // offset to 1st control
    printButt.init( pos.x+ofst.x, pos.y+ofst.y, buttW, buttH, [this]() { pSpkPt->startPrinting(); }, label );
    printButt.mode = 'I';
    Csurf.pButtVec.push_back( &printButt );
    label.setString("Repeat");
    repeatButt.init( pos.x+ofst.x+spcX, pos.y+ofst.y, buttW, buttH, nullptr, label );
    Csurf.pButtVec.push_back( &repeatButt );
    label.setString("Head");
    showHeadButt.init( pos.x+ofst.x+2.0f*spcX, pos.y+ofst.y, buttW, buttH, [this](){ if(stillButt.sel) showHeadButt.setSel(true); }, label );
    Csurf.pButtVec.push_back( &showHeadButt );
    label.setString("Still");
    stillButt.init( pos.x+ofst.x+3.0f*spcX, pos.y+ofst.y, buttW, buttH, [this](){ pSpkPt->makeStill = stillButt.sel; showHeadButt.setSel(true); }, label );
    Csurf.pButtVec.push_back( &stillButt );
    label.setString("hiDef");
    hiDefButt.init( pos.x+ofst.x+4.0f*spcX, pos.y+ofst.y, buttW, buttH, [this]()
    {
        pSpkPt->hiDef = hiDefButt.sel;
        if( pSpkPt->hiDef ) for( auto& spk : pSpkPt->spkVec ) spk.setColor( sf::Color::White );
        else for( auto& spk : pSpkPt->spkVec ) spk.setColor( sf::Color::Black );
    }, label );
    Csurf.pButtVec.push_back( &hiDefButt );

    fin >> ofst.x >> ofst.y;
    theMS.init( pos+ofst, &theStrip, fin );
    Csurf.pButtVec.push_back( &theMS );

    Csurf.drawAll = [this](sf::RenderTarget& RT){ pSpkPt->draw( RT, Csurf.sel || pSpkPt->isPrinting, showHeadButt.sel ); };

    // working great now
    if( theMS.sdataVec.size() > 5 )
    {
        stripData* pSD = &theMS.sdataVec[0];// reset for assigns below
        pSD->pSetFunc = [this,pSD](float x){ pSpkPt->printSpeed = x; pSpkPt->makeBox(); };
        theStrip.reInit( *pSD );
        pSpkPt->printSpeed = pSD->xCurr;
        theMS.selButtVec[0].setSel(true);

        ++pSD;
        pSD->pSetFunc = [this,pSD](float x){ pSpkPt->tLife_all = x; pSpkPt->makeBox(); };
        pSpkPt->tLife_all = pSD->xCurr;

        ++pSD;
        pSD->pSetFunc = [this,pSD](float x){ pSpkPt->dir = sf::Vector2f( cosf(x), sinf(x) ); pSpkPt->makeBox(); };
        pSpkPt->dir = sf::Vector2f( cosf(pSD->xCurr), sinf(pSD->xCurr) );

        // new buttons - spark spacing
        ++pSD;
        pSD->pSetFunc = [this,pSD](float x){ pSpkPt->dx = pSpkPt->dy = x; };
        pSpkPt->dx = pSpkPt->dy = pSD->xCurr;

        // spark size = quadW
        ++pSD;
        pSD->pSetFunc = [this,pSD](float x){ for( auto& spk : pSpkPt->spkVec ) spk.setSize(x); };
        for( auto& spk : pSpkPt->spkVec ) spk.setSize( pSD->xCurr );

        // Zoom: zoom*printspeed = spk.v, dy*zoom for spk.pos
        ++pSD;
        pSD->pSetFunc = [this,pSD](float x){ pSpkPt->Zoom = x; pSpkPt->makeBox(); };
        pSpkPt->Zoom = pSD->xCurr;
        SpkPt.makeBox();
    }
    else { std::cout << "\nprinterControl.init() : sdataVec.size() < 3"; return false; }

    return true;
}

void printerControl::update( float dt )
{
    (void)dt;

    bool wasPrinting = pSpkPt->isPrinting;
    pSpkPt->update(dt);
    if( repeatButt.sel && wasPrinting && !pSpkPt->isPrinting )// just finished printing
    {
        pSpkPt->startPrinting();// no delay?
    }
}
