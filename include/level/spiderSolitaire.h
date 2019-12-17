#ifndef SPIDERSOLITAIRE_H_INCLUDED
#define SPIDERSOLITAIRE_H_INCLUDED

#include "card_abs.h"

#include "../button_types/buttonRect.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/buttonList.h"
#include "../button_types/radioButton.h"
#include "../spriteSheet.h"
#include "../spark.h"

class spiderSolitaire : public card_abs
{
    public:
    float card_x0 = 130.0f, card_y0 = 50.0f, card_dx = 100.0f, card_dy = 37.0f, card_dyFD = 15.0f;
    sf::Vector2f drawPos, completePos;
    size_t numSuits = 1;// 1, 2, 4 are possible values

    size_t numCols = 10;
    std::vector<cardNode> cardNodeVec;
    std::vector<card> cardVec;// the 104 crds in play - to spiderSolitaire.h
    std::vector<cardList> cardCol;// the 104 crds in play - to spiderSolitaire.h
    cardList completeList;
    linLeg triLeg[3];// for A-K to complete list

    // for undo history via key U
    std::vector<cardList*> sceListHistory, destListHistory;
    std::vector<cardNode*> sceNodeHistory;
    std::vector<bool> flipHistory;
    // for card selection box
    bool isSelection = false;
    bool drawDestBox = false;
//    sf::Vertex selectBox[5], destBox[5];// destBox drawn on mouseover a column which selected works for

    int score = 0;
    sf::Text scoreMsg, scoreMsgNum;
    sf::Text drawMsgNum, completeMsgNum;
    sf::Text winMsg;

 //   size_t cnum = 0;// ???

 //   bool dealingAll = false, dealingLayer = false;// = false to stop dealing in update
    bool movingSet = false;//, movingPartialSet = false;
    bool gameWon = false;

    // control stuff
    buttonRect undoButt, newGameButt, saveGameButt, LdSvGameButt;
    radioButton suitSelect;
    radioButton* pRBss = nullptr;// suit selection control (above) used in loadSavecGame and main
    buttonValOnHit flareStrip;

    // fire works animation
    std::vector<spark> sparkVec;
    std::vector<spark>::iterator sparkIt;
    bool anyLive_emit = false;// fw assigns true
    spriteSheet dotSS;
    fireWork fw1, fw2, fw3;
    sf::Vector2f fwPos1, fwPos2, fwPos3;
    sf::Vector2f fireOfst;
    void updateFireworks( float dt );

    //*** funcs
    virtual bool init();
    spiderSolitaire(): card_abs() {}//{ init(); }

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~spiderSolitaire() { cleanup(); }

    bool undoLastMove();
    void startUpColAnimation();// game ending ani
    void startDownColAnimation();// saved game begin
    void loadSavedGame( const char* fileName );
    bool saveGame( const char* fileName );
    void renew_cardVec( bool go );
    void makeNewGame();
    bool loadFirstGame();
    bool loadFireWorks();
    void startCompleteCardMove( cardList* pSceList, float v0 );
    int findMoves();// for move hints
    void handleCardHit();
};

// utility
bool isValidMove( cardNode* pDestTailNode, cardNode& sceNode );

#endif // SPIDERSOLITAIRE_H_INCLUDED
