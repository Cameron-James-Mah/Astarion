#include <cstdint>

const int hashFlagExact = 0;
const int hashFlagAlpha = 1;
const int hashFlagBeta = 2;

const int hashFlagEmpty = -1;

const int noEntry = 99999;

extern int currAge;

struct Entry {
	uint64_t key;
	uint8_t depth;
	uint8_t flag;
	int value;
	int bestMove;
	uint8_t age;
};

extern int hashmapSize;
extern Entry* hashmap;

int probeHash(int depth, int alpha, int beta, int &bestMove);
void recordEntry(int depth, int val, int hashFlag, int bestMove);
void initHashmap(int mb);
void clearHashmap();
Entry* getEntry();