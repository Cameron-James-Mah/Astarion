#include <cstdint>
#include <iostream>
#include "TranspositionTable.h"
#include "Zobrist.h"

int hashmapSize = 0;
Entry* hashmap = nullptr;

int currAge = 0;

void clearHashmap() {
	Entry* temp = nullptr;
	for (temp = hashmap; temp < hashmap + hashmapSize; temp++) {
		temp->key = 0;
		temp->depth = 0;
		temp->flag = 0;
		temp->value = 0;
		temp->bestMove = 0;
		temp->age = 0;
	}
}

void initHashmap(int mb) {
	int size = 0x100000 * mb;
	hashmapSize = size / sizeof(Entry);
	if (hashmap != nullptr) {
		delete[] hashmap;
	}
	hashmap = new Entry[hashmapSize];
	if (hashmap == nullptr) { //not enough available memory
		initHashmap(mb / 2);
		std::cout << "Resizing hashtable with size: " << hashmapSize/2 << std::endl;
	}
	else {
		clearHashmap();
		std::cout << "Hashmap init with size: " << hashmapSize << std::endl;
	}
}

//return tt table value, dont need to pass color as param since it is in zobristKey
int probeHash(int depth, int alpha, int beta, int &bestMove) {
	Entry* hashEntry = &hashmap[zobristKey % hashmapSize];
	if (hashEntry->key == zobristKey) {
		if (hashEntry->depth >= depth) {
			int score = hashEntry->value; //why not just return score here? 
			if (hashEntry->flag == hashFlagExact) {
				return score;
			}
			if (hashEntry->flag == hashFlagAlpha && score <= alpha) {
				return alpha;
			}
			if (hashEntry->flag == hashFlagBeta && score >= beta) {
				return beta;
			}
			//return score;
		}
		bestMove = hashEntry->bestMove;
	}
	return noEntry;
}

void recordEntry(int depth, int val, int hashFlag, int bestMove) {
	Entry* newEntry = &hashmap[zobristKey % hashmapSize];
	/*
	if (newEntry->depth >= depth) { //dont overwrite entry
		return;
	}*/
	if (currAge > newEntry->age || newEntry->depth <= depth) {
		newEntry->key = zobristKey;
		newEntry->depth = depth;
		newEntry->flag = hashFlag;
		newEntry->value = val;
		newEntry->bestMove = bestMove;
		newEntry->age = currAge;
	}
}

Entry* getEntry() {
	Entry* hashEntry = &hashmap[zobristKey % hashmapSize];
	if (hashEntry->key == zobristKey) {
		return hashEntry;
	}
	return nullptr;
}