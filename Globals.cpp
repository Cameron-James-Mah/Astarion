#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <unordered_map>

#include "Globals.h"


uint64_t one = 1;

//FLIPPED ALL BITMASKS
uint64_t row7 = 0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'11111111;
uint64_t row6 = 0b00000000'00000000'00000000'00000000'00000000'00000000'11111111'00000000;
uint64_t row5 = 0b00000000'00000000'00000000'00000000'00000000'11111111'00000000'00000000;
uint64_t row4 = 0b00000000'00000000'00000000'00000000'11111111'00000000'00000000'00000000;
uint64_t row3 = 0b00000000'00000000'00000000'11111111'00000000'00000000'00000000'00000000;
uint64_t row2 = 0b00000000'00000000'11111111'00000000'00000000'00000000'00000000'00000000;
uint64_t row1 = 0b00000000'11111111'00000000'00000000'00000000'00000000'00000000'00000000;
uint64_t row0 = 0b11111111'00000000'00000000'00000000'00000000'00000000'00000000'00000000;

//column bitmasks
uint64_t notHFile = 0b01111111'01111111'01111111'01111111'01111111'01111111'01111111'01111111;
uint64_t notHGFile = 0b00111111'00111111'00111111'00111111'00111111'00111111'00111111'00111111;
uint64_t notAFile = 0b11111110'11111110'11111110'11111110'11111110'11111110'11111110'11111110;
uint64_t notABFile = 0b11111100'11111100'11111100'11111100'11111100'11111100'11111100'11111100;
/*
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
const int k = 12;*/

int nodes = 0;

int killers[30][2];
int history[12][64];


std::unordered_map<int, char> valToPiece;
std::unordered_map<char, int> pieceToVal;

std::unordered_map<uint64_t, int> repetition;

void pieceMaps() {
	valToPiece[0] = 'P'; valToPiece[1] = 'N'; valToPiece[2] = 'B'; valToPiece[3] = 'R'; valToPiece[4] = 'Q'; valToPiece[5] = 'K';
	valToPiece[6] = 'p'; valToPiece[7] = 'n'; valToPiece[8] = 'b'; valToPiece[9] = 'r'; valToPiece[10] = 'q'; valToPiece[11] = 'k';
	valToPiece[12] = ' ';

	pieceToVal['P'] = 0; pieceToVal['N'] = 1; pieceToVal['B'] = 2; pieceToVal['R'] = 3; pieceToVal['Q'] = 4; pieceToVal['K'] = 5;
	pieceToVal['p'] = 6; pieceToVal['n'] = 7; pieceToVal['b'] = 8; pieceToVal['r'] = 9; pieceToVal['q'] = 10; pieceToVal['k'] = 11;
	pieceToVal[' '] = 12;
}



