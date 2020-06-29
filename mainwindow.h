#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "game.h"
#include "player.h"
#include "rank.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startGameBtn_clicked();
    void on_checkRankBtn_clicked();
    void finishGame(const QString name, const int time); // 接受gameFinished信号，游戏结束后记录成绩，展示排行榜
    void q_clicked();  // “恭喜你赢了”的窗口的确定按钮事件，点击之后关闭对话框展示排行榜

private:
    Ui::MainWindow *ui;
    Game *gameWindow = nullptr;     // 游戏界面
    player *playerWindow = nullptr; // 输入姓名，难度选择界面
    rank *r;                        // 排名
    QDialog *q;  // "恭喜你赢了"的弹窗

};

#endif // MAINWINDOW_H
