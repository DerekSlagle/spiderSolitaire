#ifndef LVL_SOUND_H_INCLUDED
#define LVL_SOUND_H_INCLUDED

#include <SFML/Audio.hpp>
#include "Level.h"
#include "../button_types/buttonList.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/floatSpot.h"
#include "../button_types/radioButton.h"

class lvl_Sound : public Level
{
    public:
    buttonRect loopButt;
    std::vector<buttonList> playControlVec;
    std::vector<slideBar*> pSlideVec;
    radioButton playList;
    buttonList playControl_1;
 //   bool LoadPlayerControl( float posX, float posY );
    slideBar* LoadPlayerControl( float posX, float posY, buttonList* pPlayControl, sf::Sound* pSnd, std::string& buttName );

    std::vector<sf::SoundBuffer> soundBuffVec;
    std::vector<sf::Sound> soundVec;
    slideBar masterVol;
 //   sf::SoundBuffer soundBuff_1;
 //   sf::Sound sound_1;

    // functions
    lvl_Sound():Level() { std::cout << "Hello from lvl_Sound ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvl_Sound() { cleanup(); }
};

#endif // LVL_SOUND_H_INCLUDED
