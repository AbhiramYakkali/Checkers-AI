#include <iostream>
#include <vector>
#include <QApplication>
#include "Bot.h"
#include <chrono>

int positions = 0;
int overallBestEval = 1000;
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

    if(color == 0) {
        return eval * -1;
    } else {
        return eval;
    }
}

int search(Board board, int depth, bool max, int color) {
    int outcome = board.endTurn();
    if(outcome != -1) {
        if(outcome == 2) return 0;
        if(outcome == color) {
            return 100;
        } else {
            return -100;
        }
    }

    std::vector<move> moves = board.getAllMoves();
    int bestEval = 1000;

    for(auto m : moves) {
        int eval;
        if(depth == 0) {
            eval = evaluate(board.makeBoardForMove(m).getBoard(), color);
        } else {
            eval = search(board.makeBoardForMove(m), depth - 1, !max, color);
        }

        if(bestEval == 1000 || (eval > bestEval && max) || (eval < bestEval && !max)) {
            bestEval = eval;
        }
        if(overallBestEval == 1000 || eval > overallBestEval) {
            overallBestEval = eval;
        }

        //Alpha-beta pruning
        if(!max && eval < overallBestEval) return eval;
    }

    return bestEval;
}
move findBestMove(Board board, int color) {
    std::vector<move> moves = board.getAllMoves();
    positions = 0;
    startTime = std::chrono::high_resolution_clock::now();

    move bestMove = moves.at(0);
    int bestEval = search(board.makeBoardForMove(bestMove), 12, false, color);

    for(int i = 1; i < moves.size(); i++) {
        move m = moves.at(i);
        int eval = search(board.makeBoardForMove(m), 12, false, color);

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