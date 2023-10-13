#include <iostream>
#include <vector>
#include <QApplication>
#include "Bot.h"
#include <chrono>

//Depth that the bot will search to evaluate positions
#define DEFAULT_DEPTH 10

int positions = 0;
auto startTime = std::chrono::high_resolution_clock::now();

//Generates an evaluation of the position based on the board
//Positive numbers: black advantage, negative: white advantage
//Always returns a positive number for ease of comparison
int evaluate(int board[][8], int color) {
    int eval = 0;
    positions++;

    //Go through the board and add up number of pieces for each side, counting kings as 2
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            int piece = board[i][j];

            if(piece != 0) {
                int value;

                if(piece % 2 == 0) value = -1;
                else value = 1;

                if(piece > 2) value *= 2;

                eval += value;
            }
        }
    }

    //Return positive number for ease of comparison
    if(color == 0) {
        return eval * -1;
    } else {
        return eval;
    }
}

//Alpha: best move that bot (maximizing player) is guaranteed in this branch
//Beta: best move that player (minimizing player) is guaranteed in this branch
int search(Board board, int depth, int alpha, int beta, bool max, int color) {
    int outcome = board.endTurn();
    //See if the game ends after this move and assign values accordingly
    if(outcome != -1) {
        if(outcome == 2) return 0;
        if(outcome == color) {
            return 100;
        } else {
            return -100;
        }
    }

    std::vector<move> moves = board.getAllMoves();

    //Iterate through all possible moves and perform minmax search
    for(auto m : moves) {
        int eval;
        if(depth == 0) {
            //If depth = 0, evaluate the board at this position and return
            return evaluate(board.makeBoardForMove(m).getBoard(), color);
        } else {
            eval = search(board.makeBoardForMove(m), depth - 1, alpha, beta, !max, color);

            //Update alpha and beta if better moves are found
            if(max && eval > alpha) alpha = eval;
            if(!max && eval < beta) beta = eval;

            //Pruning (comment out line to disable pruning for testing purposes)
            if(beta <= alpha) break;
        }
    }

    if(max) return alpha;
    else return beta;
}
//Returns the best move the bot found
//This function maximizes (finds best move for the bot based on all options)
//Minimizing functions find worst move for the bot (ie. best move for the opponent)
move findBestMove(Board board, int color) {
    std::vector<move> moves = board.getAllMoves();
    positions = 0;
    //Keep track of time (to print out how long it took to find best move)
    startTime = std::chrono::high_resolution_clock::now();

    //Keep track of the best move found
    move bestMove = moves.at(0);
    int bestEval = search(board.makeBoardForMove(bestMove), DEFAULT_DEPTH, -1000, 1000, false, color);

    //Iterate through all possible moves, performing minmax search on each
    for(int i = 1; i < moves.size(); i++) {
        move m = moves.at(i);
        int eval = search(board.makeBoardForMove(m), DEFAULT_DEPTH, -1000, 1000, false, color);

        if(eval > bestEval) {
            bestEval = eval;
            bestMove = m;
        }
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
    qDebug() << "Positions evaluated: " << positions << ", spent " << elapsed/std::chrono::milliseconds(1) <<  "milliseconds, best position: " << bestEval;
    return bestMove;
}

//Based on the board given, returns what it thinks is the best move
move Bot::makeMove(Board board) {
    return findBestMove(board, 0);
}