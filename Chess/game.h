#include<vector>
#include <QMessageBox>
#include<cstring>
#include<QtDebug>


#ifndef GAME_H
#define GAME_H

enum{ //每种局面对应的分数
    WIN5 = 100000, //五子连珠
    WIN4 = 10000, //两头未被堵住的四子
    RES4 = 1000, //一头被堵住的四子
    BOT3 = 1000, //两头未被堵住的三子
    RES3 = 100, //一头被堵住的三子
    BOT2 = 100, //...
    RES2 = 10, //...
    BOT1 = 1, //...
    RES1 = 1 //...
};

class Game
{
public:
    Game();
    int **chessBoard; //储存当前游戏棋盘上棋子的分布情况，0为空白，1为黑棋，-1为白棋
    int size; //棋盘大小为size*size
    int count; //落子的总数
    int gameStatus; //局面胜负状况，1是玩家胜，-1是AI胜，0是平局，2是未分胜负

    void initGame(int _size);//初始化游戏
    int evaluatePos(int pos, int _player); //查询在某个位置下棋可以构成怎样的局面
    int evaluateBoard(); //查询当前棋盘的分数之和
    void playByAI(); //AI走子
    int min_searchAB(int depth, int alpha, int beta);//搜索最小值
    int max_searchAB(int depth, int alpha, int beta);//搜索最大值
    std::vector<int> InformedSearch(int player); //启发式搜索
    int checkStatus(int pos); //1是玩家胜，-1是AI胜，0是平局，2是未分胜负
    void putChess(int pos, int _player);//走子并判断棋盘情况
    bool noNeighbor(int pos);//查询当前位置的相邻位置是否有棋子放置，如果没有就跳过这个位置

};

#endif // GAME_H
