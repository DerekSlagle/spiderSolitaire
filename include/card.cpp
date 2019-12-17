#include "card.h"

float card::w = 0.0f;
float card::h = 0.0f;
float card::flare = 5.0f;
sf::Sprite card::backSpr;

//card* card::pFlipCard = nullptr;// 1 card may flip at once
bool card::anyFlipping = false;
//sf::Vertex card::flipVtx[4];

/*
card::card( sf::Texture& r_Txt, char Suit, int Face )
{
    spr.setTexture( r_Txt );
    spr.setPosition(50.0f, 50.0f);
    suit = Suit;
    face = Face;
}   */

card::card( sf::Texture& r_Txt, char Suit, int Face )
{
 //   spr.setTexture( r_Txt );
 //   spr.setPosition(50.0f, 50.0f);

    pTxt = &r_Txt;
    setPosition( 50.0f, 50.0f );
    setTexCoords();
    suit = Suit;
    face = Face;
}

card::card()
{
    suit='S';
    face = 1;
}

card::~card()
{
    //dtor
}

void card::setPosition( float x, float y )
{
    pos.x = x; pos.y = y;
    if( flipState == 0 )
    {
    vtx[0].position =  pos;// up lt
    vtx[1].position = vtx[3].position = vtx[0].position;
    vtx[1].position.x += card::w;// up rt
    vtx[2].position = vtx[1].position; vtx[2].position.y += card::h;// dn rt
    vtx[3].position.y += card::h;// dn lt
    }
    else
    {
        sf::Vector2f cpos = pos;
        cpos.x += card::w/2.0f; cpos.y += card::h/2.0f;// to center
        const float flare = card::h/10.0f;// = 1/5 up + down
        float ca = cosf(flipAngle), sa = sinf(flipAngle);
        vtx[0].position = vtx[1].position = vtx[2].position = vtx[3].position = cpos;
        vtx[0].position.x -= card::w*ca/2.0f; vtx[0].position.y -= card::h/2.0f + flare*sa;// up lt
        vtx[1].position.x += card::w*ca/2.0f; vtx[1].position.y -= card::h/2.0f - flare*sa;// up rt
        vtx[2].position.x += card::w*ca/2.0f; vtx[2].position.y += card::h/2.0f - flare*sa;// dn rt
        vtx[3].position.x -= card::w*ca/2.0f; vtx[3].position.y += card::h/2.0f + flare*sa;// dn lt
    }
}

void card::setTexCoords()
{
 //   sf::IntRect txtRect = faceUp ? spr.getTextureRect() : backSpr.getTextureRect();
    sf::IntRect txtRect = faceUp ? sf::IntRect( 0, 0, pTxt->getSize().x, pTxt->getSize().y ) : backSpr.getTextureRect();
    vtx[0].texCoords.x = (float)txtRect.left;  vtx[0].texCoords.y = (float)txtRect.top;// up lt
    vtx[1].texCoords.x = (float)(txtRect.left+txtRect.width); vtx[1].texCoords.y = (float)txtRect.top;// up rt
    vtx[2].texCoords.x = (float)(txtRect.left+txtRect.width); vtx[2].texCoords.y = (float)(txtRect.top+txtRect.height);// dn rt
    vtx[3].texCoords.x = (float)txtRect.left;  vtx[3].texCoords.y = (float)(txtRect.top+txtRect.height);// dn lt
}

bool card::hit( float posX, float posY ) const
{
    if( posX < pos.x ) return false;
    if( posX > pos.x + card::w ) return false;
    if( posY < pos.y ) return false;
    if( posY > pos.y + card::h ) return false;
    return true;
}

void card::draw( sf::RenderTarget& rRW )const
{
    rRW.draw( vtx, 4, sf::Quads, faceUp ? pTxt : backSpr.getTexture() );

 /*   if( pFlipCard == this )
    {
        rRW.draw( card::flipVtx, 4, sf::Quads, faceUp ? spr.getTexture() : backSpr.getTexture() );
    }
    else if( faceUp ) rRW.draw( spr );
    else
    {
        card::backSpr.setPosition( spr.getPosition() );
        rRW.draw( backSpr );
    }   */
    return;
}

void card::initFlip( float dyFU )
{
    sf::Vector2f new_pos = pos;
    new_pos.y += dyFU;
    setPosition( new_pos );
    setTexCoords();
    flipAngle = 0.0f;
    flipState = faceUp ? -1 : 1;
    card::anyFlipping = true;
 //   flipVtx[0].position = spr.getPosition();// up lt
/*    flipVtx[0].position =  pos;// up lt
    flipVtx[1].position = flipVtx[3].position = flipVtx[0].position;
    flipVtx[1].position.x += card::w;// up rt
    flipVtx[2].position = flipVtx[1].position; flipVtx[2].position.y += card::h;// dn rt
    flipVtx[3].position.y += card::h;// dn lt
    sf::IntRect txtRect = faceUp ? spr.getTextureRect() : backSpr.getTextureRect();
 //   faceUp = !faceUp;
    flipVtx[0].texCoords.x = (float)txtRect.left;  flipVtx[0].texCoords.y = (float)txtRect.top;// up lt
    flipVtx[1].texCoords.x = (float)(txtRect.left+txtRect.width); flipVtx[1].texCoords.y = (float)txtRect.top;// up rt
    flipVtx[2].texCoords.x = (float)(txtRect.left+txtRect.width); flipVtx[2].texCoords.y = (float)(txtRect.top+txtRect.height);// dn rt
    flipVtx[3].texCoords.x = (float)txtRect.left;  flipVtx[3].texCoords.y = (float)(txtRect.top+txtRect.height);// dn lt    */
}

bool card::continueFlip()
{
    if( flipState == 0 ) return false;
 //   static float ang = 0.0f;
    const float angLimit = 3.1415927f;
    const float dAng = angLimit/21.0f;
    flipAngle += dAng;
    if( flipAngle >= angLimit )
    {
     //   pFlipCard = nullptr;
        flipState = 0;
        flipAngle = 0.0f;
        setPosition( pos );
        setTexCoords();
        return false;
    }

    sf::Vector2f cpos = pos;
    cpos.x += card::w/2.0f; cpos.y += card::h/2.0f;// to center
 //   const float flare = card::h/10.0f;// = 1/5 up + down
    float ca = cosf(flipAngle), sa = sinf(flipAngle);
    vtx[0].position = vtx[1].position = vtx[2].position = vtx[3].position = cpos;
    vtx[0].position.x -= card::w*ca/2.0f; vtx[0].position.y -= card::h/2.0f + flare*sa;// up lt
    vtx[1].position.x += card::w*ca/2.0f; vtx[1].position.y -= card::h/2.0f - flare*sa;// up rt
    vtx[2].position.x += card::w*ca/2.0f; vtx[2].position.y += card::h/2.0f - flare*sa;// dn rt
    vtx[3].position.x -= card::w*ca/2.0f; vtx[3].position.y += card::h/2.0f + flare*sa;// dn lt

    if( 2.0f*flipAngle >= angLimit && 2.0f*(flipAngle-dAng) < angLimit )// flip card
    {
        faceUp = !faceUp;
        sf::IntRect txtRect = faceUp ? sf::IntRect( 0, 0, pTxt->getSize().x, pTxt->getSize().y ) : backSpr.getTextureRect();
        // 1 0 2 3 due to flip about vertical axis
        vtx[1].texCoords.x = (float)txtRect.left;  vtx[0].texCoords.y = (float)txtRect.top;// up lt
        vtx[0].texCoords.x = (float)(txtRect.left+txtRect.width); vtx[1].texCoords.y = (float)txtRect.top;// up rt
        vtx[3].texCoords.x = (float)(txtRect.left+txtRect.width); vtx[2].texCoords.y = (float)(txtRect.top+txtRect.height);// dn rt
        vtx[2].texCoords.x = (float)txtRect.left;  vtx[3].texCoords.y = (float)(txtRect.top+txtRect.height);// dn lt
    }

    return true;
}

// cardNode

void cardNode::insert_after( cardNode& rPrev, cardList& rCL )
{
    if( !rPrev.next ) rCL.tail = this;
    insert_after( rPrev );
}

void cardNode::insert_before( cardNode& rNext, cardList& rCL )
{
    if( !rNext.prev ) rCL.head = this;
    insert_before( rNext );
}

// cardList

float cardList::dyFD_min = 5.0f;

void cardList::push_back( cardNode& rcn )
{
    sf::Vector2f pos(x0,y0);
    push_back( rcn, pos );
    rcn.pCard->setPosition( pos );
}

void cardList::push_back( cardNode& rcn, sf::Vector2f& pos )// does not assign new position to card
{
    pos.x = x0; pos.y = y0;
    rcn.extract();// extract node from existing list
    rcn.prev = tail;// link to new list
    rcn.next = nullptr;

    if( tail ) { pos = tail->pCard->getPosition(); pos.x += dx; pos.y += tail->pCard->faceUp ? dyFU : dyFD; }

    if( !tail ) head = tail = &rcn;// 1st node in list
    else
    {
        tail->next = &rcn;// now next becomes != nullptr
        tail = tail->next;
    }

    return;
}

void cardList::setPosition( float X, float Y )
{
    x0 = X; y0 = Y;
    sf::Vector2f pos( x0, y0 );
    if( !isFanned )
    {
        if( tail ) tail->pCard->setPosition( pos );
        return;
    }

    if( !tail )
    {
        setEmptyBox();
        return;
    }

    const cardNode* it = head;
    while( it )
    {
        it->pCard->setPosition( pos );
        pos.x += dx;
        pos.y += it->pCard->is_FaceUp() ? dyFU : dyFD;
        it = it->next;
    }
}

void cardList::draw( sf::RenderTarget& RT )const
{
    if( isFanned ) for( cardNode* it = head; it; it = it->next ) it->pCard->draw(RT);
    else if( tail ) tail->pCard->draw(RT);
    // highlight box for selected nodes
    if( pNodeSel || !tail ) RT.draw( box, 4, sf::Quads );
}

bool cardList::hit( cardNode*& pNodeHit, float x, float y )
{
    if( tail )
    {
        if( isFanned )
        {
            for( cardNode* it = tail; it; it = it->prev )
                if( it->pCard->hit( x, y ) ) { pNodeHit = it; return true; }
        }
        else if( tail->pCard->hit( x, y ) ) { pNodeHit = tail; return true; }

    }
    else if( x >= x0 && x <= x0+card::w && y >= y0 && y <= y0+card::h )
    {
        pNodeHit = nullptr;
        return true;
    }

    pNodeHit = nullptr;
    return false;
}

bool cardList::endIsComplete( cardNode*& pNodeHit )// pNodeHit points to King in set
{
    if( !tail ) return false;
    if( tail->pCard->face != 1 ) return false;
    char matchSuit = tail->pCard->suit;
    int faceVal = 1;
    cardNode* it = nullptr;
    for( it = tail->prev; it && faceVal < 13; it = it->prev )
    {
        if( !it->pCard->is_FaceUp() ) return false;
        if( it->pCard->suit != matchSuit || it->pCard->face != ++faceVal ) return false;
        if( faceVal == 13 ) break;// avoid last it increment
    }

    if( it ) { pNodeHit = it; return true; }
    return false;
}

bool cardList::endIsStraight( cardNode*& pNodeHit, int face_st )// pNodeHit points to start of straight
{
    if( !tail || (tail->pCard->face != face_st + 1) ) { pNodeHit = nullptr; return false; }// no match at tail
    pNodeHit = tail;
    while( pNodeHit->prev && pNodeHit->prev->pCard->faceUp && pNodeHit->pCard->face + 1 == pNodeHit->prev->pCard->face )
        pNodeHit = pNodeHit->prev;
    return true;
}

bool cardList::checkLength()
{
    if( !isFanned ) return false;// no length change

    if( !head )
    {
        isFullLength = true;// empty list is full length
        dyFU = dyFU_full;
        dyFD = dyFD_full;
        return false;
    }

    sf::Vector2f headPos = head->pCard->getPosition();
    sf::Vector2f tailPos = tail->pCard->getPosition();
    float curr_len = tailPos.y - headPos.y;

    if( curr_len + card::h < maxLength )// list fits in space
    {
        if( isFullLength ) return false;
        return expand();
    }

    if( curr_len + card::h > maxLength )
        return compress();

    return false;
}

bool cardList::expand()
{
    if( !head || isFullLength || !isFanned ) return false;

    sf::Vector2f headPos = head->pCard->getPosition();
    sf::Vector2f tailPos = tail->pCard->getPosition();
    float curr_len = tailPos.y - headPos.y;

    if( curr_len + card::h >= maxLength ) return false;// list fits in space

    float under = maxLength - ( curr_len + card::h );// sb > 0
    size_t numFD = numFacedown(), numFU = numFaceup();

    // expand FU 1st
    if( under > 0.0f && numFU > 1 )// can expand
    {
        dyFU += under/( numFU - 1 );// take all extra to FU
        under = 0.0f;
        if( dyFU > dyFU_full )// too far
        {
            under = ( dyFU - dyFU_full )*( numFU - 1 );// put some back
            dyFU = dyFU_full;// limit
        }

    }

    // expand FD?
    if( under > numFD*( dyFD_full - dyFD) )// can expand
    {
        dyFD = dyFD_full;
        isFullLength = true;
    }

    // position all the cards
    sf::Vector2f pos( x0, y0 );
    const cardNode* it = head;
    while( it )
    {
        it->pCard->setPosition( pos );
        pos.x += dx;
        pos.y += it->pCard->faceUp ? dyFU : dyFD;
        it = it->next;
    }

    return true;
}

bool cardList::compress()// true if compression made
{
    if( !( head && isFanned ) ) return false;

    sf::Vector2f headPos = head->pCard->getPosition();
    sf::Vector2f tailPos = tail->pCard->getPosition();
    float curr_len = tailPos.y - headPos.y;

    if( curr_len + card::h <= maxLength ) return false;// list fits in space

    float over = curr_len + card::h - maxLength;
    size_t numFD = numFacedown(), numFU = numFaceup();

    // FD spacing only 2 values dyFD, dyFD_min
    if( isFullLength && numFD > 0 )// can compress FD cards
    {
        over -= numFD*( dyFD - dyFD_min );
        dyFD = dyFD_min;
    }

    isFullLength = false;

    if( over > 0.0f && numFU > 1 )// can compress
        dyFU -= over/( numFU - 1 );

    // position all the cards
    setPosition(x0,y0);

    return true;
}

void cardList::append_toEnd( cardList* pList, cardNode* pNode )// append List to this, starting at pNode
{
    if( !(pList&&pNode) ) return;
    if( pList == this ) return;

    // untie from old list
    cardNode* pList_oldTail = pList->tail;// save
    if( pNode->prev ){  pList->tail = pNode->prev; pList->tail->next = nullptr; }// pNode not pList->head
    else
    {
        pList->head = pList->tail = nullptr;
        pList->isFullLength = true;
        pList->dyFU = pList->dyFU_full;
        pList->dyFD = pList->dyFD_full;
        pList->setEmptyBox();
    }
    // tie into new list
    pNode->prev = tail;
    sf::Vector2f pos(x0,y0);// if empty
    if( tail ) { tail->next = pNode; pos = tail->pCard->getPosition(); pos.x += dx; pos.y += tail->pCard->is_FaceUp() ? dyFU : dyFD; }
    else head = pNode;

    for( cardNode* it = pNode; it; it = it->next )
    {
        it->pCard->setPosition( pos );
        if( isFanned ) { pos.x += dx; pos.y += it->pCard->is_FaceUp() ? dyFU : dyFD; }
    }

    tail = pList_oldTail;
}

void cardList::append_One( cardList* pList, cardNode* pNode )
{
    sf::Vector2f pos(x0,y0);
    append_One( pList, pNode, pos  );
    pNode->pCard->setPosition( pos );
}

void cardList::append_One( cardList* pList, cardNode* pNode, sf::Vector2f& pos  )// write position but don't assign it
{
    if( !(pList&&pNode) ) return;
    if( pList == this ) return;

    // extract from existing list
    if( pNode->prev ) pNode->prev->next = pNode->next;
    else { pList->head = pNode->next; }

    if( pNode->next ) pNode->next->prev = pNode->prev;
    else pList->tail = pNode->prev;

    if( !pList->head )
    {
        pList->isFullLength = true;
        pList->dyFU = pList->dyFU_full;
        pList->dyFD = pList->dyFD_full;
        pList->setEmptyBox();
    }

    // link into this list
    if( tail )
    {
        tail->next = pNode;
        pNode->prev = tail;
        tail = tail->next;
        tail->next = nullptr;
    }
    else
    {
        head = tail = pNode;// one node list
        pNode->prev = pNode->next = nullptr;
    }

    pos = getNodePosition( *pNode );
}

void cardList::insert_after( cardNode* pPrev, cardList* pList, cardNode* pNode, sf::Vector2f& pos  )// write position but don't assign it
{
    if( !(pList&&pNode&&pPrev) ) return;
    if( pPrev == pNode ) return;// dest and sce nodes same
    if( pPrev->next == pNode ) return;// already in this order
 //   if( pList == this ) return;

    // extract from existing list
    if( pNode->prev ) pNode->prev->next = pNode->next;
    else { pList->head = pNode->next; }

    if( pNode->next ) pNode->next->prev = pNode->prev;
    else pList->tail = pNode->prev;

    if( !pList->head )
    {
        pList->isFullLength = true;
        pList->dyFU = pList->dyFU_full;
        pList->dyFD = pList->dyFD_full;
        pList->setEmptyBox();
    }

    // insert in this list
    if( pPrev == tail )
    {
        tail->next = pNode;
        pNode->prev = tail;
        tail = tail->next;
        tail->next = nullptr;
    }
    else// link across
    {
        pNode->prev = pPrev;
        pNode->next = pPrev->next;
        pNode->prev->next = pNode;
        pNode->next->prev = pNode;
    }

    pos = pPrev->pCard->pos;
    if( isFanned ) { pos.x += dx; pos.y += pPrev->pCard->is_FaceUp() ? dyFU : dyFD; }
}

sf::Vector2f cardList::getNodePosition( cardNode& rNode ) const
{
    sf::Vector2f pos( x0, y0 );
    if( !isFanned ) return pos;
    const cardNode* it = head;
    while( it && it != &rNode )
    {
        pos.x += dx;
        pos.y += it->pCard->is_FaceUp() ? dyFU : dyFD;
        it = it->next;
    }
    return pos;
}

cardList::~cardList() { std::cout << "\n~cardList: " << this; }

void cardList::flip_update()
{
    for( cardNode* it = head; it; it = it->next )
        if( it->pCard->flipState != 0 )
        {
            if( it->pCard->continueFlip() )  card::anyFlipping = true;
            else setCardSpacing();
        }
}

void cardList::setSelectedNode( cardNode* pNode, bool toEnd, sf::Color clr )
{
    pNodeSel = pNode;

    boxToEnd = toEnd;
    cardNode* pNodeEnd = toEnd ? tail : pNode;
    float H = (toEnd || pNode == tail) ? card::h : ( pNode->pCard->is_FaceUp() ? dyFU : dyFD );

    sf::Vector2f posTopLt = pNode->pCard->getPosition();
    sf::Vector2f posBottRt = pNodeEnd->pCard->getPosition() + sf::Vector2f( card::w, H );
    box[0].position = posTopLt;
    box[1].position.x = posBottRt.x;
    box[1].position.y = posTopLt.y;
    box[2].position = posBottRt;
    box[3].position.x = posTopLt.x;
    box[3].position.y = posBottRt.y;
    box[4].position = box[0].position;
    box[0].color = box[1].color = box[2].color = box[3].color = box[4].color = clr;
}

void cardList::setEmptyBox()
{
    sf::Vector2f pos(x0,y0);
    box[0].position = box[1].position = box[2].position = box[3].position = box[4].position = pos;
    box[1].position.x += card::w;
    box[2].position.x += card::w;
    box[2].position.y += card::h;
    box[3].position.y += card::h;
    box[0].color = box[1].color = box[2].color = box[3].color = box[4].color = sf::Color(255,255,255,50);
}
