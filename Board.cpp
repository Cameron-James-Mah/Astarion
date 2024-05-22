#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <intrin.h>
#include <bitset>
#include <cctype>


#include "Globals.h"
#include "Zobrist.h"
#include "MoveGen.h"
#include "Board.h"
#include "AttackTables.h"


void updateFromFen(int board[64], std::string fen, uint64_t miscBoards[], int& color);
void printBoard(char board[64]);
void updateBoard(int board[64], std::vector<std::string>& moves, uint64_t miscBoards[4]);
int getCellNumber(std::string cell);
void updateBitBoards(char board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]);
void printBitBoard(uint64_t bitboard);
void resetBoard(int board[64], uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);
int getMaterialValueForTime(int piece);

void printBitBoard(uint64_t bitboard) {
    int col = 0;
    uint64_t temp = 1;
    for (int i = 0; i < 64; i++) {
        if (((temp << i) & bitboard) > 0) {
            std::cout << "1 ";
        }
        else {
            std::cout << "0 ";
        }
        col++;
        if (col == 8) {
            std::cout << std::endl;
            col = 0;
        }
    }
    std::cout << std::endl << std::endl;
}

void printBoard(char board[64]) {
    int col = 0;
    char letter = 'a';
    int row = 8;
    std::cout << std::endl << std::endl << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << "   " << letter;
        letter++;
    }
    std::cout << std::endl << " __";
    for (int j = 0; j < 8; j++) {
        std::cout << "|___";
    }
    std::cout << "|" << std::endl << " ";
    for (int i = 0; i < 64; i++) {
        if (col == 0) {
            std::cout << row;
            row--;
        }
        std::cout << " | " << board[i];
        col++;
        if (col == 8) {
            std::cout << " |" << std::endl << " __";
            for (int j = 0; j < 8; j++) {
                std::cout << "|___";
            }
            std::cout << "|" << std::endl << " ";
            col = 0;
        }
    }
    std::cout << std::endl << "Hash: " << zobristKey << std::endl;
}

void printBoard2(int board[64]) {
    int col = 0;
    char letter = 'a';
    int row = 8;
    std::cout << std::endl << std::endl << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << "   " << letter;
        letter++;
    }
    std::cout << std::endl << " __";
    for (int j = 0; j < 8; j++) {
        std::cout << "|___";
    }
    std::cout << "|" << std::endl << " ";
    for (int i = 0; i < 64; i++) {
        if (col == 0) {
            std::cout << row;
            row--;
        }
        std::cout << " | " << valToPiece[board[i]];
        col++;
        if (col == 8) {
            std::cout << " |" << std::endl << " __";
            for (int j = 0; j < 8; j++) {
                std::cout << "|___";
            }
            std::cout << "|" << std::endl << " ";
            col = 0;
        }
    }
    std::cout << std::endl << "Hash: " << zobristKey << std::endl;
}

void resetBoard(int board[64], uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]) {
    int temp[64] = {
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12
    };
    for (int i = 0; i < 64; i++) {
        board[i] = temp[i];
    }
    for (int i = 0; i < 7; i++) {
        whiteBoards[i] = 0;
        blackBoards[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
        miscBoards[i] = 0;
    }
}
void updateFromFen(int board[64], std::string fen, uint64_t miscBoards[], int &color) {
    int idx = 0;
    int temp[64] = {
        12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12,
    12,12,12,12,12,12,12,12
    };
    std::string otherInfo;
    for (int i = 0; i < fen.length(); i++) {
        if (fen[i] == ' ') {
            otherInfo = fen.substr(i);
            break;
        }
        if (isdigit(fen[i])) {
            idx += fen[i] - '0';
        }
        else if (fen[i] == '/') {
            continue;
        }
        else {
            temp[idx] = pieceToVal[fen[i]];
            idx++;
        }
    }
    if (otherInfo.find('w') != std::string::npos) {
        color = 1;
    }
    else {
        color = 0;
    }
    if (otherInfo.find('k') != std::string::npos) {
        miscBoards[3] |= (one << 4);
        miscBoards[3] |= (one << 0);
    }
    if (otherInfo.find('q') != std::string::npos) {
        miscBoards[3] |= (one << 4);
        miscBoards[3] |= (one << 7);
    }
    if (otherInfo.find('K') != std::string::npos) {
        miscBoards[3] |= (one << 60);
        miscBoards[3] |= (one << 56);
    }
    if (otherInfo.find('Q') != std::string::npos) {
        miscBoards[3] |= (one << 60);
        miscBoards[3] |= (one << 63);
    }
    memcpy(board, temp, sizeof(temp));
}

void updateBoard(int board[64], std::vector<std::string>& moves, uint64_t miscBoards[4], int color) {
    computeZobrist(board, color, miscBoards);
    repetition[zobristKey]++;
    for (int i = 0; i < moves.size(); i++) {
        miscBoards[2] = 0;
        std::string from = moves[i].substr(0, 2);
        std::string to = moves[i].substr(2, 2);
        if (board[getCellNumber(from)] == p && board[getCellNumber(to)] == 12) {
            if ((getCellNumber(to) + getCellNumber(from)) % 2 == 1) { //black en passant
                board[getCellNumber(to) - 8] = 12;
            }
            else if (getCellNumber(to) - getCellNumber(from) == 16) { //double push, update en passant board
                miscBoards[2] ^= one << (getCellNumber(to) - 8);
            }
        }
        else if (board[getCellNumber(from)] == P && board[getCellNumber(to)] == 12) {
            if ((getCellNumber(to) + getCellNumber(from)) % 2 == 1) {//white en passant
                board[getCellNumber(to) + 8] = 12;
            }
            else if (getCellNumber(from) - getCellNumber(to) == 16) { //double push, update en passant board
                miscBoards[2] ^= one << (getCellNumber(to) + 8);
            }
        }
        else if (board[getCellNumber(from)] == k && getCellNumber(to) - getCellNumber(from) == 2 || board[getCellNumber(from)] == K && getCellNumber(to) - getCellNumber(from) == 2) { //kingside castle
            board[getCellNumber(from) + 1] = board[getCellNumber(to) + 1];
            board[getCellNumber(to) + 1] = 12;
            miscBoards[3] ^= one << (getCellNumber(from));
        }
        else if (board[getCellNumber(from)] == k && getCellNumber(to) - getCellNumber(from) == -2 || board[getCellNumber(from)] == K && getCellNumber(to) - getCellNumber(from) == -2) { //queenside castle
            board[getCellNumber(from) - 1] = board[getCellNumber(to) - 2];
            board[getCellNumber(to) - 2] = 12;
            miscBoards[3] ^= one << (getCellNumber(from));
        }
        if (board[getCellNumber(from)] == p && getCellNumber(to) >= 56) { //promotion
            board[getCellNumber(to)] = pieceToVal[tolower(moves[i][4])];
        }
        else if (board[getCellNumber(from)] == P && getCellNumber(to) <= 7) {
            board[getCellNumber(to)] = pieceToVal[toupper(moves[i][4])];
        }
        else {
            board[getCellNumber(to)] = board[getCellNumber(from)];
        }
        board[getCellNumber(from)] = 12;
        zobristKey ^= blackHash;
        computeZobrist(board, color, miscBoards);
        repetition[zobristKey]++;
        
    }
}

void updateBitBoards(char board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]) {
    uint64_t temp = 1;
    for (int i = 0; i < 64; i++) {
        switch (board[i]) {
        case 'P':
            whiteBoards[0] |= temp << i;
            break;
        case 'N':
            whiteBoards[1] |= temp << i;
            break;
        case 'B':
            whiteBoards[2] |= temp << i;
            break;
        case 'R':
            whiteBoards[3] |= temp << i;
            break;
        case 'Q':
            whiteBoards[4] |= temp << i;
            break;
        case 'K':
            whiteBoards[5] |= temp << i;
            break;
        case 'p':
            blackBoards[0] |= temp << i;
            break;
        case 'n':
            blackBoards[1] |= temp << i;
            break;
        case 'b':
            blackBoards[2] |= temp << i;
            break;
        case 'r':
            blackBoards[3] |= temp << i;
            break;
        case 'q':
            blackBoards[4] |= temp << i;
            break;
        case 'k':
            blackBoards[5] |= temp << i;
            break;
        }
    }
    whiteBoards[6] = whiteBoards[5] | whiteBoards[4] | whiteBoards[3] | whiteBoards[2] | whiteBoards[1] | whiteBoards[0];
    blackBoards[6] = blackBoards[5] | blackBoards[4] | blackBoards[3] | blackBoards[2] | blackBoards[1] | blackBoards[0];
    miscBoards[0] = whiteBoards[6] | blackBoards[6];
    miscBoards[1] = ~miscBoards[0];
    //en passant and castlerights should be updated when parsing startpos moves to board
}

void updateBitBoards2(int board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]) {
    uint64_t temp = 1;
    blackBoards[0] = 0; blackBoards[1] = 0; blackBoards[2] = 0; blackBoards[3] = 0; blackBoards[4] = 0; blackBoards[5] = 0; blackBoards[6] = 0;
    whiteBoards[0] = 0; whiteBoards[1] = 0; whiteBoards[2] = 0; whiteBoards[3] = 0; whiteBoards[4] = 0; whiteBoards[5] = 0; whiteBoards[6] = 0;
    for (int i = 0; i < 64; i++) {
        switch (board[i]) {
        case P:
            whiteBoards[0] |= temp << i;
            break;
        case N:
            whiteBoards[1] |= temp << i;
            break;
        case B:
            whiteBoards[2] |= temp << i;
            break;
        case R:
            whiteBoards[3] |= temp << i;
            break;
        case Q:
            whiteBoards[4] |= temp << i;
            break;
        case K:
            whiteBoards[5] |= temp << i;
            break;
        case p:
            blackBoards[0] |= temp << i;
            break;
        case n:
            blackBoards[1] |= temp << i;
            break;
        case b:
            blackBoards[2] |= temp << i;
            break;
        case r:
            blackBoards[3] |= temp << i;
            break;
        case q:
            blackBoards[4] |= temp << i;
            break;
        case k:
            blackBoards[5] |= temp << i;
            break;
        }
    }
    whiteBoards[6] = whiteBoards[5] | whiteBoards[4] | whiteBoards[3] | whiteBoards[2] | whiteBoards[1] | whiteBoards[0];
    blackBoards[6] = blackBoards[5] | blackBoards[4] | blackBoards[3] | blackBoards[2] | blackBoards[1] | blackBoards[0];
    miscBoards[0] = whiteBoards[6] | blackBoards[6];
    miscBoards[1] = ~miscBoards[0];
    //en passant and castlerights should be updated when parsing startpos moves to board
}

//notation to cell number in board array
int getCellNumber(std::string cell) {
    int row = -1;
    int col = -1;
    switch (cell[0]) {
    case 'a':
        col = 0;
        break;
    case 'b':
        col = 1;
        break;
    case 'c':
        col = 2;
        break;
    case 'd':
        col = 3;
        break;
    case 'e':
        col = 4;
        break;
    case 'f':
        col = 5;
        break;
    case 'g':
        col = 6;
        break;
    case 'h':
        col = 7;
        break;
    }
    row = 8 - (cell[1] - '0');
    return row * 8 + col;
}

void addBitBoardPiece(int piece, int dest, uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]) {
    switch (piece) {
    case P:
        whiteBoards[0] |= one << dest;
        break;
    case N:
        whiteBoards[1] |= one << dest;
        break;
    case B:
        whiteBoards[2] |= one << dest;
        break;
    case R:
        whiteBoards[3] |= one << dest;
        break;
    case Q:
        whiteBoards[4] |= one << dest;
        break;
    case K:
        whiteBoards[5] |= one << dest;
        break;
    case p:
        blackBoards[0] |= one << dest;
        break;
    case n:
        blackBoards[1] |= one << dest;
        break;
    case b:
        blackBoards[2] |= one << dest;
        break;
    case r:
        blackBoards[3] |= one << dest;
        break;
    case q:
        blackBoards[4] |= one << dest;
        break;
    case k:
        blackBoards[5] |= one << dest;
        break;
    }
}

void removeBitBoardPiece(int piece, int dest, uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]) {
    switch (piece) {
    case P:
        whiteBoards[0] ^= one << dest;
        break;
    case N:
        whiteBoards[1] ^= one << dest;
        break;
    case B:
        whiteBoards[2] ^= one << dest;
        break;
    case R:
        whiteBoards[3] ^= one << dest;
        break;
    case Q:
        whiteBoards[4] ^= one << dest;
        break;
    case K:
        whiteBoards[5] ^= one << dest;
        break;
    case p:
        blackBoards[0] ^= one << dest;
        break;
    case n:
        blackBoards[1] ^= one << dest;
        break;
    case b:
        blackBoards[2] ^= one << dest;
        break;
    case r:
        blackBoards[3] ^= one << dest;
        break;
    case q:
        blackBoards[4] ^= one << dest;
        break;
    case k:
        blackBoards[5] ^= one << dest;
        break;
    }

}

void updateMiscBoards(uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]) {
    whiteBoards[6] = whiteBoards[5] | whiteBoards[4] | whiteBoards[3] | whiteBoards[2] | whiteBoards[1] | whiteBoards[0];
    blackBoards[6] = blackBoards[5] | blackBoards[4] | blackBoards[3] | blackBoards[2] | blackBoards[1] | blackBoards[0];
    miscBoards[0] = whiteBoards[6] | blackBoards[6];
    miscBoards[1] = ~miscBoards[0];
}

void makeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, unsigned long& kingBit) {
    if (getMoveIsCapture(move)) {
        if (getMoveIsEnPassant(move)) {
            if (color == 1) {
                capturedPiece = p;
                zobristKey ^= zobristTable[capturedPiece][dest+8];
                removeBitBoardPiece(board[dest + 8], dest + 8, whiteBoards, blackBoards, miscBoards);
                board[dest + 8] = 12;
            }
            else {
                capturedPiece = P;
                zobristKey ^= zobristTable[capturedPiece][dest - 8];
                removeBitBoardPiece(board[dest - 8], dest - 8, whiteBoards, blackBoards, miscBoards);
                board[dest - 8] = 12;
            }
        }
        else {
            zobristKey ^= zobristTable[board[dest]][dest];
            capturedPiece = board[dest];
            removeBitBoardPiece(capturedPiece, dest, whiteBoards, blackBoards, miscBoards);
        }
    }
    if (getMoveIsDoublePush(move)) {
        if (color == 1) {
            miscBoards[2] ^= one << (dest + 8);
        }
        else {
            miscBoards[2] ^= one << (dest - 8);
        }
        //printBitBoard(miscBoards[2]);
        if (color == 1 && enPassantAttacks[dest] & blackBoards[0]) {
            updateEnPassantHash(source);
        }
        else if (color == 0 && enPassantAttacks[dest] & whiteBoards[0]) {
            updateEnPassantHash(source);
        }
    }
    zobristKey ^= zobristTable[piece][source];
    board[source] = 12;
    removeBitBoardPiece(piece, source, whiteBoards, blackBoards, miscBoards);
    if (getMoveIsCastling(move)) {
        if (dest > source) { //kingside castle
            zobristKey ^= zobristTable[board[source+3]][source+1];
            zobristKey ^= zobristTable[board[source + 3]][source + 3];
            addBitBoardPiece(board[source + 3], source + 1, whiteBoards, blackBoards, miscBoards);
            removeBitBoardPiece(board[source + 3], source + 3, whiteBoards, blackBoards, miscBoards);
            board[source + 1] = board[source + 3];
            board[source + 3] = 12;
            //miscBoards[3] ^= one << source + 3;
        }
        else { //queenside castle
            zobristKey ^= zobristTable[board[source - 4]][source - 1];
            zobristKey ^= zobristTable[board[source - 4]][source - 4];
            addBitBoardPiece(board[source - 4], source - 1, whiteBoards, blackBoards, miscBoards);
            removeBitBoardPiece(board[source - 4], source - 4, whiteBoards, blackBoards, miscBoards);
            board[source - 1] = board[source - 4];
            board[source - 4] = 12;
            //miscBoards[3] ^= one << source - 4;
        }
    }

    if (getMoveIsPromotion(move)) {
        zobristKey ^= zobristTable[getMoveIsPromotion(move)][dest];
        board[dest] = getMoveIsPromotion(move);
        addBitBoardPiece(getMoveIsPromotion(move), dest, whiteBoards, blackBoards, miscBoards);
    }
    else {
        zobristKey ^= zobristTable[piece][dest];
        board[dest] = piece;
        addBitBoardPiece(piece, dest, whiteBoards, blackBoards, miscBoards);
    }

    if (piece == K) {
        miscBoards[3] |= wCastleRightsMask;
        miscBoards[3] ^= wCastleRightsMask;
        //printBitBoard(miscBoards[3]);
    }
    else if (piece == k) {
        miscBoards[3] |= bCastleRightsMask;
        miscBoards[3] ^= bCastleRightsMask;
    }
    else if (piece == R && (one << source) & miscBoards[3] || piece == r && (one << source) & miscBoards[3]) { //moved rook for first time
        miscBoards[3] ^= one << source;
    }
    if (color == 1) {
        _BitScanForward64(&kingBit, whiteBoards[5]);
    }
    else {
        _BitScanForward64(&kingBit, blackBoards[5]);
    }
    zobristKey ^= blackHash;
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
}

void unMakeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, uint64_t castleRights, unsigned long &kingBit) {
    zobristKey ^= zobristTable[piece][source];
    board[source] = piece;
    addBitBoardPiece(piece, source, whiteBoards, blackBoards, miscBoards);
    if (board[dest] != 12) {
        zobristKey ^= zobristTable[board[dest]][dest];
        board[dest] = 12;
    }
    if (getMoveIsPromotion(move)) {
        removeBitBoardPiece(getMoveIsPromotion(move), dest, whiteBoards, blackBoards, miscBoards);
    }
    else {
        removeBitBoardPiece(piece, dest, whiteBoards, blackBoards, miscBoards);
    }
    if (capturedPiece < 12) {
        if (getMoveIsEnPassant(move)) {
            if (color == 1) {
                zobristKey ^= zobristTable[capturedPiece][dest+8];
                board[dest + 8] = capturedPiece;
                addBitBoardPiece(capturedPiece, dest + 8, whiteBoards, blackBoards, miscBoards);
            }
            else {
                zobristKey ^= zobristTable[capturedPiece][dest - 8];
                board[dest - 8] = capturedPiece;
                addBitBoardPiece(capturedPiece, dest - 8, whiteBoards, blackBoards, miscBoards);
            }
        }
        else {
            zobristKey ^= zobristTable[capturedPiece][dest];
            board[dest] = capturedPiece;
            addBitBoardPiece(capturedPiece, dest, whiteBoards, blackBoards, miscBoards);
        }
    }
    if (getMoveIsCastling(move)) {
        if (dest > source) { //kingside castle
            addBitBoardPiece(board[source + 1], source + 3, whiteBoards, blackBoards, miscBoards);
            removeBitBoardPiece(board[source + 1], source + 1, whiteBoards, blackBoards, miscBoards);
            zobristKey ^= zobristTable[board[source + 1]][source + 1];
            zobristKey ^= zobristTable[board[source + 1]][source + 3];
            board[source + 3] = board[source + 1];
            board[source + 1] = 12;
            miscBoards[3] ^= one << (source + 3);
        }
        else { //queenside castle
            zobristKey ^= zobristTable[board[source - 1]][source - 1];
            zobristKey ^= zobristTable[board[source - 1]][source - 4];
            addBitBoardPiece(board[source - 1], source - 4, whiteBoards, blackBoards, miscBoards);
            removeBitBoardPiece(board[source - 1], source - 1, whiteBoards, blackBoards, miscBoards);
            board[source - 4] = board[source - 1];
            board[source - 1] = 12;
            miscBoards[3] ^= one << (source - 4);
        }
    }
    if (getMoveIsDoublePush(move)) {
        if (color == 1 && enPassantAttacks[dest] & blackBoards[0]) {
            updateEnPassantHash(source);
        }
        else if (color == 0 && enPassantAttacks[dest] & whiteBoards[0]) {
            updateEnPassantHash(source);
        }
        
    }
    miscBoards[3] = castleRights;
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
    zobristKey ^= blackHash;
    if (color == 1) {
        _BitScanForward64(&kingBit, whiteBoards[5]);
    }
    else {
        _BitScanForward64(&kingBit, blackBoards[5]);
    }
}

void makeCapture(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, unsigned long& kingBit) {
    if (getMoveIsEnPassant(move)) {
        if (color == 1) {
            capturedPiece = p;
            removeBitBoardPiece(board[dest + 8], dest + 8, whiteBoards, blackBoards, miscBoards);
            board[dest + 8] = 12;
        }
        else {
            capturedPiece = P;
            removeBitBoardPiece(board[dest - 8], dest - 8, whiteBoards, blackBoards, miscBoards);
            board[dest - 8] = 12;
        }
    }
    else {
        capturedPiece = board[dest];
        removeBitBoardPiece(capturedPiece, dest, whiteBoards, blackBoards, miscBoards);
    }
    board[source] = 12;
    removeBitBoardPiece(piece, source, whiteBoards, blackBoards, miscBoards);
    if (getMoveIsPromotion(move)) {
        board[dest] = getMoveIsPromotion(move);
        addBitBoardPiece(getMoveIsPromotion(move), dest, whiteBoards, blackBoards, miscBoards);
    }
    else {
        board[dest] = piece;
        addBitBoardPiece(piece, dest, whiteBoards, blackBoards, miscBoards);
    }
    if (color == 1) {
        _BitScanForward64(&kingBit, whiteBoards[5]);
    }
    else {
        _BitScanForward64(&kingBit, blackBoards[5]);
    }
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
}
void unMakeCapture(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color) {
    board[source] = piece;
    addBitBoardPiece(piece, source, whiteBoards, blackBoards, miscBoards);
    if (getMoveIsPromotion(move)) {
        removeBitBoardPiece(getMoveIsPromotion(move), dest, whiteBoards, blackBoards, miscBoards);
    }
    else {
        removeBitBoardPiece(piece, dest, whiteBoards, blackBoards, miscBoards);
    }
    if (getMoveIsEnPassant(move)) {
        if (color == 1) {
            board[dest + 8] = capturedPiece;
            addBitBoardPiece(capturedPiece, dest + 8, whiteBoards, blackBoards, miscBoards);
        }
        else {
            board[dest - 8] = capturedPiece;
            addBitBoardPiece(capturedPiece, dest - 8, whiteBoards, blackBoards, miscBoards);
        }
    }
    else {
        board[dest] = capturedPiece;
        addBitBoardPiece(capturedPiece, dest, whiteBoards, blackBoards, miscBoards);
    }
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
}

void getMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    getPawnMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKnightMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getBishopAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getRookAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKingMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
}

void getCaptures(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    getPawnCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKnightCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getBishopAndQueenCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getRookAndQueenCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKingCaptures(moves, idx, color, whiteBoards, blackBoards, miscBoards);
}

void clearKillers() {
    for (int i = 0; i < 30; i++) {
        killers[i][0] = 0;
        killers[i][1] = 0;
    }
}

void clearHistory() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            history[i][j] = 0;
        }
    }
}

int getMaterialCount(uint64_t whiteBoards[], uint64_t blackBoards[]) {
    int material = 0;
    int currPiece = P;
    for (int i = 0; i < 5; i++) {
        std::bitset<64> b(whiteBoards[i]);
        material += getMaterialValueForTime(currPiece) * (int)b.count();
        std::bitset<64> b2(blackBoards[i]);
        material += getMaterialValueForTime(currPiece+6) * (int)b2.count();
        currPiece++;
    }
    return material;
}

int estimatedHalfMoves(int material) {
    int res = 0;
    if (material < 2000)
    {
        res = material / 100 + 10;
    }
    else if (material <= 6000)
    {
        res = ((material / 100) * 3) / 8 + 22;
    }
    else
    {
        res = material / 100 * 5 / 4 - 30;
    }
    if (75 < res)
    {
        return 75;
    }
    return res;
}

int getMaterialValueForTime(int piece) {
    switch (piece)
    {
    case P: case p:
        return 100;
    case N: case n:
        return 420;
    case B: case b:
        return 480;
    case R: case r:
        return 750;
    case Q: case q:
        return 1400;
    default:
        return 0;
    }
}

void updateEnPassantHash(int source) {
    switch (source) {
    case 48: case 8:
        zobristKey ^= enPassantHash[0];
        break;
    case 49: case 9:
        zobristKey ^= enPassantHash[1];
        break;
    case 50: case 10:
        zobristKey ^= enPassantHash[2];
        break;
    case 51: case 11:
        zobristKey ^= enPassantHash[3];
        break;
    case 52: case 12:
        zobristKey ^= enPassantHash[4];
        break;
    case 53: case 13:
        zobristKey ^= enPassantHash[5];
        break;
    case 54: case 14:
        zobristKey ^= enPassantHash[6];
        break;
    case 55: case 15:
        zobristKey ^= enPassantHash[7];
        break;
    }
    /*
    if (miscBoards[2] & AFile) {
        zobristKey ^= enPassantHash[0];
    }
    else if (miscBoards[2] & BFile) {
        zobristKey ^= enPassantHash[1];
    }
    else if (miscBoards[2] & CFile) {
        zobristKey ^= enPassantHash[2];
    }
    else if (miscBoards[2] & DFile) {
        zobristKey ^= enPassantHash[3];
    }
    else if (miscBoards[2] & EFile) {
        zobristKey ^= enPassantHash[4];
    }
    else if (miscBoards[2] & FFile) {
        zobristKey ^= enPassantHash[5];
    }
    else if (miscBoards[2] & GFile) {
        zobristKey ^= enPassantHash[6];
    }
    else if (miscBoards[2] & HFile) {
        zobristKey ^= enPassantHash[7];
    }*/
}




