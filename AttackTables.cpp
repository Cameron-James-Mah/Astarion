#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>

#include "AttackTables.h"
#include "Globals.h"


using namespace std;

uint64_t pawnAttacksW[64];
uint64_t pawnAttacksB[64];
uint64_t knightAttacks[64];
uint64_t bishopAttacks[64][4];
uint64_t rookAttacks[64][4];
uint64_t kingAttacks[64];

/*
uint64_t notAFile = 0b01111111'01111111'01111111'01111111'01111111'01111111'01111111'01111111;
uint64_t notABFile = 0b00111111'00111111'00111111'00111111'00111111'00111111'00111111'00111111;
uint64_t notHFile = 0b11111110'11111110'11111110'11111110'11111110'11111110'11111110'11111110;
uint64_t notHGFile = 0b11111100'11111100'11111100'11111100'11111100'11111100'11111100'11111100;
uint64_t aFile = 0b00000000'10000000'10000000'10000000'10000000'10000000'10000000'00000000;*/


void generateTables() {
    generateKnightAttacks();
    generatePawnAttacksW();
    generatePawnAttacksB();
    generateBishopAttacks();
    generateRookAttacks();
    generateKingAttacks();
}

void setBit(uint64_t& b, int source) {
	uint64_t temp = 1;
	b |= temp << source;
}

void generatePawnAttacksW() {
    for (int i = 0; i < 64; i++) {
        uint64_t attacks = 0;
        uint64_t bitboard = 0;
        setBit(bitboard, i);
        //east capture
        if (((bitboard >> 7) & notAFile) > 0)
        {
            attacks |= bitboard >> 7;
        }
        //west capture
        if (((bitboard >> 9) & notHFile) > 0)
        {
            attacks |= bitboard >> 9;
        }
        pawnAttacksW[i] = attacks;
    }
}

void generatePawnAttacksB()
{
    for (int i = 0; i < 64; i++) {
        uint64_t attacks = 0;
        uint64_t bitboard = 0;
        setBit(bitboard, i);
        //east capture
        if (((bitboard << 9) & notAFile) > 0)
        {
            attacks |= bitboard << 9;
        }
        //west capture
        if (((bitboard << 7) & notHFile) > 0)
        {
            attacks |= bitboard << 7;
        }
        pawnAttacksB[i] = attacks;
    }
}

void generateKnightAttacks() {
	for (int i = 0; i < 64; i++) {
		uint64_t attacks = 0;
		uint64_t bitboard = 0;
		setBit(bitboard, i);
        //north jumps
        if (((bitboard << 6) & notHGFile) > 0)
        {
            attacks |= bitboard << 6;
        }
        if (((bitboard << 15) & notHFile) > 0)
        {
            attacks |= bitboard << 15;
        }
        if (((bitboard << 17) & notAFile) > 0)
        {
            attacks |= bitboard << 17;
        }
        if (((bitboard << 10) & notABFile) > 0)
        {
            attacks |= bitboard << 10;
        }
        //south jumps
        if (((bitboard >> 6) & notABFile) > 0)
        {
            attacks |= bitboard >> 6;
        }
        if (((bitboard >> 15) & notAFile) > 0)
        {
            attacks |= bitboard >> 15;
        }
        if (((bitboard >> 17) & notHFile) > 0)
        {
            attacks |= bitboard >> 17;
        }
        if (((bitboard >> 10) & notHGFile) > 0)
        {
            attacks |= bitboard >> 10;
        }
        knightAttacks[i] = attacks;
	}
}

void generateBishopAttacks() {
    for (int i = 0; i < 64; i++) {
        uint64_t attacks = 0;
        uint64_t bitboard = 0;
        setBit(bitboard, i);
        //NE ray
        int offset = 7;
        while (((bitboard >> offset) & notAFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard >> offset;
            offset += 7;
        }
        bishopAttacks[i][0] = attacks;
        attacks = 0;
        //NW ray
        offset = 9;
        while (((bitboard >> offset) & notHFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard >> offset;
            offset += 9;
        }
        bishopAttacks[i][1] = attacks;
        attacks = 0;
        //SE ray
        offset = 9;
        while (((bitboard << offset) & notAFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard << offset;
            offset += 9;
        }
        bishopAttacks[i][2] = attacks;
        attacks = 0;
        //SW ray
        offset = 7;
        while (((bitboard << offset) & notHFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard << offset;
            offset += 7;
        }
        bishopAttacks[i][3] = attacks;
    }
}

void generateRookAttacks()
{
    for (int i = 0; i < 64; i++) {
        uint64_t attacks = 0;
        uint64_t bitboard = 0;
        setBit(bitboard, i);
        //North ray
        int offset = 8;
        while (((bitboard >> offset) & (~row0)) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard >> offset;
            offset += 8;
        }
        rookAttacks[i][0] = attacks;
        attacks = 0;
        //West ray
        offset = 1;
        while (((bitboard >> offset) & notHFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard >> offset;
            offset += 1;
        }
        rookAttacks[i][1] = attacks;
        attacks = 0;
        //East ray
        offset = 1;
        while (((bitboard << offset) & notAFile) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard << offset;
            offset += 1;
        }
        rookAttacks[i][2] = attacks;
        attacks = 0;
        //South ray
        offset = 8;
        while (((bitboard << offset) & (~row7)) > 0)
        {
            bitboard = 0;
            setBit(bitboard, i);
            attacks |= bitboard << offset;
            offset += 8;
        }
        rookAttacks[i][3] = attacks;
    }
}

void generateKingAttacks() {
    for (int i = 0; i < 64; i++) {
        uint64_t bitboard = 0;
        setBit(bitboard, i);
        //W
        if (((bitboard << 1) & notAFile) > 0)
        {
            kingAttacks[i] |= bitboard << 1;
        }
        //NW
        if (((bitboard << 9) & notAFile) > 0)
        {
            kingAttacks[i] |= bitboard << 9;
        }
        //N
        if (((bitboard << 8)) > 0)
        {
            kingAttacks[i] |= bitboard << 8;
        }
        //NE
        if (((bitboard << 7) & notHFile) > 0)
        {
            kingAttacks[i] |= bitboard << 7;
        }

        //E
        if (((bitboard >> 1) & notHFile) > 0)
        {
            kingAttacks[i] |= bitboard >> 1;
        }
        //SE
        if (((bitboard >> 9) & notHFile) > 0)
        {
            kingAttacks[i] |= bitboard >> 9;
        }
        //S
        if (((bitboard >> 8)) > 0)
        {
            kingAttacks[i] |= bitboard >> 8;
        }
        //SW
        if (((bitboard >> 7) & notAFile) > 0)
        {
            kingAttacks[i] |= bitboard >> 7;
        }
    }
}
