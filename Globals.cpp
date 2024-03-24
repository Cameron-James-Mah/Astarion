#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <unordered_map>

#include "Globals.h"

using namespace std;

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

unordered_map<int, char> valToPiece;
unordered_map<char, int> pieceToVal;

void pieceMaps() {
	valToPiece[1] = 'P'; valToPiece[2] = 'N'; valToPiece[3] = 'B'; valToPiece[4] = 'R'; valToPiece[5] = 'Q'; valToPiece[6] = 'K';
	valToPiece[7] = 'p'; valToPiece[8] = 'n'; valToPiece[9] = 'b'; valToPiece[10] = 'r'; valToPiece[11] = 'q'; valToPiece[12] = 'k';
	valToPiece[0] = ' ';

	pieceToVal['P'] = 1; pieceToVal['N'] = 2; pieceToVal['B'] = 3; pieceToVal['R'] = 4; pieceToVal['Q'] = 5; pieceToVal['K'] = 6;
	pieceToVal['p'] = 7; pieceToVal['n'] = 8; pieceToVal['b'] = 9; pieceToVal['r'] = 10; pieceToVal['q'] = 11; pieceToVal['k'] = 12;
	pieceToVal[' '] = 0;
}



