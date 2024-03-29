#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <intrin.h>


#include "Globals.h"
#include "Zobrist.h"
#include "MoveGen.h"

using namespace std;

void updateFromFen(char board[64], string fen);
void printBoard(char board[64]);
void updateBoard(char board[64], vector<string>& moves, uint64_t miscBoards[4]);
int getCellNumber(string cell);
void updateBitBoards(char board[64], uint64_t blackBoards[7], uint64_t whiteBoards[7], uint64_t miscBoards[4]);
void printBitBoard(uint64_t bitboard);
void resetBoard(char board[64], uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]);

void printBitBoard(uint64_t bitboard) {
    int col = 0;
    uint64_t temp = 1;
    for (int i = 0; i < 64; i++) {
        if (((temp << i) & bitboard) > 0) {
            cout << "1 ";
        }
        else {
            cout << "0 ";
        }
        col++;
        if (col == 8) {
            cout << endl;
            col = 0;
        }
    }
    cout << endl << endl;
}

void printBoard(char board[64]) {
    int col = 0;
    char letter = 'a';
    int row = 8;
    cout << endl << endl << "  ";
    for (int i = 0; i < 8; i++) {
        cout << "   " << letter;
        letter++;
    }
    cout << endl << " __";
    for (int j = 0; j < 8; j++) {
        cout << "|___";
    }
    cout << "|" << endl << " ";
    for (int i = 0; i < 64; i++) {
        if (col == 0) {
            cout << row;
            row--;
        }
        cout << " | " << board[i];
        col++;
        if (col == 8) {
            cout << " |" << endl << " __";
            for (int j = 0; j < 8; j++) {
                cout << "|___";
            }
            cout << "|" << endl << " ";
            col = 0;
        }
    }
    cout << endl << "Hash: " << zobristKey << endl;
}

void printBoard2(int board[64]) {
    int col = 0;
    char letter = 'a';
    int row = 8;
    cout << endl << endl << "  ";
    for (int i = 0; i < 8; i++) {
        cout << "   " << letter;
        letter++;
    }
    cout << endl << " __";
    for (int j = 0; j < 8; j++) {
        cout << "|___";
    }
    cout << "|" << endl << " ";
    for (int i = 0; i < 64; i++) {
        if (col == 0) {
            cout << row;
            row--;
        }
        cout << " | " << valToPiece[board[i]];
        col++;
        if (col == 8) {
            cout << " |" << endl << " __";
            for (int j = 0; j < 8; j++) {
                cout << "|___";
            }
            cout << "|" << endl << " ";
            col = 0;
        }
    }
    cout << endl << "Hash: " << zobristKey << endl;
}

void resetBoard(char board[64], uint64_t whiteBoards[7], uint64_t blackBoards[7], uint64_t miscBoards[4]) {
    char temp[64] = {
    'r','n','b','q','k','b','n','r',
    'p','p','p','p','p','p','p','p',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    'P','P','P','P','P','P','P','P',
    'R','N','B','Q','K','B','N','R'
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
void updateFromFen(char board[64], string fen, uint64_t miscBoards[], int &color) {
    int idx = 0;
    char temp[64] = {
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' ',
        ' ',' ',' ',' ',' ',' ',' ',' '
    };
    string otherInfo;
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
            temp[idx] = fen[i];
            idx++;
        }
    }
    if (otherInfo.find('w') != string::npos) {
        color = 1;
    }
    else {
        color = 0;
    }
    if (otherInfo.find('k') != string::npos) {
        miscBoards[3] |= (one << 4);
        miscBoards[3] |= (one << 0);
    }
    if (otherInfo.find('q') != string::npos) {
        miscBoards[3] |= (one << 4);
        miscBoards[3] |= (one << 7);
    }
    if (otherInfo.find('K') != string::npos) {
        miscBoards[3] |= (one << 60);
        miscBoards[3] |= (one << 56);
    }
    if (otherInfo.find('Q') != string::npos) {
        miscBoards[3] |= (one << 60);
        miscBoards[3] |= (one << 63);
    }
    memcpy(board, temp, sizeof(temp));
}

void updateBoard(char board[64], vector<string>& moves, uint64_t miscBoards[4]) {
    //special cases include: en passant, castling
    //otherwise just move piece to target location
    //miscBoards[3] = 0b10001001'00000000'00000000'00000000'00000000'00000000'00000000'10001001;
    for (int i = 0; i < moves.size(); i++) {
        miscBoards[2] = 0;
        string from = moves[i].substr(0, 2);
        string to = moves[i].substr(2, 2);
        if (board[getCellNumber(from)] == 'p' && board[getCellNumber(to)] == ' ') {
            if ((getCellNumber(to) + getCellNumber(from)) % 2 == 1) { //black en passant
                board[getCellNumber(to) - 8] = ' ';
            }
            else if (getCellNumber(to) - getCellNumber(from) == 16) { //double push, update en passant board
                miscBoards[2] ^= one << (getCellNumber(to) + 8);
            }
        }
        else if (board[getCellNumber(from)] == 'P' && board[getCellNumber(to)] == ' ') {
            if ((getCellNumber(to) + getCellNumber(from)) % 2 == 1) {//white en passant
                board[getCellNumber(to) + 8] = ' ';
            }
            else if (getCellNumber(from) - getCellNumber(to) == 16) { //double push, update en passant board
                miscBoards[2] ^= one << (getCellNumber(to) - 8);
            }
        }
        else if (board[getCellNumber(from)] == 'k' && getCellNumber(to) - getCellNumber(from) == 2 || board[getCellNumber(from)] == 'K' && getCellNumber(to) - getCellNumber(from) == 2) { //kingside castle
            board[getCellNumber(from) + 1] = board[getCellNumber(to) + 1];
            board[getCellNumber(to) + 1] = ' ';
            miscBoards[3] ^= one << (getCellNumber(from));
        }
        else if (board[getCellNumber(from)] == 'k' && getCellNumber(to) - getCellNumber(from) == -2 || board[getCellNumber(from)] == 'K' && getCellNumber(to) - getCellNumber(from) == -2) { //queenside castle
            board[getCellNumber(from) - 1] = board[getCellNumber(to) - 2];
            board[getCellNumber(to) - 2] = ' ';
            miscBoards[3] ^= one << (getCellNumber(from));
        }
        if (board[getCellNumber(from)] == 'p' && getCellNumber(to) >= 56 || board[getCellNumber(from)] == 'P' && getCellNumber(to) <= 7) { //promotion
            if (board[getCellNumber(from)] == 'P') {
                moves[i][4] = toupper(moves[i][4]);
            }
            board[getCellNumber(to)] = moves[i][4];
        }
        else {
            board[getCellNumber(to)] = board[getCellNumber(from)];
        }
        board[getCellNumber(from)] = ' ';
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
int getCellNumber(string cell) {
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
    uint64_t one = 1;
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
    uint64_t one = 1;
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
    zobristKey ^= colorHash;
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
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
}

void unMakeMove(int move, int board[], uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[], int& capturedPiece, int& source, int& dest, int& piece, int color, uint64_t castleRights) {
    zobristKey ^= colorHash;
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
            miscBoards[3] ^= one << source + 3;
        }
        else { //queenside castle
            zobristKey ^= zobristTable[board[source - 1]][source - 1];
            zobristKey ^= zobristTable[board[source - 1]][source - 4];
            addBitBoardPiece(board[source - 1], source - 4, whiteBoards, blackBoards, miscBoards);
            removeBitBoardPiece(board[source - 1], source - 1, whiteBoards, blackBoards, miscBoards);
            board[source - 4] = board[source - 1];
            board[source - 1] = 12;
            miscBoards[3] ^= one << source - 4;
        }
    }
    miscBoards[3] = castleRights;
    updateMiscBoards(whiteBoards, blackBoards, miscBoards);
}

void getMoves(int moves[], int& idx, int color, uint64_t whiteBoards[], uint64_t blackBoards[], uint64_t miscBoards[]) {
    getPawnMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKnightMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getBishopAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getRookAndQueenMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
    getKingMoves(moves, idx, color, whiteBoards, blackBoards, miscBoards);
}







