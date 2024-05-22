#include "Tests.h"
#include "Search.h"

std::vector<test> tests;
std::vector<test> testsSearch;

void initTests() {
	//perft tests
	tests.push_back(test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4865609, 5));
	tests.push_back(test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 97862, 3));
	tests.push_back(test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 674624, 5));
	tests.push_back(test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292, 5));
	tests.push_back(test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 2103487, 4));
	tests.push_back(test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ", 3894594, 4));
	tests.push_back(test("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 3605103, 5));


	//search tests
	testsSearch.push_back(test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4865609, 5));
	testsSearch.push_back(test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 97862, 3));
	testsSearch.push_back(test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 674624, 5));
	testsSearch.push_back(test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292, 5));
	testsSearch.push_back(test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 2103487, 4));
	testsSearch.push_back(test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ", 3894594, 4));
	testsSearch.push_back(test("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 3605103, 5));
}

