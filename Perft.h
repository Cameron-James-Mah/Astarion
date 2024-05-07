#include <stdint.h>

uint64_t perft(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);
uint64_t perftHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);
void printBBs(uint64_t bb[]);
