#include <cstdint>

extern uint64_t zobristKey; //updated during search

//zobrist hashing for color/en passant/castling?

extern uint64_t zobristTable[13][64]; 

extern uint64_t blackHash;
extern uint64_t castleHash[4];
extern uint64_t enPassantHash[8];

void initZobrist();
void computeZobrist(int board[], int color, uint64_t miscBoards[]);


