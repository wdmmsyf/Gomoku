#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    game = new Game();
    game->initGame(15);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    setMouseTracking(true);
    QPainter pa(this);

    //设置窗口固定大小
    setFixedSize(440, 440);

    //画网格线
    for(int i = 0; i < game->size; i++){
        pa.drawLine(10 + 30 * i, 10, 10 + 30 * i, size().height() - 10);
        pa.drawLine(10, 10 + 30 * i, size().width() - 10, 10 + 30 * i);
    }

    //绘制鼠标停留位置的灰色棋子图标
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    if(mousePosRow >= 0 && mousePosLin >= 0 &&
        mousePosRow < game->size && mousePosLin < game->size &&
        game->chessBoard[mousePosRow][mousePosLin] == 0){
        brush.setColor(Qt::gray);
        pa.setBrush(brush);
        pa.drawEllipse(30 * mousePosRow, 30 * mousePosLin, 20, 20);
    }

    //绘制当前局面棋盘
    for(int i = 0; i < game->size; i++){
        for(int j = 0; j < game->size; j++){
            if(game->chessBoard[i][j]){
                if(game->chessBoard[i][j] == 1) brush.setColor(Qt::black);
                else brush.setColor(Qt::white);
                pa.setBrush(brush);
                pa.drawEllipse(30 * i, 30 * j, 20, 20);
            }
        }
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x(), y = e->y();
    if(x >= -5 && x < size().width() + 5 && y >= -5 && y < size().height() - 5)
        mousePosRow = (x+5) / 30, mousePosLin = (y+5) / 30;

    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(mousePosRow < game->size && mousePosRow >= 0 &&
        mousePosLin < game->size && mousePosLin >= 0 &&
        game->chessBoard[mousePosRow][mousePosLin] == 0){
        stepForHuman();
        update();
        if(game->gameStatus != 0){
            checkStatus();
        }
        else{
            stepForAI();
            update();
            checkStatus();
        }
    }
}

void MainWindow::stepForAI()
{

    game->playByAI();
}

void MainWindow::stepForHuman()
{
    game->putChess(mousePosRow * game->size + mousePosLin, 1);
    update();
}

int MainWindow::checkStatus()
{
    //根据不同游戏状态输出信息到弹窗
    if(game->gameStatus != 0){
        if(game->gameStatus == 1){
            QMessageBox::StandardButton Mess = QMessageBox::information(this, "You Win", "Do you want to play again?");
            if(Mess == QMessageBox::Ok) game->initGame(15);
        }
        if(game->gameStatus == -1){
            QMessageBox::StandardButton Mess = QMessageBox::information(this, "You Lose", "Do you want to play again?");
            if(Mess == QMessageBox::Ok) game->initGame(15);
        }
        if(game->gameStatus == 2){
            QMessageBox::StandardButton Mess = QMessageBox::information(this, "Draw", "Do you want to play again?");
            if(Mess == QMessageBox::Ok) game->initGame(15);
        }
    }
    return game->gameStatus;
}
