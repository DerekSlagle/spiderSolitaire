#ifndef CARDPLAY_H_INCLUDED
#define CARDPLAY_H_INCLUDED

#include "card_abs.h"
#include "../button_types/buttonList.h"

class cardPlay : public card_abs
{
    public:
    float card_x0 = 130.0f, card_y0 = 50.0f, card_dx = 100.0f, card_dy = 37.0f, card_dyFD = 15.0f;

    std::vector<card> cardVec;// the 104
//    std::vector<cardList> cardListVec;
    std::vector<cardNode> cardNodeVec;
 //   std::vector<card> cardVec;// the 104 crds in play - to spiderSolitaire.h
    std::vector<cardList> cardListVec;

    // some UI
 //   buttonRect xferButt, xferButt2, xferButt3;
    buttonList xferList;
    buttonRect moveModeButt;// dragDrop or 2 click
    sf::Text moveModeMsg;

    //*** funcs
    virtual bool init();
    cardPlay(): card_abs() {}//{ init(); }

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~cardPlay() { cleanup(); }

    void makeNewGame();
    bool loadFirstGame();
    void handleCardHit();
};

#endif // CARDPLAY_H_INCLUDED
