#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H

#include "main.h"

class Board {
private:
    int board[8][8];
    //Tracks if a jump is possible on this move, if so only add jumps to the moves list
    bool onlyJumps;
    //Turn: 0 - white, 1 - black
    int turn, whitePieces, blackPieces;
    //After 40 moves with no captures, game ends in a draw
    int movesWithoutCaptures;
    std::vector<move> allMoves, moves;

    void checkJump(int startR, int startC, int endR, int endC, int parent, int piece, std::vector<int> prevCheckedJumps);
    void checkJump(int startR, int startC, int endR, int endC);
    void checkMove(int startR, int startC, int endR, int endC);
    void findMovesForSquare(int row, int col);
    void findAllMoves();
    void isolateMovesForSquare(int row, int col);

public:
    Board(int b[][8]);
    Board(int b[][8], int t);

    bool click(int row, int col);
    int endTurn();
    void makeMove(move move);
    Board makeBoardForMove(move move);

    //Returns current possible moves based on the selected piece
    std::vector<move> getMoves();
    //Returns all possible moves in the current position
    std::vector<move> getAllMoves();
    int getTurn();

    int (*getBoard())[8];
};


#endif //PROJECT_BOARD_H
