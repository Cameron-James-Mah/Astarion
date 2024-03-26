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


using namespace std;


/*
* int P = 1;
int N = 2;
int B = 3;
int R = 4;
int Q = 5;
int K = 6;
int p = 7;
int n = 8;
int b = 9;
int r = 10;
int q = 11;
int k = 12;
*/

//White = 1, Black = 0

int main()
{
    
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
    struct test {
        string fen;
        unsigned long result;
        int depth;
        test(string fen, int result, int depth) :fen(fen), result(result), depth(depth) {}
    };
    std::chrono::time_point<std::chrono::system_clock> start, end;
    pieceMaps();
    initZobrist();
    char board[64] = {
        'r','n','b','q','k','b','n','r',
        'p','p','p','p','p','p','p','p',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        'P','P','P','P','P','P','P','P',
        'R','N','B','Q','K','B','N','R'
    };
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
    string input;

    generateTables();
    initHashmap(128);
    while (getline(cin, input)) { //uci loop
        stringstream inputSS(input);
        string token;
        vector<string> tokens;
        while (getline(inputSS, token, ' ')) {
            tokens.push_back(token);
        }
        if (tokens[0] == "go") {
            start = std::chrono::system_clock::now();
            negamaxHelper(color, whiteBoards, blackBoards, miscBoards, 7, board2);
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            cout << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
            clearHashmap();
        }
        else if (tokens[0] == "uci") {
            cout << "uciok" << endl;
        }
        else if (tokens[0] == "isready") {
            cout << "readyok" << endl;
        }
        else if (tokens[0] == "position") {
            resetBoard(board, whiteBoards, blackBoards, miscBoards);
            if (tokens[1] == "fen") {
                string wholeFen = tokens[2] + " " + tokens[3] + " " + tokens[4];
                updateFromFen(board, wholeFen, miscBoards, color);
            }
            else if (tokens[1] == "startpos") {
                miscBoards[3] = 0b10010001'00000000'00000000'00000000'00000000'00000000'00000000'10010001;
                if (tokens.size() == 2) { //starting position
                    color = 1;
                    updateFromFen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq", miscBoards, color);
                }
                else {
                    //remember to account for repetition, castlerights
                    vector<string> moves(tokens.begin() + 3, tokens.end());
                    updateBoard(board, moves, miscBoards);
                    if (tokens.size() % 2 == 0) {
                        color = 0;
                    }
                    else {
                        color = 1;
                    }
                }
            }
            updateBitBoards(board, blackBoards, whiteBoards, miscBoards);
            for (int i = 0; i < 64; i++) {
                board2[i] = pieceToVal[board[i]];
            }
            computeZobrist(board2);
        }
        else if (tokens[0] == "p") {
            printBoard(board);
        }
        else if (tokens[0] == "cell") {
            cout << getCellNumber(tokens[1]) << endl;
        }
        else if (tokens[0] == "boards") {
            cout << "\tWhite Boards" << endl << endl;
            for (int i = 0; i < 7; i++) {
                printBitBoard(whiteBoards[i]);
            }
            cout << "\tBlack Boards" << endl << endl;
            for (int i = 0; i < 7; i++) {
                printBitBoard(blackBoards[i]);
            }
            cout << "\tMisc Boards" << endl << endl;
            for (int i = 0; i < 4; i++) {
                printBitBoard(miscBoards[i]);
            }
        }
        else if (tokens[0] == "attacks") {
            for (int i = 0; i < 64; i++) {
                cout << "Source: " << i << endl;
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
                cout << "Source square: " << getMoveSource(moves[i]) << " End Square: " << getMoveTarget(moves[i]) << " Promotion: " << getMoveIsPromotion(moves[i]) << getMoveIsCapture(moves[i]) << " | " << getMoveIsCastling(moves[i]) << " | " << getMoveIsDoublePush(moves[i]) << " | " << getMoveIsEnPassant(moves[i]) << " | " << endl;
                //cout << bitset<26>(moves[i]) << endl << bitset<26>(getMoveIsPromotion(moves[i])) << endl << endl;
            }
            cout << idx;
        }
        else if (tokens[0] == "atk") {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 64; j++) {
                    printBitBoard(rookAttacks[j][i]);
                }
                cout << "---------------------------------------------" << endl;
            }
        }
        else if (tokens[0] == "atk2") {
            for (int i = 0; i < 64; i++) {
                printBitBoard(pawnAttacksB[i]);
            }
        }
        else if (tokens[0] == "test") {
            /*
            unsigned long lsb;
            _BitScanReverse64(&lsb, 0b010000);
            cout << lsb;*/
            //cout << bitset<16>(0b010000);
            //printBoard2(board2);
            /*
            unsigned long lsb;
            _BitScanForward64(&lsb, whiteBoards[5]);
            cout << isSquareAttacked(lsb, color, whiteBoards, blackBoards, miscBoards);*/
            /*
            cout << sizeof(whiteBoards) << endl; //56 
            cout << sizeof(miscBoards) << endl; //32 
            cout << sizeof(board2) << endl;
            printBitBoard(notHGFile);
            printBoard2(board2);*/
            //printBitBoard(notHGFile);
            //printBitBoard(blackBoards[1]&notHGFile);
            //printBitBoard(bCastleRightsMask);
            vector<test> tests;
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
                resetBoard(board, whiteBoards, blackBoards, miscBoards);
                updateFromFen(board, tests[i].fen, miscBoards, color);
                updateBitBoards(board, blackBoards, whiteBoards, miscBoards);
                for (int i = 0; i < 64; i++) {
                    board2[i] = pieceToVal[board[i]];
                }
                unsigned long curr = perft(color, whiteBoards, blackBoards, miscBoards, tests[i].depth, board2);
                string res = "\n" + tests[i].fen + " Expected nodes: " + to_string(tests[i].result) + " Traversed nodes: " + to_string(curr) + " Depth: " + to_string(tests[i].depth);
                if (curr == tests[i].result)
                {
                    res += " TEST PASSED\n";
                }
                else
                {
                    res += " TEST FAILED\n";
                    passed = false;
                }
                cout << res;
            }
            if (passed) {
                cout << "\nALL TESTS PASSED\n";
            }
            else {
                cout << "\nTESTING FAILED\n";
            }
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            cout << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
        }
        else if (tokens[0] == "perft") { 
            //if tokens[1] is null then no perft
            start = std::chrono::system_clock::now();
            cout << endl << "Nodes: " << perftHelper(color, whiteBoards, blackBoards, miscBoards, stoi(tokens[1]), board2);
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            cout << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
            //perft(color, whiteBoards, blackBoards, miscBoards, stoi(tokens[1]), board2);
        }
        else if (tokens[0] == "eval") {
            cout << evaluate(color, whiteBoards, blackBoards, miscBoards, board2);
        }
        else if (tokens[0] == "hash") {
            cout << "Hash: " << zobristKey << endl;
        }
        else {
            cout << "Unknown Command" << endl;
        }
    }
}


