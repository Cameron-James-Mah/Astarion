#include <stdint.h>
#include <vector>
//using namespace std;

void negamaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], bool setDepth);
int negamax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta, bool tryNull);

//void minimaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);
//int minimax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta);


//move sorting functions
void sortMoves(int idx, int moves[], int board[], int hashMove, int depth);
int scoreMove(int move, int board[], int hashMove, int depth);
//int scoreCapture(int move, int board[]);
void printMoves(int idx, int moves[]);
void quickSortMoves(int moves[], int moveScores[], int low, int high);
void collectPv(std::vector<int>& pvList, int depth, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int color);
int qSearch(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], int alpha, int beta);
void sortCaptures(int idx, int moves[], int board[]);
int scoreCapture(int move, int board[]);