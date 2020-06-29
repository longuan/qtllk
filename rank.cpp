#include "rank.h"
#include "ui_rank.h"
#include <QFileInfo>
#include "game.h"

rank::rank(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rank)
{
    ui->setupUi(this);
    QString DBPATH = "dd.db";
    QFileInfo file(DBPATH);
    if (!file.exists())
        initDB(DBPATH);
    else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(DBPATH);
        db.open();

    }
}

rank::~rank()
{
    delete ui;
}

void rank::initTable()
{
    ui->rankTable->setRowCount(10);
    ui->rankTable->setColumnCount(3);
    ui->rankTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    ui->rankTable->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选取
    ui->rankTable->verticalHeader()->setVisible(false);                 // 隐藏列头
    QStringList tableHeader;
    tableHeader<<"名次"<<"姓名"<<"用时";
    ui->rankTable->setHorizontalHeaderLabels(tableHeader);      //设置表头
}

void rank::showRank(QString playerName, QString selectQuery)
{
    ui->rankTable->clear();
    initTable();
    QSqlQuery sqlQuery;
    if (sqlQuery.exec(selectQuery)) {
        int i = 0;
        while (sqlQuery.next()) {
            int time = sqlQuery.value("time").toInt();
            QString name = sqlQuery.value("name").toString();
            ui->rankTable->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
            ui->rankTable->setItem(i, 1, new QTableWidgetItem(name));
            ui->rankTable->setItem(i, 2, new QTableWidgetItem(QString::number(time)));
            if (!playerName.compare(name) )
            { // 高亮此玩家
                ui->rankTable->item(i, 0)->setBackgroundColor(QColor(255,0,0));
                ui->rankTable->item(i, 1)->setBackgroundColor(QColor(255,0,0));
                ui->rankTable->item(i, 2)->setBackgroundColor(QColor(255,0,0));
            }
            i++;
            if (i>=10) // 最多显示十条
                break;
        }
    }
}

void rank::addPlayer(const QString playerName, const int time, const int mode)
{
    QSqlQuery sqlQuery;
    QString insertQuery = "insert into player (name, time, mode) values (?,?,?)";
    sqlQuery.prepare(insertQuery);
    sqlQuery.addBindValue(playerName);
    sqlQuery.addBindValue(time);
    sqlQuery.addBindValue(mode);
    sqlQuery.exec();
}

void rank::initDB(const QString &dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    db.open();
    QSqlQuery sqlQuery;

    QString createTable = "create table player(id int primary key, name varchar, time int, mode int)";
    sqlQuery.prepare(createTable);
    sqlQuery.exec();

}

void rank::on_okBtn_clicked()
{
    this->close();
}

void rank::on_easyModeRank_clicked(QString playerName)
{
    ui->easyModeRank->setChecked(true);
    QString selectQuery = "select name, time from player where mode=0 order by time";
    showRank(playerName, selectQuery);
}

void rank::on_hardModeRank_clicked(QString playerName)
{
    ui->hardModeRank->setChecked(true);
    QString selectQuery = "select name, time from player where mode=1 order by time";
    showRank(playerName, selectQuery);
}
