#include "game.h"

Game::Game()
{
    chessBoard = nullptr;
}

void Game::initGame(int _size)
{
    if(chessBoard != nullptr){ //如果已经初始化过棋盘，先将所有内存释放
        for(int i = 0; i < size; i++){
            if(chessBoard[i] != nullptr) delete chessBoard[i];
        }
        delete chessBoard;
    }

    size = _size;
    chessBoard = new int*[size];

    for(int i = 0; i < size; i++){
        chessBoard[i] = new int[size];
        memset(chessBoard[i], 0, size * sizeof(int));
    }
    gameStatus = 0;
}

/*
 * 通过枚举棋盘上每一条连续的棋子的状态来对局面进行估价，复杂度为O(size^2),但常数较大
 * 状况要考虑连续的棋子个数、两头分别是否有空格、空格的个数
 * 只检索四个方向，并且不检索反方向有相同颜色的格点，保证每条连续棋子只会被统计一次
 */
int Game::evaluateBoard()
{
    int sum_score = 0;
    int fx[4] = {1, 1, 0, -1}, fy[4] = {0, 1, 1, 1}; //向四个方向判断

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            for(int k = 0; k < 4; k++){
                int upx = i - fx[k], upy = j - fy[k]; //反方向上一个位置
                int score = 0;
                int x = i, y = j; //枚举的变量
                int emp1 = 0, emp2 = 0, cnt = 0, col = 0;

                if(upx < 0 || upy < 0 || upx >= size || upy >= size) continue;
                if(chessBoard[upx][upy] != 0){
                    if(chessBoard[upx][upy] == chessBoard[i][j]) continue;
                }
                while(x >= 0 && y >= 0 && x < size && y < size){
                    if(chessBoard[x][y] == 0){
                        if(cnt) emp2++;
                        else emp1++;
                    }
                    if(chessBoard[x][y] != 0 && col == 0){
                        col = chessBoard[x][y];
                    }
                    if(chessBoard[x][y] == col){
                        if(emp2) break;
                        cnt ++;
                    }
                    if(chessBoard[x][y] == -col){
                        break;
                    }
                    x += fx[k], y += fy[k];
                }
                /*
                 * 不同的局势会获得不同的分数
                 */
                if(cnt >= 5) score += WIN5;
                else if(cnt == 4 && emp1 != 0 && emp2 != 0) score += WIN4;
                else if(cnt == 4 && emp1 + emp2 != 0) score += RES4;
                else if(cnt == 3 && emp1 != 0 && emp2 != 0) score += BOT3;
                else if(cnt == 3 && emp1 + emp2 >= 2) score += RES3;
                else if(cnt == 2 && emp1 != 0 && emp2 != 0 && emp1 + emp2 >= 3) score += BOT2;
                else if(cnt == 2 && emp1 + emp2 >= 3) score += RES2;
                else if(cnt == 1 && emp1 != 0 && emp2 != 0 && emp1 + emp2 >= 4) score += BOT1;
                else if(cnt == 1 && emp1 + emp2 >= 4) score += RES1;
                sum_score += score * col;
            }
        }
    }
    return sum_score;
}

/*
 * 评估在某一个位置下棋所带来的格点价值受益
 * 因为只用于启发式剪枝，所以不考虑下这个子后破坏原有局面格点价值的影响
 */
int Game::evaluatePos(int pos, int _player)
{
    int posx = pos/size, posy = pos%size;
    int fx[4] = {1, 1, 0, -1}, fy[4] = {0, 1, 1, 1};
    int score = 0;
    chessBoard[posx][posy] = _player;

    for(int i = 0; i < 4; i++){
        int cnt = -1, emp1 = 0, emp2 = 0;
        for(int x = posx, y = posy;;x += fx[i], y += fy[i]){
            if(x < 0 || y < 0 || x >= size || y >= size) break;
            if(chessBoard[x][y] == -chessBoard[posx][posy]) break;
            if(chessBoard[x][y] != 0 && emp1) break;
            if(chessBoard[x][y] == 0) emp1++;
            else cnt++;
            if(emp1 >= 4) break;
        }
        for(int x = posx, y = posy;;x -= fx[i], y -= fy[i]){
            if(x < 0 || y < 0 || x >= size || y >= size) break;
            if(chessBoard[x][y] == -chessBoard[posx][posy]) break;
            if(chessBoard[x][y] != 0 && emp2) break;
            if(chessBoard[x][y] == 0) emp2++;
            else cnt++;
            if(emp2 >= 4) break;
        }
        if(cnt >= 5) score += WIN5;
        else if(cnt == 4 && emp1 != 0 && emp2 != 0) score += WIN4;
        else if(cnt == 4 && emp1 + emp2 != 0) score += RES4;
        else if(cnt == 3 && emp1 != 0 && emp2 != 0) score += BOT3;
        else if(cnt == 3 && emp1 + emp2 >= 2) score += RES3;
        else if(cnt == 2 && emp1 != 0 && emp2 != 0 && emp1 + emp2 >= 3) score += BOT2;
        else if(cnt == 2 && emp1 + emp2 >= 3) score += RES2;
        else if(cnt == 1 && emp1 != 0 && emp2 != 0 && emp1 + emp2 >= 4) score += BOT1;
        else if(cnt == 1 && emp1 + emp2 >= 4) score += RES1;
    }
    chessBoard[posx][posy] = 0;

    return score;
}

void Game::playByAI()
{
    int pos = min_searchAB(6, WIN5 * 100, -WIN5 * 100);
    putChess(pos, -1);
    count++;
}

int Game::min_searchAB(int depth, int alpha, int beta)
{
    int val = evaluateBoard(); //统计当前棋盘局面
    int best = 1000 * WIN5, chopos; //给最优化信息赋初值

    if(depth <= 0) return val; //如果深度到底就退出

    std::vector<int> choice = InformedSearch(-1);
    for(auto iter = choice.begin(); iter != choice.end(); iter++){
        int pos = *iter;
        if(chessBoard[pos/size][pos%size] != 0) continue;

        int valp = evaluatePos(pos, -1);
        if(valp >= WIN5){
            chopos = pos;
            best = -100 * WIN5;
            break;
        }

        chessBoard[pos/size][pos%size] = -1;
        int tmp = max_searchAB(depth - 1, best < alpha ? best : alpha, beta);
        chessBoard[pos/size][pos%size] = 0;
        if(tmp < best){
            best = tmp;
            chopos = pos;
        }
        if(tmp < beta) break;
    }

    if(depth == 6) return chopos; //如果是搜索的第一层，需要返回的是最优的选择
    else return best;
}

int Game::max_searchAB(int depth, int alpha, int beta)
{
    int val = evaluateBoard(); //统计当前棋盘局面
    int best = - 1000 * WIN5; //给最优化信息赋初值

    if(depth <= 0) return val;//如果深度到底就退出

    std::vector<int> choice = InformedSearch(1);
    for(auto iter = choice.begin(); iter != choice.end(); iter++){
        int pos = *iter;
        if(chessBoard[pos/size][pos%size] != 0) continue;

        int valp = evaluatePos(pos, 1);
        if(valp >= WIN5){
            best = 100 * WIN5;
            break;
        }

        chessBoard[pos/size][pos%size] = 1;
        int tmp = min_searchAB(depth - 1, alpha, best > beta ? best : beta);
        chessBoard[pos/size][pos%size] = 0;
        if(tmp > best){
            best = tmp;
        }
        if(tmp > alpha) break;
    }

    return best;
}

std::vector<int> Game::InformedSearch(int player)
{
    std::vector<int> wait;
    std::vector<int> comWIN5, comWIN4, comWIN3, com[5];
    std::vector<int> plaWIN5, plaWIN4, plaWIN3, pla[5];

    for(int i = 0; i < size * size; i++){
        if(chessBoard[i/size][i%size] != 0) continue;
        if(!noNeighbor(i)) continue;
        int comScore = evaluatePos(i, player);//AI下这个位置可以得到的分值
        int plaScore = evaluatePos(i, -player);//玩家下这个位置可以得到的分值

        if(comScore >= WIN5) comWIN5.push_back(i);
        else if(plaScore >= WIN5) plaWIN5.push_back(i);
        else if(comScore >= WIN4) comWIN4.push_back(i);
        else if(plaScore >= WIN4) plaWIN4.push_back(i);
        else if(comScore >= 2 * BOT3) comWIN3.push_back(i);
        else if(plaScore >= 2 * BOT3) plaWIN3.push_back(i);
        else if(comScore >= RES4) com[4].push_back(i);
        else if(plaScore >= RES4) pla[4].push_back(i);
        else if(comScore >= RES3) com[3].push_back(i);
        else if(plaScore >= RES3) pla[3].push_back(i);
        else if(comScore >= RES2) com[2].push_back(i);
        else if(plaScore >= RES2) pla[2].push_back(i);
        else if(comScore >= RES1) com[1].push_back(i);
        else if(plaScore >= RES1) pla[1].push_back(i);
    }

    //先返回必胜策略,越长的连续棋子越先胜利
    if(!comWIN5.empty()) return comWIN5;
    if(!plaWIN5.empty()) return plaWIN5;

    if(!comWIN4.empty() || !plaWIN4.empty()){
       wait.insert(wait.end(), comWIN4.begin(), comWIN4.end());
       wait.insert(wait.end(), plaWIN4.begin(), plaWIN4.end());
       return wait;
    }

    if(!comWIN3.empty() || ! plaWIN3.empty()){
       wait.insert(wait.end(), comWIN3.begin(), comWIN3.end());
       wait.insert(wait.end(), plaWIN3.begin(), plaWIN3.end());
       return wait;
    }

    //按照价值从大到小返回其他所有方案
    for(int i = 4; i >= 1; i--){
        wait.insert(wait.end(), com[i].begin(), com[i].end());
        wait.insert(wait.end(), pla[i].begin(), pla[i].end());
    }
    return wait;
}

//检查下完某步棋后游戏是否结束
int Game::checkStatus(int pos)
{
    if(count == size * size) return 2;
    int posx = pos/size, posy = pos%size;
    int fx[4] = {1, 1, 0, -1}, fy[4] = {0, 1, 1, 1};
    for(int i = 0; i < 4; i++){
        int cnt = -1;
        for(int x = posx, y = posy;;x += fx[i], y += fy[i]){
            if(x < 0 || y < 0 || x >= size || y >= size) break;
            if(chessBoard[x][y] != chessBoard[posx][posy]) break;
            cnt++;
        }
        for(int x = posx, y = posy;;x -= fx[i], y -= fy[i]){
            if(x < 0 || y < 0 || x >= size || y >= size) break;
            if(chessBoard[x][y] != chessBoard[posx][posy]) break;
            cnt++;
        }
        if(cnt >= 5) return chessBoard[posx][posy];
    }
    return 0;
}

void Game::putChess(int pos, int _player)
{
    int posx = pos/size, posy = pos%size;
    chessBoard[posx][posy] = _player;
    count++;
    gameStatus = checkStatus(pos);
}

//如果有邻居就返回1， 否则返回0
bool Game::noNeighbor(int pos)
{
    int fx[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    int fy[8] = {1, 0, -1, -1, -1, 0, 1, 1};
    for(int i = 0; i < 8; i++){
        int tx = pos / size, ty = pos % size;
        for(int k = 0; k < 1; k++){
            tx += fx[i], ty += fy[i];
            if(tx < 0 || ty < 0 || tx >= size || ty >= size) break;
            if(chessBoard[tx][ty] != 0) return 1;
        }
    }
    return 0;
}
