#ifndef CARD_H
#define CARD_H

#include <SFML/Graphics.hpp>
#include <iostream>

class card
{
    private:
//    bool faceUp = true;

    public:

    static float w;
    static float h;
    static float flare;
    static sf::Sprite backSpr;

 //   static card* pFlipCard;// 1 card may flip at once
    static bool anyFlipping;// 1 card may flip at once
 //   static sf::Vertex flipVtx[4];

 //   sf::Sprite spr;

    char suit;// 'H', 'D', 'S', 'C'
    int face;// 1 - 13 => Ace, 2, 3,..., 10, J, Q, K
    bool faceUp = true;

    // new dms
    int flipState = 0;// 0= not flipping, -1= flip to face down, +1= flip to face up
    sf::Vertex vtx[4];// replacing spr
    sf::Vector2f pos;
    float flipAngle = 0.0f;// varies 0 -> PI during a flip
    sf::Texture* pTxt = nullptr;// face texture
    void setPosition( float x, float y );
    void setPosition( sf::Vector2f Pos ) { setPosition( Pos.x, Pos.y ); }
    sf::Vector2f getPosition() const { return pos; }
    void setTexCoords();
    bool is_FaceUp() const { return flipState == 0 ? faceUp : flipState == 1; }

    // functions
    void draw( sf::RenderTarget& rRW )const;
    bool hit( float posX, float posY ) const;
    void initFlip( float dy );
    bool continueFlip();
    card( sf::Texture& r_Txt, char Suit, int Face );
    card();
    ~card();
};

class cardList;

struct cardNode
{
    card* pCard;
    cardNode *prev, *next;
    cardNode( card* p_card, cardNode* Prev, cardNode* Next ): pCard(p_card), prev(Prev), next(Next)
    {
        if(prev) prev->next = this;
        if(next) next->prev = this;
    }

    void extract() const { if(prev) prev->next = next; if(next) next->prev = prev; }
    void insert_after( cardNode& rPrev ) { prev = &rPrev; if( rPrev.next ){ next = rPrev.next; next->prev = this; } prev->next = this; }
    void insert_after( cardNode& rPrev, cardList& rCL );
    void insert_before( cardNode& rNext ) { next = &rNext; if( rNext.prev ){ prev = rNext.prev; prev->next = this; } next->prev = this; }
    void insert_before( cardNode& rNext, cardList& rCL );
};

class cardList
{
    public:
    static float dyFD_min;
    float maxLength = 500.0f;
    cardNode *head = nullptr, *tail = nullptr;
    card* pFlipCard = nullptr;
    float x0, y0, dx, dyFU, dyFU_full, dyFD, dyFD_full;
    bool isFullLength = true;
    bool isFanned = false;// drawList. true for the play lists

    cardNode* pNodeSel = nullptr;
  //  sf::Vertex* pVtxBox = nullptr;
    sf::Vertex  box[5];
    bool boxToEnd = false;

    void setPosition( float X, float Y );
    void setCardSpacing() { setPosition(x0,y0); }
    void setBasePosition( float X0, float Y0, float dX, float dYfu, float dYfd ) { x0 = X0; y0 = Y0; dx = dX; dyFU_full = dyFU = dYfu; dyFD_full = dyFD = dYfd; isFanned = true; setEmptyBox(); }
    void setBasePosition( float X0, float Y0 ) { x0 = X0; y0 = Y0; dx = dyFU_full = dyFU = dyFD_full = dyFD = 0.0f; isFanned = false; setEmptyBox(); }
    bool hit( cardNode*& pNodeHit, float x, float y );// can return true, nullptr if base area hit when list is empty
    bool endIsComplete( cardNode*& pNodeHit );// pNodeHit points to King in set
    bool endIsStraight( cardNode*& pNodeHit, int face_st );// pNodeHit points to start of straight

    bool checkLength();
    bool compress();// true if compression made
    bool expand();// true if compression made
    size_t numFaceup() const { size_t n = 0; for( cardNode* it = tail; it && it->pCard->faceUp; it = it->prev ) ++n; return n; }
    size_t numFacedown() const { size_t n = 0; for( cardNode* it = head; it && !it->pCard->faceUp; it = it->next ) ++n; return n; }
    size_t numCards() const { size_t n = 0; for( cardNode* it = head; it; it = it->next ) ++n; return n; }
 //   void push_back( card& rCard );// extend list by 1 node
    void push_back( cardNode& rcn );// no dynamic allocation
    void push_back( cardNode& rcn, sf::Vector2f& pos );// does not assign new position to card
    void append_toEnd( cardList* pList, cardNode* pNode );// append pNode through pList->tail
    void append_One( cardList* pList, cardNode* pNode );// append only pNode
    void append_One( cardList* pList, cardNode* pNode, sf::Vector2f& pos  );// write position but don't assign it
    void insert_after( cardNode* pPrev, cardList* pListSce, cardNode* pNodeSce, sf::Vector2f& pos  );// write position but don't assign it
    sf::Vector2f getNodePosition( cardNode& rNode ) const;
    void draw( sf::RenderTarget& RT )const;// { for( cardNode* it = head; it; it = it->next ) it->pCard->draw(RT); }
    ~cardList();

    // animation
    void flip_update();
    void setSelectedNode( cardNode* pNode, bool toEnd, sf::Color clr );
    void setEmptyBox();// cover base area when list is empty
};

#endif // CARD_H
