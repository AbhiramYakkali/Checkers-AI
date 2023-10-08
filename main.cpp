#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "main.h"
#include "board.h"
#include "bot.h"

//1 - black, 2 - white, 3 - black king, 4 - white king
int startingBoard[8][8] = {{0, 2, 0, 2, 0, 2, 0, 2},
                   {2, 0, 2, 0, 2, 0, 2, 0},
                   {0, 2, 0, 2, 0, 2, 0, 2},
                   {0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0},
                   {1, 0, 1, 0, 1, 0, 1, 0},
                   {0, 1, 0, 1, 0, 1, 0, 1},
                   {1, 0, 1,0, 1, 0, 1, 0}};
/*int startingBoard[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 2, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 2, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 2, 0, 0},
                   {0, 0, 0, 0, 0, 0, 1, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0}};*/
/*int startingBoard[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 2, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 1, 0, 0, 0, 0},
                           {0, 0, 0, 0, 1, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0}};*/

Board board(startingBoard);
bool playing = true;
bool playingWithAI = true;
//0: white, 1: black
int turn = 1;

MainWindow *mw;

void main::mouseClick(int row, int col) {
    if(playing) {
        //board.click() returns whether or not a move was made
        if(board.click(row, col)) {
            endTurn();
        } else {
            mw->setSelectedSquare(row, col);
        }
    }
}

void main::endTurn() {
    int outcome = board.endTurn();
    MainWindow::setBoard(board.getBoard());
    mw->setSelectedSquare(-1, -1);

    if(outcome != -1) {
        MainWindow::setGameEnd(outcome);
        playing = false;
        return;
    }

    if(turn == 1) {
        turn = 0;
    } else {
        turn = 1;
    }

    if(playingWithAI && turn == 0) {
        board.makeMove(Bot::makeMove(board));
        endTurn();
    }
}

std::vector<move> main::getMoves() {
    return board.getMoves();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mw = &mainWindow;
    mainWindow.setBoard(startingBoard);
    mainWindow.show();

    return app.exec();
}
