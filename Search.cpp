#include <stdint.h>
#include <iostream>
#include "MoveGen.h"
#include "Board.h"
#include "Position.h"
#include "Globals.h"
#include "Evaluation.h"
#include "Search.h"
#include "Zobrist.h"
#include "TranspositionTable.h"
using namespace std;

void negamaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
	int moves[256];
	int idx = 0;
	unsigned long kingBit;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, -1);
	int bestMove;
	int currVal;
	int alpha = -123456;
	int beta = 123456;
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			currVal = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth-1, board, -beta, -alpha);
			if (currVal > alpha) {
				alpha = currVal;
				bestMove = moves[i];
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	cout << "bestmove " << notation[getMoveSource(bestMove)] << notation[getMoveTarget(bestMove)] << endl;
}
int negamax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta) {
	int hashMove = -1; 
	if (int val = probeHash(depth, alpha, beta, hashMove) != noEntry) {
		return val;
	}
	if (depth == 0) {
		int val = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		recordEntry(depth, val, hashFlagExact, -1);
		return val;
	}
	int hashF = hashFlagAlpha;
	int moves[256];
	int idx = 0;
	unsigned long kingBit;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, hashMove);
	int score;
	int bestMove = -1;
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth-1, board, -beta, -alpha);
			if (score >= beta) {
				recordEntry(depth, beta, hashFlagBeta, moves[i]);
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
				return beta;
			}
			if (score > alpha) {
				bestMove = moves[i];
				hashF = hashFlagExact;
				alpha = score;
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	recordEntry(depth, alpha, hashF, bestMove);
	return alpha;
}

//Minimax stopped before fully implementing tt table
void minimaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[]) {
	int moves[256];
	int idx = 0;
	unsigned long kingBit;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, -1);
	int bestMove;
	int currVal;
	int alpha = -infinity;
	int beta = infinity;
	int maxEval = -infinity;
	int minEval = infinity;
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12;
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			currVal = minimax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, alpha, beta);
			if (color == 1) {
				if (currVal > alpha) {
					alpha = currVal;
					bestMove = moves[i];
					recordEntry(depth, alpha, hashFlagAlpha, moves[i]);
				}
			}
			else {
				if (currVal < beta) {
					beta = currVal;
					bestMove = moves[i];
					recordEntry(depth, beta, hashFlagBeta, moves[i]);
				}
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	cout << "bestmove " << notation[getMoveSource(bestMove)] << notation[getMoveTarget(bestMove)] << endl;
}

int minimax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta) {
	int hashMove = -1; 
	if (int val = probeHash(depth, alpha, beta, hashMove) != noEntry) {
		return val;
	}
	if (depth == 0) {
		int val = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		if (color == 1) {
			recordEntry(depth, val, hashFlagAlpha, -1);
		}
		else {
			recordEntry(depth, val, hashFlagBeta, -1);
		}
		return val;
	}
	int moves[256];
	int idx = 0;
	unsigned long kingBit;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, hashMove);
	int score;
	int maxEval = -infinity;
	int minEval = infinity;
	int bestMove = -1;
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12;
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			score = minimax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, alpha, beta);
			if (color == 1) {
				if (score > alpha) {
					bestMove = moves[i];
					maxEval = max(maxEval, score);
					alpha = max(alpha, score);
					if (beta <= alpha) {
						unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
						recordEntry(depth, alpha, hashFlagAlpha, moves[i]);
						break;
					}
				}
			}
			else {
				if (score < beta) {
					bestMove = moves[i];
					minEval = min(minEval, score);
					beta = min(beta, score);
					if (beta <= alpha) {
						unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
						recordEntry(depth, beta, hashFlagBeta, moves[i]);
						break;
					}
				}
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	if (color == 1) {
		if (bestMove != -1) {
			recordEntry(depth, alpha, hashFlagAlpha, bestMove);
		}
		return maxEval;
	}
	else {
		if (bestMove != -1) {
			recordEntry(depth, beta, hashFlagBeta, bestMove);
		}
		return minEval;
	}
}


void sortMoves(int idx, int moves[], int board[], int hashMove) {
	int* moveScores = new int[idx];
	for (int i = 0; i < idx; i++) {
		moveScores[i] = scoreMove(moves[i], board, hashMove);
	}
	quickSortMoves(moves, moveScores, 0, idx - 1);
	delete[] moveScores;
}
int scoreMove(int move, int board[], int hashMove) {
	if (move == hashMove) {
		return 20001;
	}
	if (getMoveIsPromotion(move) == Q || getMoveIsPromotion(move) == q) {
		return 20000;
	}
	if (getMoveIsCapture(move)) {
		return 10000 - board[getMoveSource(move)] + board[getMoveTarget(move)];
	}
	return 0;
}

void printMoves(int idx, int moves[]) {
	for (int i = 0; i < idx; i++) {
		cout << "Source square: " << getMoveSource(moves[i]) << " End Square: " << getMoveTarget(moves[i]) << " Promotion: " << getMoveIsPromotion(moves[i]) << getMoveIsCapture(moves[i]) << " | " << getMoveIsCastling(moves[i]) << " | " << getMoveIsDoublePush(moves[i]) << " | " << getMoveIsEnPassant(moves[i]) << " | " << endl;
	}
}

void quickSortMoves(int moves[], int moveScores[], int low, int high) {
	if (low < high)
	{
		int pivot = moveScores[high];
		int i = low - 1;
		for (int j = low; j <= high - 1; j++)
		{
			if (moveScores[j] > pivot)
			{
				i++;
				std::swap(moveScores[i], moveScores[j]);
				std::swap(moves[i], moves[j]);
			}
		}
		std::swap(moveScores[i + 1], moveScores[high]);
		std::swap(moves[i + 1], moves[high]);
		int pi = i + 1;
		quickSortMoves(moves, moveScores, low, pi - 1);
		quickSortMoves(moves, moveScores, pi + 1, high);
	}
}
