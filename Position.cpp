#include<intrin.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <intrin.h>

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
