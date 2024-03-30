#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <intrin.h>

#include "MoveGen.h"
#include "Globals.h"
#include "Board.h"
#include "AttackTables.h"
#include "Position.h"


int parseMove(int source, int target, int piece, int promotion, int capture, int doublePush, int enPassant, int castling) {
    return source | (target << 6) | (piece << 12) | (promotion << 16) | (capture << 20) | (doublePush << 21) | (enPassant << 22) | (castling << 23);
}

int getMoveSource(int move) {
    return move & 0x3f;
}
int getMoveTarget(int move) {
    return (move & 0xfc0) >> 6;
}
int getMovePiece(int move) {
    return (move & 0xf000) >> 12;
}
int getMoveIsPromotion(int move) {
    return (move & 0xf0000) >> 16;
}
int getMoveIsCapture(int move) {
    return move & 0x100000;
}
int getMoveIsDoublePush(int move) {
    return move & 0x200000;
}
int getMoveIsEnPassant(int move) {
    return move & 0x400000;
}
int getMoveIsCastling(int move) {
    return move & 0x800000;
}


void getPawnMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t enemyPieces;
    uint64_t pawnsDest = 0;
    unsigned long lsb;
    if (color == 0)
    {
        enemyPieces = whiteBoards[6];
        //single pushes
        pawnsDest = blackBoards[0] << 8;
        pawnsDest &= miscBoards[1];
        uint64_t pawnsDest2 = pawnsDest;
        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            //if promoting move
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0) //made it to either end
            {
                moves[idx] = parseMove(lsb - 8, lsb, p, r, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 8, lsb, p, n, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 8, lsb, p, b, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 8, lsb, p, q, 0, 0, 0, 0);
                idx++;
            }
            else
            {
                moves[idx] = parseMove(lsb - 8, lsb, p, 0, 0, 0, 0, 0);
                idx++;
            }
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;


        }
        //double pushes
        pawnsDest2 = pawnsDest2 << 8;
        pawnsDest2 &= row4;
        pawnsDest2 &= miscBoards[1];
        while (pawnsDest2 != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest2);
            pawnsDest2 = ((uint64_t)1 << lsb) ^ pawnsDest2;
            moves[idx] = parseMove(lsb - 16, lsb, p, 0, 0, 1, 0, 0);
            idx++;
        }
        //pawn captures, en passant bits added to enemyPieces at beginning of function
        //east captures

        pawnsDest = blackBoards[0] << 9;
        pawnsDest &= enemyPieces | miscBoards[2];
        pawnsDest &= notAFile;
        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0) {
                moves[idx] = parseMove(lsb - 9, lsb, p, r, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 9, lsb, p, n, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 9, lsb, p, b, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 9, lsb, p, q, 1, 0, 0, 0);
                idx++;
            }
            else
            {
                if ((((uint64_t)1 << lsb) & miscBoards[2]) > 0)
                {
                    moves[idx] = parseMove(lsb - 9, lsb, p, 0, 1, 0, 1, 0);
                    idx++;
                }
                else
                {
                    moves[idx] = parseMove(lsb - 9, lsb, p, 0, 1, 0, 0, 0);
                    idx++;
                }

            }
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;

            //newMove.source = lsb + 9;
            //newMove.dest = lsb;

        }
        //west captures
        pawnsDest = blackBoards[0] << 7;
        pawnsDest &= enemyPieces | miscBoards[2];
        pawnsDest &= notHFile;
        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0)
            {
                moves[idx] = parseMove(lsb - 7, lsb, p, r, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 7, lsb, p, n, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 7, lsb, p, b, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb - 7, lsb, p, q, 1, 0, 0, 0);
                idx++;
            }
            else
            {
                if ((((uint64_t)1 << lsb) & miscBoards[2]) > 0) {
                    moves[idx] = parseMove(lsb - 7, lsb, p, 0, 1, 0, 1, 0);
                    idx++;
                }
                else
                {
                    moves[idx] = parseMove(lsb - 7, lsb, p, 0, 1, 0, 0, 0);
                    idx++;
                }

            }
        }
    }
    else
    {
        enemyPieces = blackBoards[6];
        //single pushes
        pawnsDest = whiteBoards[0] >> 8;
        pawnsDest &= miscBoards[1];
        uint64_t pawnsDest2 = pawnsDest;
        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            //if promoting move
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0) //made it to either end
            {
                moves[idx] = parseMove(lsb + 8, lsb, P, R, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 8, lsb, P, N, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 8, lsb, P, B, 0, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 8, lsb, P, Q, 0, 0, 0, 0);
                idx++;
            }
            else
            {
                moves[idx] = parseMove(lsb + 8, lsb, P, 0, 0, 0, 0, 0);
                idx++;
            }
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;


        }
        //double pushes
        pawnsDest2 = pawnsDest2 >> 8;
        pawnsDest2 &= row3;
        pawnsDest2 &= miscBoards[1];
        while (pawnsDest2 != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest2);
            pawnsDest2 = ((uint64_t)1 << lsb) ^ pawnsDest2;
            moves[idx] = parseMove(lsb + 16, lsb, P, 0, 0, 1, 0, 0);
            idx++;
        }

        //pawn captures, en passant bits added to enemyPieces at beginning of function
        //west captures
        pawnsDest = whiteBoards[0] >> 9;
        pawnsDest &= enemyPieces | miscBoards[2];
        pawnsDest &= notHFile;

        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0)
            {
                moves[idx] = parseMove(lsb + 9, lsb, P, R, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 9, lsb, P, N, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 9, lsb, P, B, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 9, lsb, P, Q, 1, 0, 0, 0);
                idx++;
            }
            else
            {
                if ((((uint64_t)1 << lsb) & miscBoards[2]) > 0)
                {
                    moves[idx] = parseMove(lsb + 9, lsb, P, 0, 1, 0, 1, 0);
                    idx++;
                }
                else
                {
                    moves[idx] = parseMove(lsb + 9, lsb, P, 0, 1, 0, 0, 0);
                    idx++;
                }

            }
        }
        //east captures
        pawnsDest = whiteBoards[0] >> 7;
        pawnsDest &= enemyPieces | miscBoards[2];
        pawnsDest &= notAFile;
        while (pawnsDest != 0)
        {
            _BitScanReverse64(&lsb, pawnsDest);
            pawnsDest = ((uint64_t)1 << lsb) ^ pawnsDest;
            if (((uint64_t)1 << lsb & row0) > 0 || ((uint64_t)1 << lsb & row7) > 0)
            {
                moves[idx] = parseMove(lsb + 7, lsb, P, R, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 7, lsb, P, N, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 7, lsb, P, B, 1, 0, 0, 0);
                idx++;
                moves[idx] = parseMove(lsb + 7, lsb, P, Q, 1, 0, 0, 0);
                idx++;
            }
            else
            {
                if ((((uint64_t)1 << lsb) & miscBoards[2]) > 0)
                {
                    moves[idx] = parseMove(lsb + 7, lsb, P, 0, 1, 0, 1, 0);
                    idx++;
                }
                else
                {
                    moves[idx] = parseMove(lsb + 7, lsb, P, 0, 1, 0, 0, 0);
                    idx++;
                }
            }
        }
    }
}
void getKnightMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t myKnights;
    uint64_t enemyPieces;
    unsigned long lsb;
    unsigned long kb;
    int piece = N;
    if (color == 1) {
        myKnights = whiteBoards[1];
        enemyPieces = blackBoards[6];
    }
    else {
        myKnights = blackBoards[1];
        enemyPieces = whiteBoards[6];
        piece = n;
    }
    while (myKnights != 0)
    {
        uint64_t mask;
        _BitScanReverse64(&lsb, myKnights);
        uint64_t knightMoves = knightAttacks[lsb] & (miscBoards[1] | enemyPieces);
        while (knightMoves != 0)
        {
            _BitScanReverse64(&kb, knightMoves);
            mask = (uint64_t)1 << kb;
            knightMoves = mask ^ knightMoves;
            if (mask & enemyPieces) {
                moves[idx] = parseMove(lsb, kb, piece, 0, 1, 0, 0, 0);
            }
            else {
                moves[idx] = parseMove(lsb, kb, piece, 0, 0, 0, 0, 0);
            }
            idx++;
        }
        myKnights = ((uint64_t)1 << lsb) ^ myKnights;
    }
}

void getBishopAndQueenMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t myBishops;
    uint64_t myQueens;
    uint64_t enemyPieces;
    int piece = B;
    int qPiece = Q;
    if (color == 1) {
        myBishops = whiteBoards[2]; 
        myQueens = whiteBoards[4];
        enemyPieces = blackBoards[6];
    }
    else {
        myBishops = blackBoards[2];
        myQueens = blackBoards[4];
        enemyPieces = whiteBoards[6];
        piece = b;
        qPiece = q;
    }
    unsigned long lsb;
    unsigned long msb;
    unsigned long bb;
    while (myBishops != 0)
    {
        _BitScanReverse64(&lsb, myBishops);
        myBishops = ((uint64_t)1 << lsb) ^ myBishops;
        //printBitBoard(blackKnights);
        //0 = SW, 1 = SE, 2 = NW, 3 = NE
        uint64_t SWmoves = bishopAttacks[lsb][3];
        uint64_t SEmoves = bishopAttacks[lsb][2];
        uint64_t NWmoves = bishopAttacks[lsb][1];
        uint64_t NEmoves = bishopAttacks[lsb][0];
        //bitwise & with allPieces, bitscan for lsb or msb depending on dir to get first blocker, everything between is a valid square, then check if blocker is enemy for additional square
        //SW
        uint64_t blockers = SWmoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        uint64_t validSquares = 0;
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][3] & (~bishopAttacks[msb][3]);
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
        }
        else
        {
            validSquares = bishopAttacks[lsb][3];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }

        //SE
        blockers = SEmoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][2] & (~bishopAttacks[msb][2]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][2];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //NW
        blockers = NWmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers); //find first blocker
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][1] & (~bishopAttacks[msb][1]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][1];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //NE
        blockers = NEmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers); //find first blocker
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][0] & (~bishopAttacks[msb][0]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][0];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }
    }
    piece = qPiece;
    while (myQueens != 0)
    {
        _BitScanReverse64(&lsb, myQueens);
        myQueens = ((uint64_t)1 << lsb) ^ myQueens;
        //printBitBoard(blackKnights);
        //0 = SW, 1 = SE, 2 = NW, 3 = NE
        uint64_t SWmoves = bishopAttacks[lsb][3];
        uint64_t SEmoves = bishopAttacks[lsb][2];
        uint64_t NWmoves = bishopAttacks[lsb][1];
        uint64_t NEmoves = bishopAttacks[lsb][0];
        //bitwise & with allPieces, bitscan for lsb or msb depending on dir to get first blocker, everything between is a valid square, then check if blocker is enemy for additional square
        //SW
        uint64_t blockers = SWmoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        uint64_t validSquares = 0;
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][3] & (~bishopAttacks[msb][3]);
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
        }
        else
        {
            validSquares = bishopAttacks[lsb][3];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }

        //SE
        blockers = SEmoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][2] & (~bishopAttacks[msb][2]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][2];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //NW
        blockers = NWmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers); //find first blocker
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][1] & (~bishopAttacks[msb][1]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][1];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //NE
        blockers = NEmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers); //find first blocker
        if (blockers) //found blocker 
        {
            validSquares = bishopAttacks[lsb][0] & (~bishopAttacks[msb][0]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = bishopAttacks[lsb][0];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }
    }
}

void getRookAndQueenMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    uint64_t myRooks;
    uint64_t myQueens;
    uint64_t enemyPieces;
    int piece = R;
    int qPiece = Q;
    if (color == 1) {
        myRooks = whiteBoards[3];
        myQueens = whiteBoards[4];
        enemyPieces = blackBoards[6];
    }
    else {
        myRooks = blackBoards[3];
        myQueens = blackBoards[4];
        enemyPieces = whiteBoards[6];
        piece = r;
        qPiece = q;
    }
    unsigned long lsb;
    unsigned long msb;
    unsigned long bb;
    while (myRooks != 0)
    {
        _BitScanReverse64(&lsb, myRooks);
        myRooks = ((uint64_t)1 << lsb) ^ myRooks;
        //printBitBoard(blackKnights);
        //0 = S][1 = E][2 = W][3 = N
        uint64_t Smoves = rookAttacks[lsb][3];
        uint64_t Emoves = rookAttacks[lsb][2];
        uint64_t Wmoves = rookAttacks[lsb][1];
        uint64_t Nmoves = rookAttacks[lsb][0];
        //bitwise & with allPieces][bitscan for lsb or msb depending on dir to get first blocker][everything between is a valid square][then check if blocker is enemy for additional square
        //S
        uint64_t blockers = Smoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        uint64_t validSquares = 0;
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][3] & (~rookAttacks[msb][3]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][3];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //E
        blockers = Emoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][2] & (~rookAttacks[msb][2]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][2];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //W
        blockers = Wmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][1] & (~rookAttacks[msb][1]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][1];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //N
        blockers = Nmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][0] & (~rookAttacks[msb][0]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][0];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }
    }
    piece = qPiece;
    while (myQueens != 0)
    {
        _BitScanReverse64(&lsb, myQueens);
        myQueens = ((uint64_t)1 << lsb) ^ myQueens;
        //printBitBoard(blackKnights);
        //0 = S][1 = E][2 = W][3 = N
        uint64_t Smoves = rookAttacks[lsb][3];
        uint64_t Emoves = rookAttacks[lsb][2];
        uint64_t Wmoves = rookAttacks[lsb][1];
        uint64_t Nmoves = rookAttacks[lsb][0];
        //bitwise & with allPieces][bitscan for lsb or msb depending on dir to get first blocker][everything between is a valid square][then check if blocker is enemy for additional square
        //S
        uint64_t blockers = Smoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        uint64_t validSquares = 0;
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][3] & (~rookAttacks[msb][3]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][3];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //E
        blockers = Emoves & miscBoards[0];
        _BitScanForward64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][2] & (~rookAttacks[msb][2]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][2];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //W
        blockers = Wmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][1] & (~rookAttacks[msb][1]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][1];
        }

        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }


        //N
        blockers = Nmoves & miscBoards[0];
        _BitScanReverse64(&msb, blockers);
        if (blockers) //found blocker 
        {
            validSquares = rookAttacks[lsb][0] & (~rookAttacks[msb][0]);
            validSquares = ((uint64_t)1 << msb) ^ validSquares;
            if ((((uint64_t)1 << msb) & enemyPieces) > 0) //check if blocker is enemy piece(capturable)
            {
                moves[idx] = parseMove(lsb, msb, piece, 0, 1, 0, 0, 0);
                idx++;
            }
        }
        else
        {
            validSquares = rookAttacks[lsb][0];
        }
        while (validSquares != 0)
        {
            _BitScanReverse64(&bb, validSquares);
            validSquares = ((uint64_t)1 << bb) ^ validSquares;
            moves[idx] = parseMove(lsb, bb, piece, 0, 0, 0, 0, 0);
            idx++;
        }

    }
}
void getKingMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    unsigned long lsb = 0, kb = 0;
    uint64_t enemyPieces = 0, castleRights = miscBoards[3], myKing = 0, myRook = 0;
    uint64_t ePawn = 0, eKnight = 0, eBishop = 0, eRook = 0, eQueen = 0, eKing = 0;
    uint64_t mask = 0;
    int piece = K;
    if (color == 1) {
        _BitScanReverse64(&lsb, whiteBoards[5]);
        enemyPieces = blackBoards[6];
        myKing = whiteBoards[5];
        myRook = whiteBoards[3];
        uint64_t ePawn = blackBoards[0], eKnight = blackBoards[1], eBishop = blackBoards[2], eRook = blackBoards[3], eQueen = blackBoards[4], eKing = blackBoards[5];
    }
    else {
        _BitScanReverse64(&lsb, blackBoards[5]);
        enemyPieces = whiteBoards[6];
        piece = k;
        myKing = blackBoards[5];
        myRook = blackBoards[3];
        uint64_t ePawn = whiteBoards[0], eKnight = whiteBoards[1], eBishop = whiteBoards[2], eRook = whiteBoards[3], eQueen = whiteBoards[4], eKing = whiteBoards[5];
    }
    uint64_t kingMoves = kingAttacks[lsb] & (miscBoards[1] | enemyPieces);
    while (kingMoves != 0)
    {
        _BitScanReverse64(&kb, kingMoves);
        mask = (uint64_t)1 << kb;
        kingMoves = mask ^ kingMoves;
        if (mask & enemyPieces) {
            moves[idx] = parseMove(lsb, kb, piece, 0, 1, 0, 0, 0);
        }
        else {
            moves[idx] = parseMove(lsb, kb, piece, 0, 0, 0, 0, 0);
        }
        idx++;
    }

    //castling moves
    if ((myKing & castleRights) > 0 && !isSquareAttacked(lsb, color, whiteBoards, blackBoards, miscBoards)) //dont forget to check if in check
    {
        uint64_t shortSquares = myKing << 1 | myKing << 2;
        uint64_t longSquares = myKing >> 1 | myKing >> 2 | myKing >> 3;
        castleRights &= myRook;
        if (((myKing >> 4) & castleRights) > 0 && (longSquares & miscBoards[0]) == 0 && !isSquareAttacked(lsb - 1, color, whiteBoards, blackBoards, miscBoards) && !isSquareAttacked(lsb - 2, color, whiteBoards, blackBoards, miscBoards)) //long castle && not in check or castling through check
        {
            moves[idx] = parseMove(lsb, lsb - 2, piece, 0, 0, 0, 0, 1);
            idx++;
        }
        //printBitBoard(castleRights);
        if (((myKing << 3) & castleRights) > 0 && (shortSquares & miscBoards[0]) == 0 && !isSquareAttacked(lsb+1, color, whiteBoards, blackBoards, miscBoards) && !isSquareAttacked(lsb + 2, color, whiteBoards, blackBoards, miscBoards)) //short castle && not in check or castling through check
        {
            moves[idx] = parseMove(lsb, lsb + 2, piece, 0, 0, 0, 0, 1);
            idx++;

        }
    }
}

