#include <iostream>
#include <vector>
#include "Board.h"
#include "main.h"

void Board::checkJump(int startR, int startC, int endR, int endC, int parent, int piece, std::vector<int> prevCheckedJumps) {
    //Check out of bounds
    if(endR < 0 || endR > 7 || endC < 0 || endC > 7) return;

    //Check if this square has been checked for jumps previously
    //Prevents infinite loops when checking jumps for kings
    int sq = startR * 10 + startC;
    for(int square : prevCheckedJumps) {
        if(square == sq) return;
    }
    prevCheckedJumps.push_back(sq);

    int mid = board[(startR + endR) / 2][(startC + endC) / 2];
    if((mid % 2 != piece % 2 && mid != 0) && board[endR][endC] == 0) {
        move j{startR, startC, endR, endC, parent, true};

        if(parent != -1) {
            j.actStartR = allMoves.at(parent).actStartR;
            j.actStartC = allMoves.at(parent).actStartC;
        }

        allMoves.push_back(j);
        int index = allMoves.size() - 1;
        onlyJumps = true;

        //Check for double, triple, etc. jumps
        checkJump(endR, endC, endR + (endR - startR), endC + 2, index, piece, prevCheckedJumps);
        checkJump(endR, endC, endR + (endR - startR), endC - 2, index, piece, prevCheckedJumps);

        if(piece > 2) {
            //Check backwards jumps if piece is a king
            checkJump(endR, endC, endR - (endR - startR), endC + (endC - startC), index, piece, prevCheckedJumps);
        }
    }
}
void Board::checkJump(int startR, int startC, int endR, int endC) {
    std::vector<int> jumps;
    checkJump(startR, startC, endR, endC, -1, board[startR][startC], jumps);
}
void Board::checkMove(int startR, int startC, int endR, int endC) {
    //Check out of bounds
    if(endR < 0 || endR > 7 || endC < 0 || endC > 7) return;

    if(board[endR][endC] == 0) {
        if(!onlyJumps) {
            move m{startR, startC, endR, endC, -1, false};
            allMoves.push_back(m);
        }
    } else {
        int diffR = (endR - startR) * 2;
        int diffC = (endC - startC) * 2;

        checkJump(startR, startC, startR + diffR, startC + diffC);
    }
}
void Board::findMovesForSquare(int row, int col) {
    int piece = board[row][col];

    if(piece % 2 == 0 || piece > 2) {
        //Piece is white
        checkMove(row, col, row + 1, col + 1);
        checkMove(row, col, row + 1, col - 1);
    }
    if(piece % 2 == 1 || piece > 2) {
        //Piece is black
        checkMove(row, col, row - 1, col + 1);
        checkMove(row, col, row - 1, col - 1);
    }
}
void Board::findAllMoves() {
    //Cycle through the board and find all possible moves
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            int piece = board[i][j];
            if(piece != 0 && piece % 2 == turn) {
                findMovesForSquare(i, j);
            }
        }
    }

    if(onlyJumps) {
        int removed = 0, index = 0;

        while(index < allMoves.size()) {
            if(!allMoves.at(index).jump) {
                allMoves.erase(allMoves.begin() + index);
                removed++;
            } else {
                allMoves.at(index).parent -= removed;
                index++;
            }
        }
    }
}

//Finds all moves in 'allMoves' that originate at row, col and moves them to 'moves' to be displayed on the board
void Board::isolateMovesForSquare(int row, int col) {
    moves.clear();
    if(board[row][col] != 0 && board[row][col] % 2 == turn) {
        for(move m : allMoves) {
            if(m.actStartR == row && m.actStartC == col) {
                moves.push_back(m);
            }
        }
    }
}

void Board::makeMove(move move) {
    if(move.jump) {
        movesWithoutCaptures = 0;

        if(move.parent >= 0) {
            makeMove(allMoves.at(move.parent));
        }

        board[(move.endR + move.startR) / 2][(move.endC + move.startC) / 2] = 0;
        if(turn == 0) {
            blackPieces--;
        } else {
            whitePieces--;
        }
    } else {
        movesWithoutCaptures++;
    }

    int piece = board[move.startR][move.startC];
    board[move.endR][move.endC] = piece;
    //Check if piece should become a king
    if(((piece % 2 == 0 && move.endR == 7) || move.endR == 0) && piece < 3) {
        board[move.endR][move.endC] += 2;
    }
    board[move.startR][move.startC] = 0;
}

//Create a copy of this board and make the given move, must call endTurn() on the returned Board
//Used by the bot for evaluation of future positions
Board Board::makeBoardForMove(move move) {
    Board output = Board(board, turn);
    output.makeMove(move);
    return output;
}

//Returns the outcome of the game after this move
//0: white victory, 1: black victory, 2: draw
int Board::endTurn() {
    moves.clear();
    allMoves.clear();
    onlyJumps = false;

    if(turn == 0) {
        turn = 1;
    } else {
        turn = 0;
    }

    //Victory condition if either side runs out of pieces
    if(blackPieces == 0) {
        return 0;
    }
    if(whitePieces == 0) {
        return 1;
    }

    //Game ends in draw after 40 moves with no captures
    if(movesWithoutCaptures == 40) {
        return 2;
    }

    findAllMoves();

    //If player cannot make any moves, then the opponent wins
    if(allMoves.size() == 0) {
        if(turn == 1) return 0;
        else return 1;
    }

    return -1;
}

//Returns whether a move was made or not
bool Board::click(int row, int col) {
    for(const auto& m : moves) {
        if(m.endR == row && m.endC == col) {
            makeMove(m);
            return true;
        }
    }

    isolateMovesForSquare(row, col);
    return false;
}

std::vector<move> Board::getMoves() {
    return moves;
}
std::vector<move> Board::getAllMoves() {
    return allMoves;
}
int Board::getTurn() {
    return turn;
}

int (*Board::getBoard())[8] {
    return board;
}

Board::Board(int (*b)[8], int t) {
    onlyJumps = false;
    turn = t;
    blackPieces = 0;
    whitePieces = 0;
    movesWithoutCaptures = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int piece = b[i][j];
            board[i][j] = piece;

            if(piece != 0 && piece % 2 == 0) {
                whitePieces++;
            } else {
                blackPieces++;
            }
        }
    }

    findAllMoves();
}
Board::Board(int b[][8]) : Board(b, 1) {}
