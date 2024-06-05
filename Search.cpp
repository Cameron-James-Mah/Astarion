#include <stdint.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>
#include <bitset>

#include "MoveGen.h"
#include "Board.h"
#include "Position.h"
#include "Globals.h"
#include "Evaluation.h"
#include "Search.h"
#include "Zobrist.h"
#include "TranspositionTable.h"
#include "Perft.h"

std::chrono::time_point<std::chrono::system_clock> start, end;
std::vector<int> pvList;
//int pvLength;
bool inPV;

void negamaxHelper(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], bool setDepth) {
	nodes++;
	int ply = 0;
	int moves[218];
	int idx = 0;
	int depth = 0;
	unsigned long kingBit = 100;
	int newColor = color ^ 1;
	uint64_t castleRights = miscBoards[3];
	//std::cout << repetition[zobristKey];
	getMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	//sortMoves(idx, moves, board, -1, depth);
	int bestMove = 0;
	int bestVal;
	int maxDepth = 50;
	if (setDepth) {
		maxDepth = 8;
	}
	start = std::chrono::system_clock::now();
	for (int j = 1; j < maxDepth+1; j++) {
		int currVal;
		int alpha = -infinity;
		int beta = infinity;
		int unmake = 0;
		int currBest = 0;
		currAge = j;
		int hashMove = 0;
		Entry* temp = getEntry();
		if (temp != nullptr) {
			hashMove = temp->bestMove;
		}
		sortMoves(idx, moves, board, hashMove, depth, ply, whiteBoards, blackBoards, miscBoards, color);
		/*
		if (hashMove != -1) {

		}*/
		inPV = true;
		for (int i = 0; i < idx; i++) {
			miscBoards[2] = 0;
			unmake = 0;
			int source = getMoveSource(moves[i]);
			int dest = getMoveTarget(moves[i]);
			int piece = getMovePiece(moves[i]);
			int capturedPiece = 12; //empty space
			makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
			//std::cout << zobristKey << std::endl;
			if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
				currVal = -negamax(newColor, whiteBoards, blackBoards, miscBoards, j - 1, board, -beta, -alpha, 1, 1);
				//std::cout << notation[getMoveSource(moves[i])] << notation[getMoveTarget(moves[i])] << std::endl;
				//std::cout << currVal << std::endl;
				if (currVal > alpha) {
					alpha = currVal;
					unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit); //unmake before becuase zobrist key updated in make/unmake
					unmake = 1;
					recordEntry(j, alpha, hashFlagAlpha, moves[i]);
					currBest = moves[i];
				}
				//std::cout << currVal << std::endl;
			}
			if (!unmake) { //I have to do this because i update my zobrist key in my make/unmake functions, and I have to update zobrist key before hashing
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit);
			}
			//unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
			if (stop) {
				break;
			}
			//std::cout << 123;
		}
		if (stop) {
			break;
		}
		bestVal = alpha;
		bestMove = currBest;
		depth=j;
		/*
		if (alpha >= mateVal || alpha <= -mateVal) { 
			break;
		}*/
		//collectPv(depth, board, whiteBoards, blackBoards, miscBoards, color);
		//std::cout << 1;
	}
	//collect pv
	/*
	collectPv(depth, board, whiteBoards, blackBoards, miscBoards, color);
	std::cout << "PV: ";
	for (int i = 0; i < depth; i++) {
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
	std::cout << "eval: " << bestVal << std::endl;
}
int negamax(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int depth, int board[], int alpha, int beta, int tryNull, int ply) {
	int hashMove = 0;
	nodes++;
	repetition[zobristKey]++;
	if (repetition[zobristKey] == 3 || stop) { //threefold repetition or stop search due to time
		repetition[zobristKey]--;
		return 0;
	}

	Entry* temp = getEntry();
	if (temp != nullptr) {
		if (temp->depth >= depth && currAge == temp->age) {
			if (temp->flag == hashFlagExact) {
				repetition[zobristKey]--;
				return temp->value;
			}
			else if (temp->flag == hashFlagBeta && temp->value >= beta) {
				repetition[zobristKey]--;
				return beta;
			}
			else if (temp->flag == hashFlagAlpha && temp->value <= alpha) {
				repetition[zobristKey]--;
				return alpha;
			}
			if (alpha >= beta) {
				repetition[zobristKey]--;
				return temp->value;
			}	
		}
		hashMove = temp->bestMove;
	}
	unsigned long kingBit = 100;
	int newColor = color ^ 1;
	if (color == 1) {
		_BitScanForward64(&kingBit, whiteBoards[5]);
	}
	else {
		_BitScanForward64(&kingBit, blackBoards[5]);
	}
	if (depth == 0) {
		inPV = false;
		//return evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		if(isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			depth++;
		}
		else {
			/*
			if (whiteBoards[5] == 0 || blackBoards[5] == 0) {
				printBoard2(board);
				//printBitBoard(miscBoards[2]);
			}*/
			int val = qSearch(color, whiteBoards, blackBoards, miscBoards, board, alpha, beta);
			recordEntry(depth, val, hashFlagExact, -1);
			repetition[zobristKey]--;
			return val;
		}
	}
	unsigned long kingBit2 = 100;
	if (color == 0) {
		_BitScanForward64(&kingBit2, whiteBoards[5]);
	}
	else {
		_BitScanForward64(&kingBit2, blackBoards[5]);
	}
	int staticEval;
	int futile = 0;
	//Checking if can futility prune 
	if (depth == 1) {
		staticEval = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		if ((staticEval + 200) < alpha) {
			futile = 1;
		}
	}


	
	int enoughMaterial = 0; 
	if (color == 1) {
		for (int i = 1; i < 5; i++) {
			std::bitset<64> b(whiteBoards[i]);
			enoughMaterial += b.count();
			if (enoughMaterial > 0) {
				break;
			}
		}
	}
	else {
		for (int i = 1; i < 5; i++) {
			std::bitset<64> b(blackBoards[i]);
			enoughMaterial += b.count();
			if (enoughMaterial > 0) {
				break;
			}
		}
	}
	
	uint64_t enPassant = miscBoards[2];
	if (tryNull == 1 && depth >= 4 && !isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards) && !inPV && enoughMaterial) { //null move, make sure to also check if no pieces
		miscBoards[2] = 0;
		int score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 4, board, -beta, -beta+1, 0, ply+4);
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
	sortMoves(idx, moves, board, hashMove, depth, ply, whiteBoards, blackBoards, miscBoards, color);
	int score = 0;
	int bestMove = -1;
	int moved = 0;
	int moves_searched = 0;
	bool isPV = false;
	
	for (int i = 0; i < idx; i++) {
		miscBoards[2] = 0;
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			moved = 1;
			//futility pruning
			if (futile && !getMoveIsCapture(moves[i]) && !isSquareAttacked(kingBit2, newColor, whiteBoards, blackBoards, miscBoards)) {
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit);
				continue;
			}
			enPassant = miscBoards[2];
			//score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, -beta, -alpha, 1, ply + 1);
			//LMR
			if (moves_searched < 2 || isPV) {
				score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, -beta, -alpha, 0, ply+1);
			}
			else {
				inPV = false;
				if (moves_searched >= 2 && depth >= 3 && getMoveIsCapture(moves[i]) == 0 && getMoveIsPromotion(moves[i]) == 0 && !isSquareAttacked(kingBit2, newColor, whiteBoards, blackBoards, miscBoards)) {
					score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 2, board, -(alpha+1), -alpha, 1, ply+2);
					miscBoards[2] = enPassant;
				}
				else {
					score = alpha + 1;
				}
				if (score > alpha) {
					score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, -(alpha+1), -alpha, 0, ply + 1);
					miscBoards[2] = enPassant;
					if (score > alpha && score < beta) { 
						score = -negamax(newColor, whiteBoards, blackBoards, miscBoards, depth - 1, board, -beta, -alpha, 0, ply + 1);
						miscBoards[2] = enPassant;
					}
				}
			}
			if (score >= beta) {
				unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit);
				recordEntry(depth, beta, hashFlagBeta, moves[i]);
				if (getMoveIsCapture(moves[i]) == 0) {
					for (int j = 0; j < 2; j++) {
						if (killers[depth][j] != moves[i]) {
							killers[depth][j] = moves[i];
							break;
						}
					}
				}
				repetition[zobristKey]--;
				return beta;
			}
			if (score > alpha) {
				if (getMoveIsCapture(moves[i]) == 0) {
					history[getMovePiece(moves[i])][getMoveTarget(moves[i])] += depth;
				}
				bestMove = moves[i];
				hashF = hashFlagExact;
				alpha = score;
				isPV = true;
				//recordEntry(depth, alpha, hashFlagAlpha, moves[i]);
			}
			moves_searched++;
		}
		unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit);
	}
	if (!moved) { //no moves in pos
		repetition[zobristKey]--;
		if (isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) { //checkmate
			recordEntry(depth, -depth-mateVal, hashFlagExact, 0);
			//std::cout << -depth - mateVal << std::endl;
			return - depth - mateVal;
		}
		else { //stalemate
			return 0;
		}
	}
	recordEntry(depth, alpha, hashF, bestMove);
	repetition[zobristKey]--;
	return alpha;
}

int qSearch(int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int board[], int alpha, int beta) {
	qNodes++;
	if (stop) {
		return 0;
	}
	unsigned long kingBit = 100;
	if (color == 1) {
		_BitScanForward64(&kingBit, whiteBoards[5]);
	}
	else {
		_BitScanForward64(&kingBit, blackBoards[5]);
	}
	/*
	if (whiteBoards[5] == 0 || blackBoards[5] == 0) {
		printBoard2(board);
		//printBitBoard(miscBoards[2]);
	}*/
	int inCheck = 1;
	if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
		int stand_pat = evaluate(color, whiteBoards, blackBoards, miscBoards, board);
		//std::cout << stand_pat << std::endl;
		if (stand_pat >= beta) {
			return beta;
		}
		/*
		int delta = 1025;
		if (stand_pat < alpha - delta ) {
			return alpha;
		}	*/
		if (alpha < stand_pat) {
			alpha = stand_pat;
		}
		inCheck = 0;
	}
	int moves[256];
	int idx = 0;
	int score = 0;
	int bestMove = -1;
	int newColor = color ^ 1;
	int moved = 0;
	getCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
	sortCaptures(idx, moves, board, whiteBoards, blackBoards, miscBoards, color);
	miscBoards[2] = 0;
	for (int i = 0; i < idx; i++) {
		int source = getMoveSource(moves[i]);
		int dest = getMoveTarget(moves[i]);
		
		if (pieceValues[board[source]] > pieceValues[board[dest]]) { 
			int captureVal = SEE(getMoveTarget(moves[i]), whiteBoards, blackBoards, miscBoards, board, moves[i], color, getMoveIsEnPassant(moves[i]));
			if (captureVal < 0) {
				continue;
			}
		}
		int piece = getMovePiece(moves[i]);
		int capturedPiece = 12; //empty space
		/*
		printBoard2(board);
		printBBs(whiteBoards);*/
		makeCapture(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
		/*
		if (whiteBoards[5] == 0 || blackBoards[5] == 0) {
			printBoard2(board);
			printBBs(blackBoards);
			unMakeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights);
			printBoard2(board);
			printBBs(blackBoards);
			makeMove(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
			//printBitBoard(miscBoards[2]);
		}*/
		if (!isSquareAttacked(kingBit, color, whiteBoards, blackBoards, miscBoards)) {
			moved = 1;
			score = -qSearch(newColor, whiteBoards, blackBoards, miscBoards, board, -beta, -alpha);
			if (score >= beta) {
				unMakeCapture(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color);
				return beta;
			}
			if (score > alpha) {
				alpha = score;
			}
		}
		unMakeCapture(moves[i], board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color);
		/*
		printBoard2(board);
		printBBs(whiteBoards);
		std::cout << std::endl << "------------------------------------------------------------" << std::endl;*/
	}

	//printBoard2(board);
	
	if (!moved && inCheck) {
		return -mateVal;
	}
	//std::cout << alpha << std::endl;
	return alpha;
}



void sortMoves(int idx, int moves[], int board[], int hashMove, int &depth, int &ply, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int &color) {
	int *moveScores = new int[idx];
	for (int i = 0; i < idx; i++) {
		moveScores[i] = scoreMove(moves[i], board, hashMove, depth, ply, whiteBoards, blackBoards, miscBoards, color);
	}
	quickSortMoves(moves, moveScores, 0, idx - 1);
	/*
	for (int i = 0; i < idx; i++) {
		cout << moveScores[i] << endl;
	}*/
	delete[] moveScores;
}
int scoreMove(int move, int board[], int hashMove, int depth, int ply, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int color) {
	/*
	if (pvLength != -1 && pvList[ply] == move) {
		return 150000;
	}*/
	if (move == hashMove) {
		return 100001;
	}
	if (getMoveIsPromotion(move) == Q || getMoveIsPromotion(move) == q) {
		return 100000;
	}
	if (getMoveIsCapture(move)) {
		
		if (getMoveIsEnPassant(move)) {
			return 90000;
		}
		if (pieceValues[board[getMoveSource(move)]] <= pieceValues[board[getMoveTarget(move)]]) { //winning or equal capture
			return 90000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]];
		}
		//return 65000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]];
		
		else {
			int captureVal = SEE(getMoveTarget(move), whiteBoards, blackBoards, miscBoards, board, move, color, getMoveIsEnPassant(move));
			if (captureVal >= 0) {
				//printBoard2(board);
				//std::cout << "From: " << notation[getMoveSource(move)] << " To: " << notation[getMoveTarget(move)] << "\n------------------------------------------------------";
				return 75000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]] + captureVal;
			}
			return 0;
		}
		//just realized not handling en passant
		/*
		int captureVal = SEE(getMoveTarget(move), whiteBoards, blackBoards, miscBoards, board, move, color, getMoveIsEnPassant(move));
		if (captureVal >= 0) {
			//printBoard2(board);
			//std::cout << "From: " << notation[getMoveSource(move)] << " To: " << notation[getMoveTarget(move)] << "\n------------------------------------------------------";
			return 75000 - pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]] + captureVal;
		}
		else {
			//printBoard2(board);
			//std::cout << "From: " << notation[getMoveSource(move)] << " To: " << notation[getMoveTarget(move)] << "\n------------------------------------------------------";
			return captureVal-1000;
		}*/
	}
	if (move == killers[depth][0]) {
		return 70000;
	}
	if (move == killers[depth][1]) {
		return 69000;
	}
	return history[getMovePiece(move)][getMoveTarget(move)];
}

void sortCaptures(int idx, int moves[], int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int &color) {
	int* moveScores = new int[idx];
	for (int i = 0; i < idx; i++) {
		moveScores[i] = scoreCapture(moves[i], board, whiteBoards, blackBoards, miscBoards, color);
	}
	quickSortMoves(moves, moveScores, 0, idx - 1);
	/*
	for (int i = 0; i < idx; i++) {
		cout << moveScores[i] << endl;
	}*/
	delete[] moveScores;
}

int scoreCapture(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int color) {
	return -pieceValues[board[getMoveSource(move)]] + pieceValues[board[getMoveTarget(move)]];
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

void collectPv(int depth, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int color) {
	if (depth == 0) {
		return;
	}
	uint64_t castleRights = miscBoards[3];
	int capturedPiece = 12; 
	unsigned long kingBit = 100;
	Entry* temp = getEntry();
	pvList.push_back(temp->bestMove);
	int source = getMoveSource(temp->bestMove);
	int dest = getMoveTarget(temp->bestMove);
	int piece = getMovePiece(temp->bestMove);
	makeMove(temp->bestMove, board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, kingBit);
	collectPv(depth - 1, board, whiteBoards, blackBoards, miscBoards, color ^ 1);
	unMakeMove(temp->bestMove, board, whiteBoards, blackBoards, miscBoards, capturedPiece, source, dest, piece, color, castleRights, kingBit);
}

/*
int getMaterialCount(uint64_t whiteBoards[], uint64_t blackBoards[]) {
	int material = 0;
	for (int i = 1; i < 4; i++) {
		std::bitset<64> b(whiteBoards[i]);
		material += getMaterialValueForTime(i) * b.count();
		std::bitset<64> b2(blackBoards[i]);
		material += getMaterialValueForTime(i + 6) * b2.count();
	}
	return material;
}*/

