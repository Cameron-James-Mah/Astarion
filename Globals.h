#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <unordered_map>

using namespace std;

extern uint64_t one;

extern uint64_t row0;
extern uint64_t row1;
extern uint64_t row2;
extern uint64_t row3;
extern uint64_t row4;
extern uint64_t row5;
extern uint64_t row6;
extern uint64_t row7;


//column bitmasks
extern uint64_t notAFile;
extern uint64_t notABFile;
extern uint64_t notHFile;
extern uint64_t notHGFile;


//Piece IDs
const int P = 1;
const int N = 2;
const int B = 3;
const int R = 4;
const int Q = 5;
const int K = 6;
const int p = 7;
const int n = 8;
const int b = 9;
const int r = 10;
const int q = 11;
const int k = 12;

const string notation[64] = {
    "a8","b8","c8","d8","e8","f8","g8","h8",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a1","b1","c1","d1","e1","f1","g1","h1",
};



const uint64_t wCastleRightsMask = 0b10010001'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
const uint64_t bCastleRightsMask = 0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10010001;

extern unordered_map<int, char> valToPiece;
extern unordered_map<char, int> pieceToVal;

const int pieceValues[13] = {0, 100, 420, 470, 750, 1400, 1700, 100, 420, 470, 750, 1400, 1700 }; //offset by 1 since P = 1

const int infinity = 100000;
/*
int whiteBoardSize;
int blackBoardSize;
int miscBoardSize;*/

void pieceMaps();

//These psq tables match my notation array, and an index in this array matches the amount of left shifts in my bitboards
const int pawnSqauresW[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    100,100, 100,100, 100, 100, 100, 100,
    50, 50, 50, 50, 50, 50, 50, 50,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int pawnSquaresB[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    50, 50, 50, 50, 50, 50, 50, 50,
    100,100, 100,100, 100, 100, 100, 100,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int knightSquares[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const int bishopSquaresB[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int bishopSquaresW[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int rookSquaresB[64] = {
    0,  2,  3,  5,  5,  3,  2,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0,
};

const int rookSquaresW[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  2,  3,  5,  5,  3,  2,  0
};

const int queenSquares[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int kingSquaresMiddleW[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 20,  0,  0, 10, 30, 20
};

const int kingSquaresMiddleB[64] = {
    20, 30, 20,  0,  0, 10, 30, 20,
    20, 20,  0,  0,  0,  0, 20, 20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
};

const int kingSquaresEnd[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -40,-30, 0,  0,  0, 0, -30, -40,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -40,-30,  0,  0,  0,  0,-30,-40,
    -50,-40,-30,-20,-20,-30,-40,-50
};

const int kingSquaresCorner[64] = {
    -100,-80,-60,-20,-20,-60,-80,-100,
    -80,-60, 0,  0,  0, 0, -60, -80,
    -60,-10, 20, 30, 30, 20,-10,-60,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -60,-10, 20, 30, 30, 20,-10,-60,
    -80,-60,  0,  0,  0,  0,-60,-80,
    -100,-80,-60,-20,-20,-60,-80,-100
};