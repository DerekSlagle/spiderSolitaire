// *************************
// copyright(c) 2018 Derek Slagle
//
// ************************

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include "include/button_types/buttonRect.h"
#include "include/button_types/buttonValOnHit.h"
#include "include/button_types/buttonList.h"
#include "include/button_types/radioButton.h"
#include "include/leg_types/linLeg.h"
//#include "include/level/Level.h"
#include "include/level/menu.h"
#include "include/level/spiderSolitaire.h"
#include "include/level/cardPlay.h"
#include "include/level/lvl_spark.h"
#include "include/level/lvl_sparkAni.h"
#include "include/level/mvHitPlay.h"
//#include "include/level/lvl_Sound.h"
//#include "include/card.h"

//const unsigned int wdwW = 1280, wdwH = 720;// window dimensions
unsigned int wdwW = 1600, wdwH = 900;// window dimensions
float Level::winW = (float)wdwW;
float Level::winH = (float)wdwH;

Level* loadLevel( size_t I );

int main()
{
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return EXIT_FAILURE;
    button::init( font, 30 );

    menu main_Menu;
    if( !main_Menu.init() ) { std::cout << "main menu init fail\n"; return 1; }
    main_Menu.pLoadLvl = loadLevel;
    Level::init_stat( &main_Menu );
 //   Level::init_stat( nullptr );
 //   Level::pCurrLvl = loadLevel(0);
    if( !Level::pCurrLvl ) return 2;

    wdwW = floor( abs( Level::winW ) );
    wdwH = floor( abs( Level::winH ) );

 //   sparkAni spk_aniA;
 //   if( !spk_aniA.init( sf::Vector2f(150.0f,600.0f), nullptr, "include/level/lvl_sparkAni_data/savedAnis/flower_7petal.txt" ) ) return 3;
 //   sparkAni* pSA = &spk_aniA;
 //   Level *pMM = &main_Menu, *pCL = Level::pCurrLvl;
 //   std::function<void(float)> pUpFunc = [pSA,pCL,pMM]( float dt ){ if( Level::pCurrLvl == pMM ) pSA->update(dt); };
 //   std::function<void(sf::RenderTarget&)> pDrawFunc = [pSA,pCL,pMM]( sf::RenderTarget& RT ){ if( Level::pCurrLvl == pMM ) pSA->draw(RT); };

    // ready to go!
    sf::RenderWindow RW(sf::VideoMode(wdwW, wdwH), "Levels",  sf::Style::Titlebar | sf::Style::Close);
    RW.setVerticalSyncEnabled(true);
    float dt = 1.0f;

  //  while (RW.isOpen())
  //      if( !Level::run_stat( RW, dt ) ) RW.close();
     //   if( !Level::run_stat( RW, dt, pUpFunc, pDrawFunc ) ) RW.close();

     while (RW.isOpen())
     {
        button::reset();

        sf::Event event;
        while (RW.pollEvent(event))
            if( !Level::handleEvent_stat( event ) )
            { RW.close(); break; }

        Level::update_stat(dt);
    //    if( Level::pCurrLvl == &main_Menu ) spk_aniA.update(dt);

        // draw
        RW.clear( Level::clearColor );// sf::Color(0,64,128) )
    //    if( Level::pCurrLvl == &main_Menu ) spk_aniA.draw(RW);
        Level::draw_stat(RW);
        RW.display();

        if( Level::do_capture )
        {
            Level::do_capture = false;
            sf::Image captImg = RW.capture();
            captImg.saveToFile( "screenCapt.png" );
        }
     }


    Level::cleanup_stat();

    return 0;
}

Level* loadLevel( size_t I )
{
    Level* pLvl = nullptr;

    switch( I )
    {
        case 0 :
        pLvl = new spiderSolitaire;
        break;

        case 1 :
        pLvl = new cardPlay;
        break;

        case 2 :
        pLvl = new lvl_spark;
        break;

        case 3 :
        pLvl = new lvl_sparkAni;
        break;

        case 4 :
        pLvl = new mvHitPlay;
        break;
    }

    if( !pLvl ) return nullptr;
    if( pLvl->init() ) return pLvl;
    else { delete pLvl; return nullptr; }
}
