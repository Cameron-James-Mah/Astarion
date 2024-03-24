#include <stdint.h>
#include "Globals.h"
int evaluate(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[]) {
    int pawn = 120; //P/p
    int knight = 420; //N/n
    int bishop = 470; //B/b
    int rook = 750; //R/r
    int queen = 1400; //Q/q
    int king = 0; //K/k
    int score = 0;
    int whiteMaterial = 0; //material not including pawns/king
    int blackMaterial = 0; //material not including pawns/king
    int wKingIdx = 0; //index of white king
    int bKingIdx = 0; //index of black king
    int currPiece = 1;
    for (int i = 0; i < 6; i++) {
        uint64_t bitboard = whiteBoards[i];
        unsigned long lsb;
        while (bitboard) {
            _BitScanReverse64(&lsb, bitboard);
            switch (currPiece) {
                case P:
                    score += pawn;
                    score += pawnSqauresW[lsb];
                    whiteMaterial += pawn;
                    break;
                case N:
                    score += knight;
                    score += knightSquares[lsb];
                    whiteMaterial += knight;
                    break;
                case B:
                    score += bishop;
                    score += bishopSquaresW[lsb];
                    whiteMaterial += bishop;
                    break;
                case R:
                    score += rook;
                    score += rookSquaresW[lsb];
                    whiteMaterial += rook;
                    break;
                case Q:
                    score += queen;
                    score += queenSquares[lsb];
                    whiteMaterial += queen;
                    break;
                case K:
                    wKingIdx = lsb;
                    break;

            }
            bitboard ^= one << lsb;
        }
        currPiece++;
    }

    for (int i = 0; i < 6; i++) {
        uint64_t bitboard = blackBoards[i];
        unsigned long lsb;
        while (bitboard) {
            _BitScanReverse64(&lsb, bitboard);
            switch (currPiece) {
            case p:
                score -= pawn;
                score -= pawnSquaresB[lsb];
                blackMaterial += pawn;
                break;
            case n:
                score -= knight;
                score -= knightSquares[lsb];
                blackMaterial += knight;
                break;
            case b:
                score -= bishop;
                score -= bishopSquaresB[lsb];
                blackMaterial += bishop;
                break;
            case r:
                score -= rook;
                score -= rookSquaresB[lsb];
                blackMaterial += rook;
                break;
            case q:
                score -= queen;
                score -= queenSquares[lsb];
                blackMaterial += queen;
                break;
            case k:
                bKingIdx = lsb;
                break;
            }
            bitboard ^= one << lsb;
        }
        currPiece++;
    }
    if (blackMaterial > 1300 || whiteMaterial > 1300)
    {
        score -= kingSquaresMiddleB[bKingIdx];
        score += kingSquaresMiddleW[wKingIdx];
    }
    /*
    if (color == 1) {
        return score;
    }
    else {
        return -score;
    }*/
    return score;
}