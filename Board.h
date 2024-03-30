#include <string>
#include <stdint.h>
#include <vector>

void printBitBoard(uint64_t bitboard);
void printBoard(char board[64]);
void printBoard2(int board[64]);
void resetBoard(int board[64], uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);
void updateFromFen(int board[64], std::string fen, uint64_t miscBoards[], int &color);
void updateBoard(int board[64], std::vector<std::string>& moves, uint64_t miscBoards[4]);
void updateBitBoards(char board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]);
void updateBitBoards2(int board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]);
int getCellNumber(std::string cell);
void addBitBoardPiece(int piece, int dest, uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);
void removeBitBoardPiece(int piece, int dest, uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);
void updateMiscBoards(uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);
void clearKillers();
void makeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, unsigned long& kingBit);
void unMakeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, uint64_t castleRights);