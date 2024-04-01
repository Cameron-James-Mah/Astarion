#include <stdint.h>
#include "MoveGen.h"
#include "Board.h"
#include "Position.h"
#include "Globals.h"
#include <iostream>

void printBBs(uint64_t bb[]);
uint64_t perft(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);
uint64_t perftHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]);

uint64_t perftHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
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
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			currNodes = perft(newColor, whiteBoards, blackBoards, miscBoards, depth-1, board);
			std::cout << notation[getMoveSource(moves[i])] << notation[getMoveTarget(moves[i])] << ": "  << currNodes << std::endl;
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

uint64_t perft(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
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
		int capturedPiece = 12; //empty space
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
				std::cout << "1 ";
			}
			else {
				std::cout << "0 ";
			}
			col++;
			if (col == 8) {
				std::cout << std::endl;
				col = 0;
			}
		}
		std::cout << std::endl << std::endl;
		col = 0;
	}
}