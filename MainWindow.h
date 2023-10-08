#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);
    static void setBoard(int arr[][8]);
    void setSelectedSquare(int row, int col);
    static void setGameEnd(int ending);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // MAINWINDOW_H
