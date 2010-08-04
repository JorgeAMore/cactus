#ifndef CACTUS_LINK_PRIVATE_H_
#define CACTUS_LINK_PRIVATE_H_

#include "cactusGlobals.h"

struct _link {
    End *_3End;
    End *_5End;
    Chain *chain;
    Group *group;
    //previous link in the chain.
    Link *pLink;
    //next link in the chain.
    Link *nLink;
    //index of the link in the chain.
    int32_t linkIndex;
};

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
//Link functions.
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

/*
 * Destructs the link and all subsequent nLinks.
 */
void link_destruct(Link *link);

/*
 * Write a binary representation of the link to the write function.
 */
void link_writeBinaryRepresentation(Link *link, void(*writeFn)(
        const void * ptr, size_t size, size_t count));

/*
 * Loads a net into memory from a binary representation of the net.
 */
Link *link_loadFromBinaryRepresentation(void **binaryString, Chain *chain);

#endif
