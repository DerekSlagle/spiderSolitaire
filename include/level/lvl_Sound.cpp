#include "lvl_Sound.h"

bool lvl_Sound::init()
{
    Level::goto_MMButt.setPosition( {Level::winW-160.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::quitButt.setPosition( {Level::winW-80.0f,10.0f} );
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::clearColor = sf::Color(50,50,50);

    sf::Text label("Play", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    // play list
    std::ifstream fin( "include/level/lvl_Sound_data/AudioClips/list_audio_files.txt" );
    if( !fin ) { std::cout << "no sound file data\n"; return false; }
    std::vector<std::string> fnameStrVec;
    std::string fileNameStr;
    std::vector<std::string> buttNameStrVec;
    std::string buttNameStr;
    while( fin >> fileNameStr >> buttNameStr )
    {
        fnameStrVec.push_back( "include/level/lvl_Sound_data/AudioClips/" + fileNameStr );
        buttNameStrVec.push_back( buttNameStr );
    }
    if( fnameStrVec.empty() ) {  std::cout << "\nno sound file names"; return false; }



    float posX = 30.0f, posY = 60.0f, dy = 40.0f;
    soundBuffVec.reserve( fnameStrVec.size() );
    soundVec.reserve( fnameStrVec.size() );
    playControlVec.reserve( fnameStrVec.size() );
    pSlideVec.reserve( fnameStrVec.size() );
    for( size_t i = 0; i < fnameStrVec.size(); ++i )
    {
        soundBuffVec.push_back( sf::SoundBuffer() );
        if ( !soundBuffVec.back().loadFromFile( fnameStrVec[i].c_str() ) ) return false;
        soundVec.push_back( sf::Sound() );
        soundVec.back().setBuffer( soundBuffVec.back() );
        playControlVec.push_back( buttonList() );
        slideBar* pSB = LoadPlayerControl( posX, posY, &playControlVec.back(), &soundVec.back(), buttNameStrVec[i] );
        pSlideVec.push_back( pSB );
        posY += dy;
    }

    for( auto& snd : soundVec )snd.setLoop( true );
    label.setString( "Master Volume" );
    masterVol.init( 30.0f, 10.0f, 140.0f, 22.0f, label, 0.0f, 100.0f, 50.0f, nullptr );
    masterVol.pSetFunc = [this](float x)
    {
        for( size_t i = 0; i < soundVec.size(); ++i )
            soundVec[i].setVolume( x*pSlideVec[i]->fVal/100.0f );
    };
    masterVol.pSetFunc(50.0f);
    button::RegisteredButtVec.push_back( &masterVol );
  //  label.setString( "Loop" );
  //  loopButt.init( 400.0f, 30.0f, 55.0f, 25.0f, [this]() { for( auto& snd : soundVec )snd.setLoop( loopButt.sel ); }, label );
  //  loopButt.setSel(true);
  //  loopButt.pHitFunc();
  //  button::RegisteredButtVec.push_back( &loopButt );
 //   label.setString( "List" );
 //   playList.init( 280.0f, 30.0f, 30.0f*fnameStrVec.size(), 25.0f, fnameStrVec.size(), label, 0 );
 //   playList.pFuncIdx = [this,fnameStrVec](size_t i) { sound_1.stop(); soundBuff_1.loadFromFile( fnameStrVec[i].c_str() ); sound_1.play(); };
 //   button::RegisteredButtVec.push_back( &playList );



    return true;
}

bool lvl_Sound::handleEvent( sf::Event& rEvent ) { return true; }
void lvl_Sound::update( float dt ) { return; }
void lvl_Sound::draw( sf::RenderTarget& RT ) const { return; }
void lvl_Sound::cleanup() { return; }

slideBar* lvl_Sound::LoadPlayerControl( float posX, float posY, buttonList* pPlayControl, sf::Sound* pSnd, std::string& buttName )
{
    float buttW = 55.0f, buttH = 22.0f;// float dy = 2.0f;
    sf::Text label( buttName.c_str(), *button::pFont, 14);
    label.setColor( sf::Color::Black );
    pPlayControl->init( posX, posY, buttW, buttH, label );
    pPlayControl->pHitFunc = [this,pSnd,pPlayControl](){ if( pPlayControl->sel ) pSnd->play(); else pSnd->stop(); };
 //   label.setString( "play" );
 //   playControl.pButtVec.push_back( new buttonRect( posX, posY + 1.0f*(buttH+dy), buttW, buttH, [this,pSnd]() { pSnd->play(); }, label ) );
 //   label.setString( "pause" );
 //   playControl.pButtVec.push_back( new buttonRect( posX, posY + 2.0f*(buttH+dy), buttW, buttH, [this,pSnd]() { pSnd->pause(); }, label ) );
  //  label.setString( "stop" );
 //   playControl.pButtVec.push_back( new buttonRect( posX, posY + 3.0f*(buttH+dy), buttW, buttH, [this,pSnd]() { pSnd->stop(); }, label ) );
 //   label.setString( "pitch" );
 //   playControl.pButtVec.push_back( new buttonValOnHit( posX + buttW + 10.0f, posY + 55.0f, 140.0f, 22.0f, label, 0.5f, 2.0f, 1.0f, [this,pSnd](float x) { pSnd->setPitch(x); } ) );
    label.setString( "vol" ); float iVol = 50.0f; pSnd->setVolume( iVol );
    slideBar* pSB = new slideBar( posX + buttW + 10.0f, posY, 140.0f, 22.0f, label, 0.0f, 100.0f, iVol, [this,pSnd](float x) { pSnd->setVolume( x*masterVol.fVal/100.0f ); } );
    pPlayControl->pButtVec.push_back( pSB );

    pPlayControl->ownButts = true;
    pPlayControl->persist = 3;
    button::RegisteredButtVec.push_back( pPlayControl );

    return pSB;
}
