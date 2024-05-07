#include <stdint.h>
//using namespace std;
bool isSquareAttacked(int source, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
int SEE(int source, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], int move, int color, int enP);
void xrayAttack(int source, int dir, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);

