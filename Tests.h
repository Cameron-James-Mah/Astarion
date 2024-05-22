#include <vector>
#include <string>


struct test {
    std::string fen;
    unsigned long result;
    int depth;
    test(std::string fen, int result, int depth) :fen(fen), result(result), depth(depth) {}
};

extern std::vector<test> tests;
extern std::vector<test> testsSearch;

void initTests();

