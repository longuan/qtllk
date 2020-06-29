#ifndef RANK_H
#define RANK_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

namespace Ui {
class rank;
}

class rank : public QDialog
{
    Q_OBJECT

public:
    explicit rank(QWidget *parent = nullptr);
    ~rank();

    void showRank(const QString name, const QString selectQuery);     // 展示排行榜，同时高亮某个玩家的成绩
    void addPlayer(const QString playerName, const int time, const int mode);  // 记录成绩到数据库

    QSqlDatabase db;   // 数据库

public slots:
    void on_okBtn_clicked();

    void on_easyModeRank_clicked(QString playerName="");

    void on_hardModeRank_clicked(QString playerName="");

private:
    Ui::rank *ui;
    void initDB(const QString &dbName);   // 初始化数据库，创建数据库文件，创建相关表
    void initTable();

};

#endif // RANK_H
