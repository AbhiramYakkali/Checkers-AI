#include "mainwindow.h"
#include <windows.h>
#include "main.h"
#include <QDebug>
#include <iostream>

const int HEIGHT = 1080, WIDTH = 1920;
const int vertOffset = 8;
const int sqLength = (HEIGHT - vertOffset * 2) / 8;
const int horizStart = (WIDTH - (sqLength * 8)) / 2;

const QColor white("#EEEED2");
const QColor whitePiece("#F8F8F8");
const QColor black("#769656");
const QColor blackPiece("#565352");
const QColor bg("#312E2B");
const QColor red("#ff5252");

int b[8][8];
int sR = -1, sC;
//0: white win, 1: black win, 2: draw
int end = -1;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setWindowState(Qt::WindowMaximized);
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    //Set appropriate font size for displaying kings
    QFont font = painter.font();
    font.setPointSize(25);
    painter.setFont(font);

    //Filling background with white
    painter.setBrush(bg);
    painter.setPen(bg);
    painter.drawRect(0, 0, 1920, 1080);

    //Draw the board
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(i == sR && j == sC) {
                painter.setBrush(red);
                painter.setPen(red);
            } else if((i + j) % 2 == 0) {
                painter.setBrush(white);
                painter.setPen(white);
            } else {
                painter.setBrush(black);
                painter.setPen(black);
            }

            painter.drawRect(horizStart + sqLength * j, vertOffset + sqLength * i, sqLength, sqLength);

            if(b[i][j] != 0) {
                //There is a piece on this square that must be drawn
                int piece = b[i][j];
                if(piece % 2 == 0) {
                    painter.setBrush(whitePiece);
                    painter.setPen(bg);
                } else {
                    painter.setBrush(blackPiece);
                    painter.setPen(bg);
                }
                painter.drawEllipse(QPoint(horizStart + sqLength * j + sqLength / 2, vertOffset + sqLength * i + sqLength / 2), sqLength / 3, sqLength / 3);

                if(piece > 2) {
                    //Piece is a king
                    if(piece % 2 == 0) {
                        painter.setPen(blackPiece);
                    } else {
                        painter.setPen(whitePiece);
                    }

                    int centerX = horizStart + sqLength * j + sqLength / 2;
                    int centerY = vertOffset + sqLength * i + sqLength * 15 / 16;
                    int textX = centerX - painter.fontMetrics().horizontalAdvance("K") / 2;
                    int textY = centerY - painter.fontMetrics().height();
                    painter.drawText(textX, textY, "K");
                }
            }
        }
    }

    //Draw the possible moves(if any)
    painter.setBrush(red);
    painter.setPen(red);
    for(struct::move m : main::getMoves()) {
        painter.drawEllipse(QPoint(horizStart + sqLength * m.endC + sqLength / 2, vertOffset + sqLength * m.endR + sqLength / 2), sqLength / 3, sqLength / 3);
    }

    if(end != -1) {
        painter.setPen(white);

        if(end == 0) {
            painter.drawText(horizStart / 3, HEIGHT / 2, "white wins");
        } else if(end == 1) {
            painter.drawText(horizStart / 3, HEIGHT / 2, "black wins");
        } else {
            painter.drawText(horizStart / 3, HEIGHT / 2, "draw");
        }
    }
}

void MainWindow::setSelectedSquare(int row, int col) {
    sR = row;
    sC = col;

    repaint();
}

void MainWindow::setGameEnd(int ending) {
    end = ending;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    int x = event->position().x();
    int y = event->position().y();

    int col = (x - horizStart) / sqLength;
    int row = (y - vertOffset) / sqLength;

    if(col > -1 && col < 8 && row > -1 && row < 8) {
        main::mouseClick(row, col);
        repaint();
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::setBoard(int arr[][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            b[i][j] = arr[i][j];
        }
    }
}
