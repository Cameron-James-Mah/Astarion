//using namespace std;

int parseMove(int source, int target, int piece, int promotion, int capture, int doublePush, int enPassant, int castling);

int getMoveSource(int move);
int getMoveTarget(int move);
int getMovePiece(int move);
int getMoveIsPromotion(int move);
int getMoveIsCapture(int move);
int getMoveIsDoublePush(int move);
int getMoveIsEnPassant(int move);
int getMoveIsCastling(int move);


void getPawnMoves(int moves[], int &idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
void getKnightMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
void getBishopAndQueenMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
void getRookAndQueenMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
void getKingMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);
//void makeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, unsigned long &kingBit);
//void unMakeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, uint64_t castleRights);
void getMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]);