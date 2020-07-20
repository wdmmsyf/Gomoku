#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QBrush>
#include <QMessageBox>
#include <QTimer>
#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    Ui::MainWindow *ui;
    Game *game;
    int mousePosRow, mousePosLin; //鼠标停留位置对应的格子

private slots:
    int checkStatus(); //检查游戏局面，1是玩家胜，-1是AI胜，0是平局，2是未分胜负
    void stepForAI(); //AI走子
    void stepForHuman(); //玩家走子
};
#endif // MAINWINDOW_H
