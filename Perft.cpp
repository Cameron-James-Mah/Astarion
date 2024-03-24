#include <stdint.h>
#include "MoveGen.h"
#include "Board.h"
#include "Position.h"
#include "Globals.h"
#include <iostream>
using namespace std;
void printBBs(uint64_t bb[]);
unsigned long perft(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);
unsigned long perftHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);

unsigned long perftHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
	int moves[256];
	int idx = 0;
	int nodes = 0;
	unsigned long kingBit;
	unsigned long currNodes = 0;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		currNodes = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 0;
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			currNodes = perft(newColor, whiteBoards, blackBoards, miscBoards, depth-1, board);
			cout << notation[getMoveSource(moves[i])] << notation[getMoveTarget(moves[i])] << ": "  << currNodes << endl;
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
		nodes += currNodes;
	}
	/*
	printBoard2(board);
	printBBs(whiteBoards);
	printBBs(blackBoards);*/
	return nodes;
}

unsigned long perft(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
	if (depth == 0) {
		/*
		printBoard2(board);
		printBBs(whiteBoards);
		printBBs(blackBoards);
		cout << "-----------------------" << endl;*/
		return 1;
	}
	int moves[256];
	int idx = 0;
	unsigned long nodes = 0;
	int newColor = color ^ 1;
	unsigned long kingBit;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 0;
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			//cout << "Source square: " << notation[getMoveSource(moves[i])] << " End Square: " << notation[getMoveTarget(moves[i])] << " Promotion: " << getMoveIsPromotion(moves[i]) << endl;
			nodes += perft(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board);
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	miscBoards[2] = 0;
	return nodes;
}

void printBBs(uint64_t bb[]) {
	int col = 0;
	uint64_t temp = 1;
	for (int k = 0; k < 6; k++) {
		for (int i = 0; i < 64; i++) {
			if (((temp << i) & bb[k]) > 0) {
				cout << "1 ";
			}
			else {
				cout << "0 ";
			}
			col++;
			if (col == 8) {
				cout << endl;
				col = 0;
			}
		}
		cout << endl << endl;
		col = 0;
	}
}