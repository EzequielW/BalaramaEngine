#include "generator.h"

Generator::Generator(){
    genPawnMoves();
    genKnightMoves();
    genKingMoves();
    genPawnAttacks();
    genRayMoves();
    genRookMoves();
    genBishopMoves();
    genRookMoveboards();
    genBishopMoveboards();
    genRookXrays();
    genBishopXrays();
    std::cout << "Generator created" << std::endl;
}

int Generator::bitScanForward(uint64_t n){
    if(n == 0) return -1;

    int i = A1;
    while((n >> i) % 2 == 0){
        i++;
    }

    return i;
}

int Generator::bitScanReverse(uint64_t n){
    if(n == 0) return -1;

    int i = H8;
    while((n >> i) % 2 == 0){
        i--;
    }

    return i;
}

// Brian Kernighan algorithm to count the number of bits set on an integer.
int Generator::bitCountSet(uint64_t n){
    int counter = 0;

    while(n != 0){
        n &= (n - 1);
        counter++;
    }

    return counter;
}

void Generator::genPawnMoves(){
    // They are set with the predefined moves that are shifted for each square
    uint64_t whiteMoves;
    uint64_t blackMoves;

    for(int i = 0; i < 64; i++){
        whiteMoves = 256;
        blackMoves = 36028797018963968;
        if(i >= A2 && i <= H2){
            whiteMoves = 65792;
        }
        else if(i >= A7 && i <=H7){
            blackMoves = 36169534507319296;
        }

        pawnMoves[WHITE][i] = whiteMoves << i;
        pawnMoves[BLACK][i] = blackMoves >> (63 - i);
    }
}

void Generator::genKnightMoves(){
    // Moves to be shifted, on the borders moves change.
    uint64_t moves;

    for(int i = 0; i < 64; i++){
        if(isAFile((Square)i)){
            moves = 34628177928;
        }
        else if(isBFile((Square)i)){
            moves = 43218112522;
        }
        else if(isGFile((Square)i)){
            moves = 42966450442;
        }
        else if(isHFile((Square)i)){
            moves = 8606712066;
        }
        else{
            moves = 43234889994;
        }

        // All predefined moves are shifted from C3
        if(i >= C3){
            knightMoves[i] = moves << (i - C3);
        }
        else{
            knightMoves[i] = moves >> (C3 - i);
        }
    }
}

void Generator::genKingMoves(){
    uint64_t moves;

    for(int i = 0; i < 64; i++){
        if(isAFile((Square)i)){
            moves = 394246;
        }
        else if(isHFile((Square)i)){
            moves = 196867;
        }
        else{
            moves = 460039;
        }

        // All predefined moves are shifted from B2
        if(i >= B2){
            kingMoves[i] = moves << (i - B2);
        }
        else{
            kingMoves[i] = moves >> (B2 - i);
        }
    }
}

void Generator::genPawnAttacks(){
    uint64_t whiteMoves;
    uint64_t blackMoves;

    for(int i = 0; i < 64; i++){
        if(isAFile((Square)i)){
            whiteMoves = 262144;
            blackMoves = 4;
        }
        else if(isHFile((Square)i)){
            whiteMoves = 65536;
            blackMoves = 1;
        }
        else{
            whiteMoves = 327680;
            blackMoves = 5;
        }

        // All predefined moves are shifted from B2
        if(i >= B2){
            pawnAttacks[WHITE][i] = whiteMoves << (i - B2);
            pawnAttacks[BLACK][i] = blackMoves << (i - B2);
        }
        else{
            pawnAttacks[WHITE][i] = whiteMoves >> (B2 - i);
            pawnAttacks[BLACK][i] = blackMoves >> (B2 - i);
        }
    }
}

void Generator::genRayMoves(){
    for(int i = 0; i < 64; i++){
        int sqFile = i % 8;
        int sqRank = i / 8;

        rayMoves[i][NORTH] = 72340172838076672 << i;
        rayMoves[i][SOUTH] = 36170086419038336 >> (63 - i);

        // For the sides we do 2^file(or 7 - file for east) and subtract 1 so all the bits before the power are set,
        // then we shift square with and offset.
        rayMoves[i][EAST] = (1 << (7 - sqFile)) - 1;
        rayMoves[i][EAST] <<= (i + 1);
        rayMoves[i][WEST] = (1 << sqFile) - 1;
        rayMoves[i][WEST] <<= (i - sqFile);

        // Has all 64 bit set. Is used to clear the extra bits after shifting diagonals.
        uint64_t mask = 18446744073709551615U;
        int maskShift;

        uint64_t neRay = 9241421688590303744U << i;
        maskShift = 8*(sqFile - sqRank);
        if(maskShift < 0) { maskShift = 0; }
        rayMoves[i][NORTH_EAST] = (mask >> maskShift) & neRay;

        uint64_t nwRay = 567382630219904 << i;
        maskShift = 8*(7 - sqFile - sqRank) + 1;
        if(maskShift < 0) { maskShift = 0; }
        rayMoves[i][NORTH_WEST] = (mask >> maskShift) & nwRay;

        uint64_t seRay = 72624976668147712 >> (63 - i);
        maskShift = 8*(-7 + sqFile + sqRank) + 1;
        if(maskShift < 0) { maskShift = 0;}
        rayMoves[i][SOUTH_EAST] = (mask << maskShift) & seRay;

        uint64_t swRay = 18049651735527937 >> (63 - i);
        maskShift = 8*(-sqFile + sqRank);
        if(maskShift < 0) { maskShift = 0; }
        rayMoves[i][SOUTH_WEST] = (mask << maskShift) & swRay;
    }
}

void Generator::genRookMoves(){
    // We remove the borders at the end to reduce unnecessary mappings later, but we must do it individually
    // for each side.
    for(int i = 0; i < 64; i++){
        uint64_t northMoves = 72057594037927935U & rayMoves[i][NORTH];
        uint64_t southMoves = 18446744073709551360U & rayMoves[i][SOUTH];
        uint64_t westMoves = 18374403900871474942U & rayMoves[i][WEST];
        uint64_t eastMoves = 9187201950435737471U & rayMoves[i][EAST];

        rookMoves[i] = northMoves | southMoves | westMoves | eastMoves;
    }
}

void Generator::genBishopMoves(){
    // We remove the borders at the end to reduce unnecessary mappings later.
    for(int i = 0; i < 64; i++){
        bishopMoves[i] = (rayMoves[i][NORTH_WEST] | rayMoves[i][NORTH_EAST] |
                            rayMoves[i][SOUTH_WEST] | rayMoves[i][SOUTH_EAST]) & 35604928818740736;
    }
}

// Generates a unique blockerboard with an index and either a rook or bishop moveboard(without blockers).
// The number of indexes is determined by the number of bits set in the moveboard, so 2^bits blockerboards.
uint64_t Generator::genBlockerBoard(int index, uint64_t rayMask){
    int bitindex = 0;
    uint64_t j = 1;

    for(int i = 0; i < 64; i++){
        if((rayMask & (j << i)) != 0){
            if((index & (j << bitindex)) == 0){
                rayMask &= ~(j << i);
            }
            bitindex += 1;
        }
    }

    return rayMask;
}

// Create the mapping for the final rook moveboards
void Generator::genRookMoveboards(){
    for(int i = 0; i < 64; i++){
        int bitLength = bitCountSet(rookMoves[i]);

        // The number of blockers combinations is 2 to the power of the bit count.
        // Once with have the blockers board we check for the first blocker in each direction and remove the bits afterwards.
        for(int j = 0; j < (1 << bitLength); j++){
            uint64_t blockersBoard = genBlockerBoard(j, rookMoves[i]);
            uint64_t moveboard = rayMoves[i][NORTH] | rayMoves[i][SOUTH] | rayMoves[i][WEST] | rayMoves[i][EAST];
            uint64_t northBlocker, southBlocker, westBlocker, eastBlocker;

            if((northBlocker = (rayMoves[i][NORTH] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanForward(northBlocker)][NORTH];
            }
            if((southBlocker = (rayMoves[i][SOUTH] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanReverse(southBlocker)][SOUTH];
            }
            if((westBlocker = (rayMoves[i][WEST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanReverse(westBlocker)][WEST];
            }
            if((eastBlocker = (rayMoves[i][EAST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanForward(eastBlocker)][EAST];
            }

            rookMoveboard[i][blockersBoard] = moveboard;
        }
    }
}

void Generator::genBishopMoveboards(){
    for(int i = 0; i < 64; i++){
        int bitLength = bitCountSet(bishopMoves[i]);

        // The number of blockers combinations is 2 to the power of the bit count.
        // Once with have the blockers board we check for the first blocker in each direction and remove the bits afterwards.
        for(int j = 0; j < (1 << bitLength); j++){
            uint64_t blockersBoard = genBlockerBoard(j, bishopMoves[i]);
            uint64_t moveboard = rayMoves[i][NORTH_WEST] | rayMoves[i][NORTH_EAST] | rayMoves[i][SOUTH_WEST] | rayMoves[i][SOUTH_EAST];
            uint64_t nwestBlocker, neastBlocker, swestBlocker, seastBlocker;

            if((nwestBlocker = (rayMoves[i][NORTH_WEST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanForward(nwestBlocker)][NORTH_WEST];
            }
            if((neastBlocker = (rayMoves[i][NORTH_EAST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanForward(neastBlocker)][NORTH_EAST];
            }
            if((swestBlocker = (rayMoves[i][SOUTH_WEST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanReverse(swestBlocker)][SOUTH_WEST];
            }
            if((seastBlocker = (rayMoves[i][SOUTH_EAST] & blockersBoard)) > 0){
                moveboard ^= rayMoves[bitScanReverse(seastBlocker)][SOUTH_EAST];
            }

            bishopMoveboard[i][blockersBoard] = moveboard;
        }
    }
}

void Generator::genRookXrays(){
    for(int i = 0; i < 64; i++){
        int bitLength = bitCountSet(rookMoves[i]);

        for(int j = 0; j < (1 << bitLength); j++){
            // AND to obtain only the first blockers in each direction
            uint64_t blockersBoard = genBlockerBoard(j, rookMoves[i]);
            uint64_t firstBlockers = blockersBoard & rookMoveboard[i][blockersBoard];
            uint64_t xrayMoves = 0;

            while(firstBlockers > 0){
                int blocker = bitScanForward(firstBlockers);
                // Create a new blockers board with only the blockers of one direction
                uint64_t newBlockers = blockersBoard & rookMoves[blocker];
                // Create xray to the next blocker
                uint64_t newRay = rookMoveboard[i][0] & (~rookMoveboard[i][blockersBoard]);
                newRay &= rookMoveboard[blocker][newBlockers];
                // We add it to the other rays
                xrayMoves |= newRay;
                // After we get the ray we clear the bit (the 1 most be uint64)
                uint64_t k = 1;
                firstBlockers ^= (k << blocker);
            }
            rookXrays[i][blockersBoard] = xrayMoves;
        }
    }
}

void Generator::genBishopXrays(){
    for(int i = 0; i < 64; i++){
        int bitLength = bitCountSet(bishopMoves[i]);

        for(int j = 0; j < (1 << bitLength); j++){
            // AND to obtain only the first blockers in each direction
            uint64_t blockersBoard = genBlockerBoard(j, bishopMoves[i]);
            uint64_t firstBlockers = blockersBoard & bishopMoveboard[i][blockersBoard];
            uint64_t xrayMoves = 0;

            while(firstBlockers > 0){
                int blocker = bitScanForward(firstBlockers);
                // Create a new blockers board with only the blockers of one direction
                uint64_t newBlockers = blockersBoard & bishopMoves[blocker];
                // Create xray to the next blocker
                uint64_t newRay = bishopMoveboard[i][0] & (~bishopMoveboard[i][blockersBoard]);
                newRay &= bishopMoveboard[blocker][newBlockers];
                // We add it to the other rays
                xrayMoves |= newRay;
                // After we get the ray we clear the bit (the 1 most be uint64)
                uint64_t k = 1;
                firstBlockers ^= (k << blocker);
            }
            bishopXrays[i][blockersBoard] = xrayMoves;
        }
    }
}
