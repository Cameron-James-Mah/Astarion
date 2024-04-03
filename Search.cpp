#include <stdint.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>

#include "MoveGen.h"
#include "Board.h"
#include "Position.h"
#include "Globals.h"
#include "Evaluation.h"
#include "Search.h"
#include "Zobrist.h"
#include "TranspositionTable.h"

std::chrono::time_point<std::chrono::system_clock> start, end;

void negamaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], bool setDepth) {
	start = std::chrono::system_clock::now();
	nodes++;
	int moves[218];
	int idx = 0;
	int depth = 0;
	unsigned long kingBit;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	//sortMoves(idx, moves, board, -1, depth);
	int bestMove = 1;
	int maxDepth = 21;
	if (setDepth) {
		maxDepth = defaultDepth;
	}
	for (int j = 1; j < maxDepth+1; j++) {
		int currVal;
		int alpha = -infinity;
		int beta = infinity;
		int unmake = 0;
		int currBest;
		currAge = j;
		int hashMove = -1;
		Entry* temp = getEntry();
		if (temp != nullptr) {
			hashMove = temp->bestMove;
		}
		sortMoves(idx, moves, board, hashMove, j);
		for (int i = 0; i < idx; i++) {
			miscBoards[2] = 0;
			unmake = 0;
			int source = getMoveSource(moves[i]);
			int dest = getMoveTarget(moves[i]);
			int piece = getMovePiece(moves[i]);
			int capturedPiece = 12; //empty space
			makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
			if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
				currVal = -negamax(newColor, whiteBoards, blackBoards, miscBoards, j - 1, board, -beta, -alpha, true);
				if (currVal > alpha) {
					alpha = currVal;
					unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights); //unmake before becuase zobrist key updated in make/unmake
					unmake = 1;
					recordEntry(j, alpha, hashFlagAlpha, moves[i]);
					currBest = moves[i];
				}
				//std::cout << currVal << std::endl;
			}
			if (!unmake) { //I have to do this because i update my zobrist key in my make/unmake functions, and I have to update zobrist key before hashing
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
			}
			//unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
			if (stop) {
				break;
			}
		}
		if (stop) {
			break;
		}
		bestMove = currBest;
		depth++;
	}/*
	//collect pv
	std::vector<int> pvList;
	collectPv(pvList, 8, board, whiteBoards, blackBoards, miscBoards, color);
	std::cout << "PV: ";
	for (int i = 0; i < 8; i++) {
		std::cout << notation[getMoveSource(pvList[i])] << notation[getMoveTarget(pvList[i])] << " ";
	}
	std::cout << std::endl;*/
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	if (getMoveIsPromotion(bestMove)) {
		std::cout << "bestmove " << notation[getMoveSource(bestMove)] << notation[getMoveTarget(bestMove)] << valToPiece[getMoveIsPromotion(bestMove)] << "\nElapsed time: " << elapsed_seconds.count() << "s Nodes: " << nodes << " qNodes: " << qNodes << " Depth: " << depth << std::endl;
	}
	else {
		std::cout << "bestmove " << notation[getMoveSource(bestMove)] << notation[getMoveTarget(bestMove)] << "\nElapsed time: " << elapsed_seconds.count() << "s Nodes: " << nodes << " qNodes: " << qNodes << " Depth: " << depth << std::endl;
	}
	//std::cout << "eval: " << alpha << std::endl;
}
int negamax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta, bool tryNull) {
	int hashMove = -1;
	/*
	if (int val = probeHash(depth, alpha, beta, hashMove) != noEntry) {
		return val;
	}*/
	nodes++;
	repetition[zobristKey]++;
	if (repetition[zobristKey] == 3 || stop) { //threefold repetition or stop search due to time
		return 0;
	}
	Entry* temp = getEntry();
	if (temp != nullptr) {
		if (temp->depth >= depth && currAge == temp->age) {
			if (temp->flag == hashFlagExact) {
				repetition[zobristKey]--;
				return temp->value;
			}
			if (temp->flag == hashFlagBeta) {
				alpha = std::max(alpha, temp->value);
			}
			else if (temp->flag == hashFlagAlpha) {
				beta = std::min(beta, temp->value);
			}
			if (alpha >= beta) {
				repetition[zobristKey]--;
				return temp->value;
			}	
		}
		hashMove = temp->bestMove;
	}
	if (depth == 0) {
		int val = qSearch(color, whiteBoards, blackBoards, miscBoards, 6, board, alpha, beta);
		recordEntry(depth, val, hashFlagExact, -1);
		repetition[zobristKey]--;
		return val;
		/*
		int val = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		recordEntry(depth, val, hashFlagExact, -1);
		repetition[zobristKey]--;
		//return qSearch(color, whiteBoards, blackBoards, miscBoards, qDepth, board, alpha, beta);
		return val;*/
	}
	unsigned long kingBit;
	int newColor = color ^ 1;
	if (color == 1) {
		_BitScanForward64(&kingBit, whiteBoards[5]);
	}
	else {
		_BitScanForward64(&kingBit, blackBoards[5]);
	}
	
	uint64_t enPassant = miscBoards[2];
	if (tryNull && !isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards) && depth >= 3) { //null move, make sure to also check if no pieces
		miscBoards[2] = 0;
		int score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 3, board, -beta, -beta+1, false);
		miscBoards[2] = enPassant;
		if (score >= beta) {
			repetition[zobristKey]--;
			return beta;
		}
	}
	
	int hashF = hashFlagAlpha;
	int moves[256];
	int idx = 0;
	uint64_t castleRights = miscBoards[3];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, hashMove, depth);
	int score = 0;
	int bestMove = -1;
	bool moved = false;
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			moved = true;
			score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth-1, board, -beta, -alpha, true);
			if (score >= beta) {
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
				recordEntry(depth, beta, hashFlagBeta, moves[i]);
				for (int j = 0; j < 2; j++) {
					if (killers[depth][j] != moves[i]) {
						killers[depth][j] = moves[i];
						break;
					}
				}
				repetition[zobristKey]--;
				return beta;
			}
			if (score > alpha) {
				history[getMovePiece(moves[i])][getMoveTarget(moves[i])] += depth;
				bestMove = moves[i];
				hashF = hashFlagExact;
				alpha = score;
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	if (!moved) { //no moves in pos
		repetition[zobristKey]--;
		if (isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) { //checkmate
			return - depth - mateVal;
		}
		else { //stalemate
			recordEntry(depth, 0, hashFlagExact, 0);
			return 0;
		}
	}
	recordEntry(depth, alpha, hashF, bestMove);
	repetition[zobristKey]--;
	return alpha;
}

int qSearch(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta) {
	qNodes++;
	if (stop) {
		return 0;
	}
	unsigned long kingBit;
	if (depth == 0) {
		return evaluate(color, whiteBoards, blackBoards, miscBoards, board);
	}
	if (color == 1) {
		_BitScanForward64(&kingBit, whiteBoards[5]);
	}
	else {
		_BitScanForward64(&kingBit, blackBoards[5]);
	}
	if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
		int stand_pat = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		if (stand_pat >= beta) {
			return beta;
		}
		int delta = 1400;
		if (stand_pat < alpha - delta) {
			return alpha;
		}	
		if (alpha < stand_pat) {
			alpha = stand_pat;
		}
	}
	int moves[256];
	int idx = 0;
	int score = 0;
	int bestMove = -1;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	getCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortMoves(idx, moves, board, 0, depth);
	miscBoards[2] = 0;
	for (int i = 0; i < idx; i++) {
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			score = -qSearch(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, -beta, -alpha);
			if (score >= beta) {
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
				return beta;
			}
			if (score > alpha) {
				alpha = score;
			}
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
	}
	return alpha;
}



void sortMoves(int idx, int moves[], int board[], int hashMove, int depth) {
	int* moveScores = new int[idx];
	for (int i = 0; i < idx; i++) {
		moveScores[i] = scoreMove(moves[i], board, hashMove, depth);
	}
	quickSortMoves(moves, moveScores, 0, idx - 1);
	/*
	for (int i = 0; i < idx; i++) {
		cout << moveScores[i] << endl;
	}*/
	delete[] moveScores;
}
int scoreMove(int move, int board[], int hashMove, int depth) {
	if (move == hashMove) {
		return 100001;
	}
	if (getMoveIsPromotion(move) == Q || getMoveIsPromotion(move) == q) {
		return 100000;
	}
	if (getMoveIsCapture(move)) {
		if (pieceValues[board[getMoveSource(move)]] <= pieceValues[board[getMoveTarget(move)]]) { //winning or equal capture
			return 90000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]];
		}
		else {
			return 80000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]];
		}
	}
	if (move == killers[depth][0]) {
		return 70000;
	}
	if (move == killers[depth][1]) {
		return 69000;
	}
	return history[getMovePiece(move)][getMoveTarget(move)];
}

void printMoves(int idx, int moves[]) {
	for (int i = 0; i < idx; i++) {
		std::cout << "Source square: " << getMoveSource(moves[i]) << " End Square: " << getMoveTarget(moves[i]) << " Promotion: " << getMoveIsPromotion(moves[i]) << getMoveIsCapture(moves[i]) << " | " << getMoveIsCastling(moves[i]) << " | " << getMoveIsDoublePush(moves[i]) << " | " << getMoveIsEnPassant(moves[i]) << " | " << std::endl;
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

void collectPv(std::vector<int> &pvList, int depth, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int color) {
	if (depth == 0) {
		return;
	}
	uint64_t castleRights = miscBoards[3];
	int capturedPiece = 12; 
	unsigned long kingBit;
	Entry* temp = getEntry();
	pvList.push_back(temp->bestMove);
	int source = getMoveSource(temp->bestMove);
	int dest = getMoveTarget(temp->bestMove);
	int piece = getMovePiece(temp->bestMove);
	makeMove(temp->bestMove, board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
	collectPv(pvList, depth - 1, board, whiteBoards, blackBoards, miscBoards, color ^ 1);
	unMakeMove(temp->bestMove, board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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
}*/


