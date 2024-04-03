#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <unordered_map>

//using namespace std;

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

extern int nodes;
extern int qNodes;
extern int pvLength;

extern int killers[30][2];
extern int history[12][64];

const int infinity = 200000;
const int mateVal = 100000;

const int rValue = 3;

const int qDepth = 6;

extern bool stop;
const int defaultDepth = 8;


//Piece IDs
const int P = 0;
const int N = 1;
const int B = 2;
const int R = 3;
const int Q = 4;
const int K = 5;
const int p = 6;
const int n = 7;
const int b = 8;
const int r = 9;
const int q = 10;
const int k = 11;

const std::string notation[64] = {
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

extern std::unordered_map<int, char> valToPiece;
extern std::unordered_map<char, int> pieceToVal;

const int pieceValues[12] = { 82, 337, 365, 477, 1025,  0, 82, 337, 365, 477, 1025,  0 };

extern std::unordered_map<uint64_t, int> repetition;



void pieceMaps();

