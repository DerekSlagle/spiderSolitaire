#ifndef CARD_ABS_H_INCLUDED
#define CARD_ABS_H_INCLUDED

#include <vector>
#include "../card.h"
#include "../leg_types/linLeg.h"
#include "Level.h"

class card_abs : public Level
{
    public:
    static sf::Color selectClr, destClr;
    std::vector<card> cardSetVec;// the 52 full card set
    std::vector<sf::Texture> textureVec;// the 52 card textures
    sf::Texture cardBackTxt;
    float cardColMaxLength = 500.0f;
    bool doSeedRand = true;
    // animation params
    bool animating = false;
    int animateID = 0;// 0=no ani, 1=dealAll, 2=dealSet, 3=moveSet, 4=moveComplete, 5=newGame pre, 6=saveGame pre, 7= saveGame begin
    float dealDelay = 3.0f;// using click strips on both
    float card_v0 = 1.0f, card_v1 = 40.0f, card_vf = 1.0f, card_xa = 0.3f, card_xb = 0.7f;// using click strips on both

    cardList moveList;
    size_t numLegs = 0;
    std::vector<linLeg> linLegVec;
    std::vector<LegAdapter> LegAdaptVec;
    std::function<void(void)> pEndAniFunc = nullptr;// when animation ceases
  //  sf::Vertex selectBox[5], destBox[5];
    sf::Vertex destBox[5];

    cardList *pListDest = nullptr, *pListSce = nullptr;
 //   cardNode *pNodeDest = nullptr, *pNodeSce = nullptr;
    cardNode *pNodeDest = nullptr;
    // card transfer mode
    bool dragDrop = false;
    sf::Vector2f grabOfst;

    // dealing
    cardList drawList;
    int dealID = 0, dealID_last = 0;// 0=none, 1=initial, 2=deal2, etc
    std::string deal1_fname, deal2_fname;
    std::function<void(void)> pEndDealFunc_1 = nullptr, pEndDealFunc_2 = nullptr;
    size_t xferCount = 0;// number to transfer from pListSce to pListDest


    // pure virtual funcs
    virtual bool init() = 0;
    card_abs();//: Level() {}

    virtual bool handleEvent( sf::Event& rEvent ) = 0;
    virtual void update( float dt ) = 0;// calls virtual logic()
    virtual void draw( sf::RenderTarget& RT ) const = 0;

  //  virtual void cleanup() = 0;
    virtual ~card_abs() { dealID_last = 0; button::pOKtoRun = nullptr; Level::goto_MMButt.setMO(false); }

    bool loadCards();
    void startLegAnimation( linLeg& rLeg, LegAdapter& rLA, card* pCard, sf::Vector2f begin_pos, sf::Vector2f end_pos, float v );
    void startSelectedCardMove( cardList* pSceList, cardList* pDestList, cardNode* pSceNode, bool doFlip = true );
    void moveOneCard_event( std::vector<cardList>& rCLvec );
    void moveOneCard_finish();
    void moveOneCard_update();
    void updateAnimations( std::vector<cardList>& rCLvec, float dt );
    void updateDeal( std::vector<cardList>& rCLvec, float dt );
    void startCardListMove( cardList& CL, sf::Vector2f disp, linLeg& rLeg, LegAdapter& LA, float v );// displacement from existing position
};

// utility
void makeBox( sf::Vertex* pBox, cardNode& rNodeTop, cardNode& rNodeBottom );

#endif // CARD_ABS_H_INCLUDED
