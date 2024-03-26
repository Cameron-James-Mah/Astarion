#include <cstdint>

extern uint64_t zobristKey; //updated during search

//zobrist hashing for color/en passant/castling?

extern uint64_t zobristTable[12][64]; 

extern uint64_t colorHash;

void initZobrist();
void computeZobrist(int board[]);


