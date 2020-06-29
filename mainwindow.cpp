#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    r = new rank(this);
    setFixedSize(width(), height());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::finishGame(const QString name, const int time)
{
    r->addPlayer(name, time, gameWindow->mode);
    gameWindow->close();

    q = new QDialog();
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(new QLabel("恭喜你赢了"+QString("\n\n")+QString("姓名：")+name+QString("      ")+QString("用时：")+QString::number(time)));
    QPushButton *b = new QPushButton();
    b->setText("确定");
    vlayout->addWidget(b);
    connect(b, SIGNAL(clicked()), this, SLOT(q_clicked()));
    q->setLayout(vlayout);
    q->setWindowTitle("通关");
    q->resize(300,200);
    q->show();
}

void MainWindow::q_clicked()
{
    q->close();
    r->resize(510, 600);
    r->setWindowTitle("排名");
    if (gameWindow->mode)
        r->on_hardModeRank_clicked(gameWindow->playerName);
    else
        r->on_hardModeRank_clicked(gameWindow->playerName);
    r->show();
}

void MainWindow::on_startGameBtn_clicked()
{
    playerWindow = new player();
    playerWindow->setWindowTitle("登录");
    if (playerWindow->exec() == QDialog::Accepted )
    {
        if ( playerWindow->playerName != "") {
            gameWindow = new Game(playerWindow->playerName, playerWindow->mode, this);
            connect(gameWindow, SIGNAL(gameFinished(const QString, const int)),this,SLOT(finishGame(const QString, const int) ));

            gameWindow->setWindowTitle("连连看游戏");
            gameWindow->show();
            QDesktopWidget* desktop = QApplication::desktop();
            gameWindow->move((desktop->width() - gameWindow->width())/2, (desktop->height() - gameWindow->height())/2);

        }
        else {
            QDialog *emptyName = new QDialog();
            QVBoxLayout *vlayout = new QVBoxLayout();
            vlayout->addWidget(new QLabel("用户名不能为空!!"));
            emptyName->setLayout(vlayout);
            emptyName->resize(100, 100);
            emptyName->show();

        }
    }
}

void MainWindow::on_checkRankBtn_clicked()
{
    r->setFixedSize(510, 600);
    r->setWindowTitle("排名");
    r->on_easyModeRank_clicked();
    r->show();
}
