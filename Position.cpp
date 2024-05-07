#include<intrin.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <intrin.h>
#include <bitset>

#include "Position.h"
#include "Globals.h"
#include "Board.h"
#include "AttackTables.h"
#include "MoveGen.h"



bool isSquareAttacked(int source, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t blockers = 0;
    uint64_t sourceBit = (uint64_t)1 << source;
    //bishop rays
    uint64_t SWmoves = bishopAttacks[source][3];
    uint64_t SEmoves = bishopAttacks[source][2];
    uint64_t NWmoves = bishopAttacks[source][1];
    uint64_t NEmoves = bishopAttacks[source][0];
    unsigned long lsb = 0; uint64_t ePawn; uint64_t eKnight; uint64_t eBishop; uint64_t eRook; uint64_t eQueen; uint64_t eKing;
    if (color == 1) {
        ePawn = blackBoards[0]; eKnight = blackBoards[1]; eBishop = blackBoards[2]; eRook = blackBoards[3]; eQueen = blackBoards[4]; eKing = blackBoards[5];
    }
    else {
        ePawn = whiteBoards[0]; eKnight = whiteBoards[1]; eBishop = whiteBoards[2]; eRook = whiteBoards[3]; eQueen = whiteBoards[4]; eKing = whiteBoards[5];
    }
    eBishop |= eQueen;
    eRook |= eQueen;
    blockers = SWmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanForward64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eBishop) > 0)
        {
            return true;
        }
    }
    blockers = SEmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanForward64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eBishop) > 0)
        {
            return true;
        }
    }
    blockers = NWmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanReverse64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eBishop) > 0)
        {
            return true;
        }
    }
    blockers = NEmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanReverse64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eBishop) > 0)
        {
            return true;
        }
    }

    //rook rays
    uint64_t Smoves = rookAttacks[source][3];
    uint64_t Emoves = rookAttacks[source][2];
    uint64_t Wmoves = rookAttacks[source][1];
    uint64_t Nmoves = rookAttacks[source][0];
    blockers = Smoves & miscBoards[0];
    if (blockers)
    {
        _BitScanForward64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eRook) > 0)
        {
            return true;
        }
    }
    blockers = Emoves & miscBoards[0];
    if (blockers)
    {
        _BitScanForward64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eRook) > 0)
        {
            return true;
        }
    }
    blockers = Wmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanReverse64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eRook) > 0)
        {
            return true;
        }
    }
    blockers = Nmoves & miscBoards[0];
    if (blockers)
    {
        _BitScanReverse64(&lsb, blockers);
        uint64_t enemy = (uint64_t)1 << lsb; //first blocker in ray
        if ((enemy & eRook) > 0)
        {
            return true;
        }
    }
    //knight jumps
    uint64_t knightMoves = knightAttacks[source];
    if ((knightMoves & eKnight) > 0)
    {
        return true;
    }
    //pawn attacks

    if (color == 1) //black pawn attacks for white king
    {
        if (((sourceBit >> 7) & ePawn) && (sourceBit & notHFile) > 0) {
            return true;
        }
        if (((sourceBit >> 9) & ePawn) && (sourceBit & notAFile) > 0) {
            return true;
        }
    }
    else if (color == 0) //white pawn attacks for black king
    {
        if (((sourceBit << 7) & ePawn) && (sourceBit & notAFile) > 0) {
            return true;
        }
        if (((sourceBit << 9) & ePawn) && (sourceBit & notHFile) > 0) {
            return true;
        }
    }

    //king attacks
    _BitScanReverse64(&lsb, eKing);
    if ((kingAttacks[lsb] & sourceBit) > (uint64_t)0)
    {
        return true;
    }
    return false;
}

int parseCap(int source, int dir, int pieceType) { //source square, ray direction I need to check N = 0, NE = 1, E = 2, SE = 3, S = 4, SW = 5, W = 6, NW = 7, piece idx in pieceValues array defined below
    return source | (dir << 6) | (pieceType << 12); //square and ray dir only matter for pieces with possible xray attacks(not kings and queens)
}
int getCapSource(int move) {
    return move & 0x3f;
}
int getCapDir(int move) {
    return (move & 0xfc0) >> 6;
}
int getCapPieceType(int move) {
    return (move & 0xf000) >> 12;
}
int wAttackers[16];
int bAttackers[16];
int wTotal; int bTotal;
int pieceValuesSEE[12] = {1,3,3,5,9,99, 1,3,3,5,9,99 };
int scores[32];
int SEE(int source, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], int move, int color, int enP) { //source is really more like target....
    wTotal = 0;
    int wIndex = 0;
    bTotal = 0;
    int bIndex = 0;
    int scoresIndex = 0;
    int attackedPieceValue = pieceValuesSEE[board[getMoveSource(move)]];
    int score;
    if (enP) {
        score = 1;
    }
    else {
        score = pieceValuesSEE[board[source]];
    }
    int mvsource = getMoveSource(move);
    int mvdest = getMoveTarget(move);
    int mvpiece = getMovePiece(move);
    int capturedPiece = 12; //empty space
    unsigned long kingBit;
    /*
    for (int dir = 0; dir < 8; dir++) {
        if (((one << mvsource) & allRays[source][dir]) > 0) {
            xrayAttack(mvsource, dir, whiteBoards, blackBoards, miscBoards);
            //std::cout << "Dir: " << dir << std::endl;
            break;
        }
    }*/
    //std::cout << "Score: " << score << std::endl;
    int tempColor = color ^ 1;
    makeCapture(move, board, whiteBoards, blackBoards, miscBoards, capturedPiece, mvsource, mvdest, mvpiece, color, kingBit);
    int lowestValueIndex;
    int lowestValue; 
    int temp; 
    scores[0] = score;
    scoresIndex = 1;
    uint64_t pieceB = one << source;
    //pawn attacks
    if (((pieceB >> 7) & blackBoards[0]) > 0 && (pieceB & notHFile) > 0) {
        bAttackers[bTotal] = parseCap(source - 7, 1, p); //NE
        bTotal++;
    }
    if (((pieceB >> 9) & blackBoards[0]) > 0 && (pieceB & notAFile) > 0) {
        bAttackers[bTotal] = parseCap(source - 9, 7, p); //NW
        bTotal++;
    }
    if (((pieceB << 7) & whiteBoards[0]) > 0 && (pieceB & notAFile) > 0) {
        wAttackers[wTotal] = parseCap(source + 7, 5, P); //SW
        wTotal++;
    }
    if (((pieceB << 9) & whiteBoards[0]) > 0 && (pieceB & notHFile) > 0) {
        wAttackers[wTotal] = parseCap(source + 9, 3, P); //SE
        wTotal++;
    }
    //std::cout << bTotal << std::endl;
    //knight attacks
    uint64_t knightMoves = knightAttacks[source];
    std::bitset<64> b(knightMoves & blackBoards[1]);
    for (int i = 0; i < b.count(); i++) {
        bAttackers[bTotal] = parseCap(64, 64, n); //dont need to consider xray
        bTotal++;
    }
    std::bitset<64> b2(knightMoves & whiteBoards[1]);
    for (int i = 0; i < b2.count(); i++) {
        wAttackers[wTotal] = parseCap(64, 64, N); //dont need to consider xray
        wTotal++;
    }
    for (int i = 0; i < 8; i++) {
        xrayAttack(source, i, whiteBoards, blackBoards, miscBoards);
    }
    if ((kingAttacks[source] & blackBoards[5]) > 0) {
        bAttackers[bTotal] = parseCap(64, 64, k); //dont need to consider xray
        bTotal++;
    }
    if ((kingAttacks[source] & whiteBoards[5]) > 0) {
        wAttackers[wTotal] = parseCap(64, 64, K); //dont need to consider xray
        wTotal++;
    }
    while (true) {
        if (tempColor == 1 && wIndex == wTotal || tempColor == 0 && bIndex == bTotal) {
            break;
        }
        scores[scoresIndex] = attackedPieceValue - scores[scoresIndex - 1];
        //std::cout << attackedPieceValue << std::endl;
        scoresIndex++;
        if (tempColor == 1) {
            lowestValueIndex = wIndex;
            lowestValue = pieceValuesSEE[getCapPieceType(wAttackers[wIndex])];
            for (int i = wIndex+1; i < wTotal; i++) { 
                if (pieceValuesSEE[getCapPieceType(wAttackers[i])] < lowestValue) {
                    lowestValueIndex = i;
                    lowestValue = pieceValuesSEE[getCapPieceType(wAttackers[i])];
                }
            }

            if (lowestValueIndex != wIndex) { //didnt iterate to end of moves, therefore updated move
                temp = wAttackers[lowestValueIndex];
                wAttackers[lowestValueIndex] = wAttackers[wIndex];
                wAttackers[wIndex] = temp;
            }
            if (getCapDir(wAttackers[wIndex]) != 64) {
                xrayAttack(getCapSource(wAttackers[wIndex]), getCapDir(wAttackers[wIndex]), whiteBoards, blackBoards, miscBoards);
            }
            attackedPieceValue = lowestValue;
            tempColor ^= 1;
            wIndex++;
            //std::cout << "W: " << valToPiece[getCapPieceType(wAttackers[wIndex - 1])] << std::endl;
        }
        else {
            lowestValueIndex = bIndex;
            lowestValue = pieceValuesSEE[getCapPieceType(bAttackers[bIndex])];
            for (int i = bIndex + 1; i < bTotal; i++) {
                if (pieceValuesSEE[getCapPieceType(bAttackers[i])] < lowestValue) {
                    lowestValueIndex = i;
                    lowestValue = pieceValuesSEE[getCapPieceType(bAttackers[i])];
                }
            }

            if (lowestValueIndex != bIndex) { //didnt iterate to end of moves, therefore updated move
                temp = bAttackers[lowestValueIndex];
                bAttackers[lowestValueIndex] = bAttackers[bIndex];
                bAttackers[bIndex] = temp;
            }
            if (getCapDir(bAttackers[bIndex]) != 64) {
                xrayAttack(getCapSource(bAttackers[bIndex]), getCapDir(bAttackers[bIndex]), whiteBoards, blackBoards, miscBoards);
            }
            attackedPieceValue = lowestValue;
            tempColor ^= 1;
            bIndex++;
            //std::cout << "B: " << valToPiece[getCapPieceType(bAttackers[bIndex - 1])] << std::endl;
        }
    }
    //std::cout << scoresIndex;
    unMakeCapture(move, board, whiteBoards, blackBoards, miscBoards, capturedPiece, mvsource, mvdest, mvpiece, color); //
    while (scoresIndex > 1) {
        scoresIndex--;
        if (scores[scoresIndex - 1] > -scores[scoresIndex]) {
            scores[scoresIndex - 1] = -scores[scoresIndex];
        }
        //std::cout << scores[scoresIndex-1] << std::endl;
    }
    return scores[0];
    
}

void xrayAttack(int source, int dir, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t ray = allRays[source][dir];
    unsigned long lsb;
    uint64_t blockers = ray & miscBoards[0];
    if (!blockers) { //no pieces in ray
        return;
    }
    switch (dir) {
        case 3: case 5:{ //lsb scan, bishops queens
            _BitScanForward64(&lsb, blockers);
            uint64_t pieceBit = one << lsb;
            if ((pieceBit & blackBoards[2]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, b); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[2]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, B); 
                wTotal++;
            }
            else if ((pieceBit & blackBoards[4]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, q); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[4]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, Q); 
                wTotal++;
            }
            break;
        }
        case 2: case 4: {//lsb scan, rooks queens
            _BitScanForward64(&lsb, blockers);
            uint64_t pieceBit = one << lsb;
            if ((pieceBit & blackBoards[3]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, r); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[3]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, R); 
                wTotal++;
            }
            else if ((pieceBit & blackBoards[4]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, q); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[4]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, Q); 
                wTotal++;
            }
            break;
        }
            
        case 1: case 7:{ //msb scan, bishops queens
            _BitScanReverse64(&lsb, blockers);
            uint64_t pieceBit = one << lsb;
            if ((pieceBit & blackBoards[2]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, b); 
                bTotal++;
                //std::cout << bTotal << std::endl;
            }
            else if ((pieceBit & whiteBoards[2]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, B); 
                wTotal++;
            }
            else if ((pieceBit & blackBoards[4]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, q); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[4]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, Q); 
                wTotal++;
            }
            break;
        }
        case 0: case 6: { //msb scan, rooks queens
            _BitScanReverse64(&lsb, blockers);
            uint64_t pieceBit = one << lsb;
            if ((pieceBit & blackBoards[3]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, r); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[3]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, R); 
                wTotal++;
            }
            else if ((pieceBit & blackBoards[4]) > 0) {
                bAttackers[bTotal] = parseCap(lsb, dir, q); 
                bTotal++;
            }
            else if ((pieceBit & whiteBoards[4]) > 0) {
                wAttackers[wTotal] = parseCap(lsb, dir, Q); 
                wTotal++;
            }
            break;
        }
    }
}