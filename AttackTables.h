#include <iostream>
#include <vector>
#include <algorithm>
#include <stdint.h>

//using namespace std;


extern uint64_t pawnAttacksW[64];
extern uint64_t pawnAttacksB[64];
extern uint64_t knightAttacks[64];
extern uint64_t bishopAttacks[64][4];
extern uint64_t rookAttacks[64][4];
extern uint64_t kingAttacks[64];
extern uint64_t enPassantAttacks[64];

extern uint64_t allRays[64][8];

void generateTables();

void generatePawnAttacksW();
void generatePawnAttacksB();
void generateKnightAttacks();
void generateBishopAttacks();
void generateRookAttacks();
void generateKingAttacks();
void generateEnPassantAttacks();

void setBit(uint64_t& b, int source);



