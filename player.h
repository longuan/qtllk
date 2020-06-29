#ifndef PLAYER_H
#define PLAYER_H

#include <QDialog>
#include <QString>

namespace Ui {
class player;
}

class player : public QDialog
{
    Q_OBJECT

public:
    explicit player(QWidget *parent = nullptr);
    ~player();

    QString playerName = "";  // 玩家姓名
    int mode = 0; // 0代表简单模式，1代表困难模式

private slots:

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_easyGameBtn_clicked();  // 设置mode为0

    void on_hardGameBtn_clicked();  // 设置mode为1

private:
    Ui::player *ui;
};

#endif // PLAYER_H
