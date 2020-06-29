#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QDialog>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTime>

namespace Ui {
class Game;
}

class Game : public QMainWindow
{
    Q_OBJECT

public:
//    explicit Game(QWidget *parent = nullptr);
    Game( QString name, int mode=0, QWidget *parent = nullptr);  // name是玩家姓名，mode是难度

    ~Game();
    int remainBtns;      // 剩余的按钮数量，用来判断游戏是否结束，remainBtns为0说明游戏结束
    QString playerName;  // 玩家姓名

    int mode;     // mode为1: image_set=22, GAME_COL, GAMEMAP_ROW=20，困难模式
                  // mode为0: image_set=14, GAME_COL, GAMEMAP_ROW=10，简单模式

signals:
    void gameFinished(const QString name, const int time);  // 游戏结束会发出这个信号，会被MainWindow接收

private slots:
    void on_finishGame_clicked(); // 点击结束游戏按钮，关闭此窗口
    void click_imageBtn();        // 点击图片按钮，游戏的主要逻辑

    void on_pauseGame_clicked();  // 点击暂停游戏按钮

    void on_timer_timeout();      // 用于每秒更新一下界面上的时间
    void on_getAHint_clicked();   // 点击获取提示按钮

private:
    int image_set;    // 使用的不重复的图片数量
    int GAMEMAP_COL;  // 棋盘的列数
    int GAMEMAP_ROW;  // 期盼的行数

    QVector<QVector<QPushButton *>> imageBtns;   // 二维数组，存储所有的图片按钮，与images数组对应
    QVector<QVector<int>> images;                // 二维数组，将图片抽象成int存储。0代表此处无图片
    void initGameMap();        // 初始化imageBtns和images，生成棋盘

    QTimer * timer;      // 定时器，每秒更新一下界面
    QTime * recordTime;  // 记录用时

    QPushButton *firstSelectedImage = nullptr;  // 记录第一下点击的图片
    Ui::Game *ui;

    QPushButton* createImageBtn(QString s); // 创建一个图片按钮，s是图片的路径

    bool canLink(QPushButton *firstBtn, QPushButton *secondBtn, bool hintFlag=false);
    bool linkDirectly(int x1, int y1, int x2, int y2);  // 当两点同x坐标或者同y坐标时，判断两点是否可以用一个线段直接连接
    bool linkWithOneCorner(int x1, int y1, int x2, int y2); // 能用一个拐角连接
    bool linkWithTwoCorner(int x1, int y1, int x2, int y2); // 能用两个拐角连接

    bool isPaused; // 记录当前是否是暂停状态
    int usedTime;  // 暂停时用来保存已用时间

    QPushButton *hintBtnA;  // 提示按钮
    QPushButton *hintBtnB;  // 提示按钮
    void refreshMap();      // 重置棋盘
    bool getHint();         // 获取提示

};

#endif // GAME_H
