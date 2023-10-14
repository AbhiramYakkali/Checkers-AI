#include <vector>
#include <QApplication>
#include "Bot.h"
#include <chrono>

//Depth that the bot will search to evaluate positions
//If this number is odd the bot will maximize the last checked move
//If this number is even the bot will minimize the last checked move
#define DEFAULT_DEPTH 11

int positions = 0;
auto startTime = std::chrono::high_resolution_clock::now();

//Returns a long representing the current board
long generateBoardState(int board[][8]) {
    long boardState = 0;

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            //Skip empty squares
            if((i + j) % 2 == 0) continue;

            boardState *= 10;
            boardState += board[i][j];
        }
    }

    return boardState;
}

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
int search(Board board, int depth, int alpha, int beta, bool max, int color, std::unordered_map<long, int>* transpositionTable) {
    //Check transposition table to see if this position was searched previously
    long boardState = generateBoardState(board.getBoard());
    if(transpositionTable->count(boardState) != 0) {
        return transpositionTable->at(boardState);
    }

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
            eval = search(board.makeBoardForMove(m), depth - 1, alpha, beta, !max, color, transpositionTable);

            //Update alpha and beta if better moves are found
            if(max && eval > alpha) alpha = eval;
            if(!max && eval < beta) beta = eval;

            //Alpha-beta pruning
            if(beta <= alpha) break;
        }
    }

    if(max) {
        transpositionTable->insert({boardState, alpha});
        return alpha;
    }
    else {
        transpositionTable->insert({boardState, beta});
        return beta;
    }
}
//Returns the best move the bot found
//This function maximizes (finds best move for the bot based on all options)
//Minimizing functions find worst move for the bot (ie. best move for the opponent)
move findBestMove(Board board, int color) {
    std::vector<move> moves = board.getAllMoves();
    positions = 0;
    //Keep track of time (to print out how long it took to find best move)
    startTime = std::chrono::high_resolution_clock::now();

    //Initialize transposition table
    //Maps board positions to previously found evals to eliminate searching of identical positions
    std::unordered_map<long, int> transpositionTable;

    //Keep track of the best move found
    move bestMove = moves.at(0);
    int bestEval = search(board.makeBoardForMove(bestMove), DEFAULT_DEPTH, -1000, 1000, false, color, &transpositionTable);

    //Iterate through all possible moves, performing minmax search on each
    for(int i = 1; i < moves.size(); i++) {
        move m = moves.at(i);
        int eval = search(board.makeBoardForMove(m), DEFAULT_DEPTH, -1000, 1000, false, color, &transpositionTable);

        if(eval > bestEval) {
            bestEval = eval;
            bestMove = m;
        } else if(eval == bestEval && rand() / (RAND_MAX / 2) == 1) {
            //Introduce randomness to move selection in case of several equal moves
            //This is in an attempt to reduce back and forth shuffling of pieces in equal endgames
            //Also helps to make each game more unique
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