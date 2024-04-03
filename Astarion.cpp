// Astarion.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
    position fen r1bqkb1r/ppp2ppp/2np1n2/4p3/2B1P3/2NP1N2/PPP2PPP/R1BQK2R b KQkq - 0 5
    position startpos moves d2d4 e7e6 g1f3 b8c6 c1f4 d7d5 e2e3 f8b4 c2c3 b4e7 f1d3 h7h6
*/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <bitset>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <thread>
#include <future>

#include "AttackTables.h"
#include "MoveGen.h"
#include "Board.h"
#include "Perft.h"
#include "Globals.h"
#include "Position.h"
#include "Evaluation.h"
#include "Search.h"
#include "Zobrist.h"
#include "TranspositionTable.h"

//using namespace std;

//White = 1, Black = 0

void sendCommand(std::chrono::seconds delay)
{
    std::this_thread::sleep_for(delay);
    stop = true;
}

int main()
{
    struct test {
        std::string fen;
        unsigned long result;
        int depth;
        test(std::string fen, int result, int depth) :fen(fen), result(result), depth(depth) {}
    };
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int board2[64];
    
    //Placeholder variables
    uint64_t bPawn = 0;
    uint64_t bKnight = 0;
    uint64_t bBishop = 0;
    uint64_t bRook = 0;
    uint64_t bQueen = 0;
    uint64_t bKing = 0;

    uint64_t wPawn = 0;
    uint64_t wKnight = 0;
    uint64_t wBishop = 0;
    uint64_t wRook = 0;
    uint64_t wQueen = 0;
    uint64_t wKing = 0;

    uint64_t whitePieces = 0;
    uint64_t blackPieces = 0;

    uint64_t allPieces = 0;
    uint64_t empty = 0;

    uint64_t enPassant = 0;
    uint64_t castleRights = 0;

    uint64_t blackBoards[7]; blackBoards[0] = bPawn; blackBoards[1] = bKnight; blackBoards[2] = bBishop; blackBoards[3] = bRook; blackBoards[4] = bQueen; blackBoards[5] = bKing; blackBoards[6] = blackPieces;
    uint64_t whiteBoards[7]; whiteBoards[0] = wPawn; whiteBoards[1] = wKnight; whiteBoards[2] = wBishop; whiteBoards[3] = wRook; whiteBoards[4] = wQueen; whiteBoards[5] = wKing; whiteBoards[6] = whitePieces;
    uint64_t miscBoards[4]; miscBoards[0] = allPieces; miscBoards[1] = empty; miscBoards[2] = enPassant; miscBoards[3] = castleRights;

    int color = 1;
    std::string input;
    pieceMaps(); 
    initZobrist(); //zobrist table
    generateTables(); //attack tables
    initTables(); //pestos psq tables
    clearHashmap(); //tt table
    clearHistory(); //history table
    clearKillers(); //killer moves table
    initHashmap(256);
    while (std::getline(std::cin, input)) { //uci loop
        std::stringstream inputSS(input);
        std::string token;
        std::vector<std::string> tokens;
        while (getline(inputSS, token, ' ')) {
            tokens.push_back(token);
        }
        if (tokens[0] == "go") {
            float time;
            stop = false;
            bool setDepth = false;
            nodes = 0;
            qNodes = 0;
            if (tokens.size() > 1)
            {
                if (color == 0)
                {
                    time = std::stoi(tokens[4]);
                }
                else
                {
                    time = std::stoi(tokens[2]);;
                }
                int material = getMaterialCount(whiteBoards, blackBoards);
                //Console.WriteLine(time);
                time = time / estimatedHalfMoves(material);
                if (time <= 0)
                {
                    time = 100;
                }
                time /= 1000;
            }
            else { //if time not specified
                setDepth = true;
            }
            if (!setDepth) {
                auto s1 = std::async(std::launch::async, sendCommand, std::chrono::seconds((int)time));
                negamaxHelper(color, whiteBoards, blackBoards, miscBoards, board2, setDepth);
            }
            else {
                negamaxHelper(color, whiteBoards, blackBoards, miscBoards, board2, setDepth);
            }
            
            
            clearHashmap();
            clearKillers();
            clearHistory();
            nodes = 0;
            repetition.clear();
        }
        else if (tokens[0] == "uci") {
            std::cout << "id name " << "Astarion_V1" << std::endl;
            std::cout << "id author " << "Cameron Mah" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (tokens[0] == "position") {
            resetBoard(board2, whiteBoards, blackBoards, miscBoards);
            if (tokens[1] == "fen") {
                std::string wholeFen = tokens[2] + " " + tokens[3] + " " + tokens[4];
                updateFromFen(board2, wholeFen, miscBoards, color);
                auto it = find(tokens.begin(), tokens.end(), "moves");
                if (it != tokens.end()) {
                    _int64 index = it - tokens.begin();
                    std::vector<std::string> moves(tokens.begin() + index, tokens.end());
                    updateBoard(board2, moves, miscBoards);
                    if ((tokens.size()-index) % 2 == 0) {
                        color ^= 1;
                    }
                }
            }
            else if (tokens[1] == "startpos") {
                miscBoards[3] = 0b10010001'00000000'00000000'00000000'00000000'00000000'00000000'10010001;
                updateFromFen(board2, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq", miscBoards, color);
                if (tokens.size() == 2) { //starting position
                    color = 1;
                }
                else {
                    //remember to account for repetition, castlerights
                    std::vector<std::string> moves(tokens.begin() + 3, tokens.end());
                    updateBoard(board2, moves, miscBoards);
                    if (tokens.size() % 2 == 0) {
                        color = 0;
                    }
                    else {
                        color = 1;
                    }
                }
            }
            updateBitBoards2(board2, blackBoards, whiteBoards, miscBoards);
            /*
            for (int i = 0; i < 64; i++) {
                board2[i] = pieceToVal[board[i]];
            }*/
            computeZobrist(board2);
        }
        else if (tokens[0] == "p") {
            printBoard2(board2);
        }
        else if (tokens[0] == "cell") {
            std::cout << getCellNumber(tokens[1]) << std::endl;
        }
        else if (tokens[0] == "boards") {
            std::cout << "\tWhite Boards" << std::endl << std::endl;
            for (int i = 0; i < 7; i++) {
                printBitBoard(whiteBoards[i]);
            }
            std::cout << "\tBlack Boards" << std::endl << std::endl;
            for (int i = 0; i < 7; i++) {
                printBitBoard(blackBoards[i]);
            }
            std::cout << "\tMisc Boards" << std::endl << std::endl;
            for (int i = 0; i < 4; i++) {
                printBitBoard(miscBoards[i]);
            }
        }
        else if (tokens[0] == "attacks") {
            for (int i = 0; i < 64; i++) {
                std::cout << "Source: " << i << std::endl;
                printBitBoard(pawnAttacksB[i]);
            }
            /*
            for (int i = 0; i < 64; i++) {
                for (int j = 0; j < 4; j++) {
                    cout << "Source: " << i << endl;
                    printBitBoard(rookAttacks[i][j]);
                }
            }*/
        }
        else if (tokens[0] == "mvs") {
            int idx = 0;
            int moves[256];
            getPawnMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            getKnightMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            getBishopAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            getRookAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            getKingMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            for (int i = 0; i < idx; i++) {
                std::cout << "Source square: " << getMoveSource(moves[i]) << " End Square: " << getMoveTarget(moves[i]) << " Promotion: " << getMoveIsPromotion(moves[i]) << getMoveIsCapture(moves[i]) << " | " << getMoveIsCastling(moves[i]) << " | " << getMoveIsDoublePush(moves[i]) << " | " << getMoveIsEnPassant(moves[i]) << " | " << std::endl;
                //cout << bitset<26>(moves[i]) << endl << bitset<26>(getMoveIsPromotion(moves[i])) << endl << endl;
            }
            std::cout << idx;
        }
        else if (tokens[0] == "atk") {
            int idx = 0;
            int moves[256];
            getCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
            for (int i = 0; i < idx; i++) {
                std::cout << "Source square: " << getMoveSource(moves[i]) << " End Square: " << getMoveTarget(moves[i]) << " Promotion: " << std::endl;
            }
            std::cout << "Total Attacks: " << idx << std::endl;
        }
        else if (tokens[0] == "atkTables") {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 64; j++) {
                    printBitBoard(rookAttacks[j][i]);
                }
                std::cout << "---------------------------------------------" << std::endl;
            }
            for (int i = 0; i < 64; i++) {
                printBitBoard(pawnAttacksB[i]);
            }
        }
        else if (tokens[0] == "test") {
            std::vector<test> tests;
            bool passed = true;
            tests.push_back(test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4865609, 5));
            tests.push_back(test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 97862, 3));
            tests.push_back(test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 674624, 5));
            tests.push_back(test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292, 5));
            tests.push_back(test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 2103487, 4));
            tests.push_back(test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ", 3894594, 4));
            tests.push_back(test("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 3605103, 5));
            for (int i = 0; i < tests.size(); i++) {
                start = std::chrono::system_clock::now();
                resetBoard(board2, whiteBoards, blackBoards, miscBoards);
                updateFromFen(board2, tests[i].fen, miscBoards, color);
                updateBitBoards2(board2, blackBoards, whiteBoards, miscBoards);
                unsigned long curr = perft(color, whiteBoards, blackBoards, miscBoards, tests[i].depth, board2);
                std::string res = "\n" + tests[i].fen + " Expected nodes: " + std::to_string(tests[i].result) + " Traversed nodes: " + std::to_string(curr) + " Depth: " + std::to_string(tests[i].depth);
                if (curr == tests[i].result)
                {
                    res += " TEST PASSED\n";
                }
                else
                {
                    res += " TEST FAILED\n";
                    passed = false;
                }
                std::cout << res;
            }
            if (passed) {
                std::cout << "\nALL TESTS PASSED\n";
            }
            else {
                std::cout << "\nTESTING FAILED\n";
            }
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
        }
        else if (tokens[0] == "perft") { 
            start = std::chrono::system_clock::now();
            std::cout << std::endl << "Nodes: " << perftHelper(color, whiteBoards, blackBoards, miscBoards, stoi(tokens[1]), board2);
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
        }
        else if (tokens[0] == "eval") {
            std::cout << evaluate(color, whiteBoards, blackBoards, miscBoards, board2);
        }
        else if (tokens[0] == "hash") {
            std::cout << "Hash: " << zobristKey << std::endl;
        }
        else {
            std::cout << "Unknown Command" << std::endl;
        }
    }
}



