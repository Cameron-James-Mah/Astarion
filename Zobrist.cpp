#include <cstdint>
#include <type_traits>
#include <cstdint>
#include <random>
#include <iostream>

uint64_t zobristKey = 0;

uint64_t zobristTable[13][64];

uint64_t colorHash = 0;

std::random_device rd;
std::mt19937 gen(rd());

template <typename E>
uint64_t getRandomUINT64_t(E& engine) {
    std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max()
    );
    return dis(engine);
}


void initZobrist() {
    colorHash = getRandomUINT64_t(gen);
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 64; j++) {
            zobristTable[i][j] = getRandomUINT64_t(gen);
		}
	}
}

void computeZobrist(int board[]) {
    zobristKey = 0;
    for (int i = 0; i < 64; i++) {
        if (board[i] != 12) {
            zobristKey ^= zobristTable[board[i]][i];
        }
    }
}
