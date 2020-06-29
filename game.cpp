#include "game.h"
#include "ui_game.h"
#include <QPixmap>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QFileInfo>

Game::Game( QString name, int mode, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Game),
    playerName(name),
    mode(mode)

{
    ui->setupUi(this);

    ui->playerName->setText("玩家："+playerName);

    if (mode == 0) {
        GAMEMAP_COL = 10;
        GAMEMAP_ROW = 10;
        remainBtns = GAMEMAP_COL*GAMEMAP_ROW;
        image_set = 14;
    }
    else {
        GAMEMAP_COL = 20;
        GAMEMAP_ROW = 20;
        remainBtns = GAMEMAP_COL*GAMEMAP_ROW;
        image_set = 22;
    }
    timer = new QTimer;
    recordTime = new QTime;

    recordTime->start();

    connect(timer, SIGNAL(timeout()), this , SLOT(on_timer_timeout()));

    timer->start(1000);

    isPaused = false;
    usedTime = 0;

    initGameMap();

    setMaximumSize(width()*1.5, height()*1.5);
}

Game::~Game()
{
    delete ui;
}

void Game::on_timer_timeout()
{
    ui->playTime->setText("时间："+QString::number(recordTime->elapsed()/1000 + usedTime));
}

void Game::initGameMap()
{
    int image_number = 0;
    QVector<int> image_list;
    for (int i=0;i<GAMEMAP_ROW;i++) {
        for (int j=0;j<GAMEMAP_COL/2;j++) {
            image_number = std::rand() % image_set;
            if (image_number == 0) {   // 0代表无图片，所以跳过这种情况
                j--;
                continue;
            }
            image_list.append(image_number); // 将同一张图片存储两次，确保配对
            image_list.append(image_number);
        }
    }
    std::random_shuffle(image_list.begin(), image_list.end()); // 打乱顺序

    QVector<int> empty_vector;
    for (int i=0;i<GAMEMAP_COL+2;i++) {
        empty_vector.append(0);
    }
    images.append(empty_vector);  // 用一圈0包围图片矩阵，主要用于两个拐角的判断
    for (int i=0;i<GAMEMAP_ROW;i++) {
        QVector<int> images_r;
        images_r.append(0);       // 用一圈0包围图片矩阵
        QVector<QPushButton *> btns;
        for (int j=0;j<GAMEMAP_COL;j++) {
            images_r.append(image_list[i*10+j]);
            QPushButton *b = createImageBtn("img\\" + QString::number(image_list[i*10+j]) + ".ico");
            ui->gridLayout->addWidget(b, i, j, 1, 1);

            b->setObjectName(QString("i_")+QString::number(i)+"_"+QString::number(j));  // 用objectName来判断点击的是哪个按钮
            connect(b, SIGNAL(clicked(bool)), this, SLOT(click_imageBtn()));
            btns.append(b);
        }
        imageBtns.append(btns);
        images_r.append(0);        // 用一圈0包围图片矩阵
        images.append(images_r);
    }
    images.append(empty_vector);   // 用一圈0包围图片矩阵

    while(!getHint())   // 生成棋盘后马上获取提示，同时判断是否死局
        refreshMap();
}

void Game::on_finishGame_clicked()
{
    this->close();
}

QPushButton* Game::createImageBtn(QString s)
{
    QPushButton *btn = new QPushButton();
    QPixmap icon(s);
    btn->setIcon(icon);
    btn->setFixedSize(icon.size());
    return btn;
}

void Game::click_imageBtn()
{
    QPushButton *selectedImg = qobject_cast<QPushButton *>(sender());

    if(!firstSelectedImage) {  // 第一次点击
        firstSelectedImage = selectedImg;
        firstSelectedImage->setStyleSheet("background:transparent");
    }
    else {   // 第二次点击
        if (firstSelectedImage->objectName() == selectedImg->objectName()) { // 两次点击同一张图片
            firstSelectedImage->setStyleSheet("background:none");
            firstSelectedImage = nullptr;
        }
        else {  // 两次点击不同的图片，判断两张图片是否可以连接消除
            if (canLink(firstSelectedImage, selectedImg))
            {         // 可以消除
                QSizePolicy sp_retain = firstSelectedImage->sizePolicy();
                sp_retain.setRetainSizeWhenHidden(true);
                firstSelectedImage->setSizePolicy(sp_retain);
                firstSelectedImage->hide();    // 隐藏按钮同时占有原来的空间

                QSizePolicy sp_retain2 = selectedImg->sizePolicy();
                sp_retain2.setRetainSizeWhenHidden(true);
                selectedImg->setSizePolicy(sp_retain);
                selectedImg->hide();          // 隐藏按钮同时占有原来的空间

                firstSelectedImage = nullptr;
                remainBtns -= 2;
                if(remainBtns == 0)   // 所有图片都已经消除，游戏完成
                {
                    emit gameFinished(playerName, recordTime->elapsed()/1000+usedTime); // 发出游戏完成的信号
                    return;
                }

                while(!getHint()) // 获取提示，判断是否死局
                    refreshMap();
            }
            else {       // 不能消除
                firstSelectedImage->setStyleSheet("background:none");
                firstSelectedImage = nullptr;
            }
        }
    }
}

bool Game::canLink(QPushButton *firstBtn, QPushButton *secondBtn, bool hintFlag)
{
    int x1 = firstBtn->objectName().split("_")[1].toInt() + 1; // 加一是因为images二维数组多了一圈0
    int y1 = firstBtn->objectName().split("_")[2].toInt() + 1;
    int x2 = secondBtn->objectName().split("_")[1].toInt() + 1;
    int y2 = secondBtn->objectName().split("_")[2].toInt() + 1;

    if (images[x1][y1] == images[x2][y2] &&( linkDirectly(x1, y1, x2, y2) || linkWithOneCorner(x1, y1, x2, y2) || linkWithTwoCorner(x1, y1, x2, y2)))
    {
        if (!hintFlag){  // 如果用做获取提示，就不用改变images数组
            images[x1][y1] = 0;
            images[x2][y2] = 0;
        }
        return true;
    }
    else {
        return false;
    }
}

bool Game::linkDirectly(int x1, int y1, int x2, int y2) // 判断是否能用一条线段连接
{
    if (x1 == x2) // 保证y2>y1
    {
        if (y1>y2) {
            std::swap(y1, y2);
        }
        for (int i=y1+1;i<y2;i++) {
            if (images[x1][i] != 0)
                return false;
        }
        return true;
    }
    else if (y1 == y2)
    {
        if(x1>x2) { // 保证x2>x1
            std::swap(x1, x2);
        }
        for(int i=x1+1;i<x2;i++)
        {
            if (images[i][y1] != 0)
                return false;
        }
        return true;
    }
    else {
        return false;
    }
}

bool Game::linkWithOneCorner(int x1, int y1, int x2, int y2) // 判断是否能用两条线段连接
{
    if (x1 == x2 || y1 == y2)
    {
        return false;
    }

    if (linkDirectly(x1, y1, x2, y1) && linkDirectly(x2, y1, x2, y2) && images[x2][y1] == 0){
        return true;
    }
    if (linkDirectly(x1, y1, x1, y2) && linkDirectly(x1, y2, x2, y2) && images[x1][y2] == 0){
        return true;

    }
    return false;
}

bool Game::linkWithTwoCorner(int x1, int y1, int x2, int y2)
{
    // 从x1，y1 向左延申
    for (int i=x1-1;i>=0;i--) {
        if (images[i][y1] != 0)
            break;
        else {
            if (linkWithOneCorner(i, y1, x2, y2))
            {
                return true;
            }
        }
    }

    // 向右延申
    for (int i=x1+1;i<=GAMEMAP_COL+1;i++) {
         if (images[i][y1] != 0)
            break;
        else {
            if (linkWithOneCorner(i, y1, x2, y2))
            {
                return true;
            }
        }
    }

    // 向上延伸
    for (int i=y1-1;i>=0;i--) {
        if (images[x1][i] != 0)
            break;
        else {
            if (linkWithOneCorner(x1, i, x2, y2))
            {
                return true;
            }
        }
    }

    // 向下延伸
    for (int i=y1+1;i<=GAMEMAP_ROW+1;i++) {
        if (images[x1][i] != 0)
            break;
        else {
            if (linkWithOneCorner(x1, i, x2, y2))
            {
                return true;
            }
        }
    }
    return false;
}

void Game::on_pauseGame_clicked()
{
    if (isPaused) {  // 继续游戏
        ui->pauseGame->setText("暂停游戏");
        timer->start(1000);
        recordTime->restart();
        for (int i=0;i<imageBtns.size();i++) {
            QVector<QPushButton *> b = imageBtns[i];
            for (int j=0;j<b.size();j++) {
                b[j]->setEnabled(true);
            }
        }
        isPaused = false;
    }
    else {  // 暂停游戏
        ui->pauseGame->setText("继续游戏");
        timer->stop();
        usedTime = recordTime->elapsed()/1000 + usedTime;
        for (int i=0;i<imageBtns.size();i++) {
            QVector<QPushButton *> b = imageBtns[i];
            for (int j=0;j<b.size();j++) {
                b[j]->setEnabled(false);
            }
        }
        isPaused = true;
    }
}

void Game::refreshMap()
{
    QVector<int> temp;
    for (int i=0;i<images.size();i++) {
        for (int j=0;j<images[i].size();j++) {
            if (images[i][j] != 0)
            {
                temp.append(images[i][j]);
            }
        }
    }

    std::random_shuffle(temp.begin(), temp.end());  // 打乱顺序

    int index = 0;
    for (int i=0;i<images.size();i++) {
        for (int j=0;j<images[i].size();j++) {
            if (images[i][j] != 0)
            {
                images[i][j] = temp[index];

                imageBtns[i-1][j-1]->setIcon(QPixmap("img\\"
                                                     +QString::number(temp[index])+".ico"));
                index++;
            }
        }
    }
}

bool Game::getHint()
{
    for (int i=0;i<images.size();i++) {
        for (int j=0;j<images[i].size();j++) {
            if (images[i][j] != 0)
            {
                for (int m=0;m<images.size();m++) {
                    for (int n=0;n<images[m].size();n++) {
                        if ((i!=m || j!=n) && images[i][j]==images[m][n] && canLink(imageBtns[i-1][j-1], imageBtns[m-1][n-1], true))
                        {   // 找到可以连接的图片对，作为提示，说明不是死局
                            hintBtnA = imageBtns[i-1][j-1];
                            hintBtnB = imageBtns[m-1][n-1];
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;   // 找不到提示，死局
}

void Game::on_getAHint_clicked()
{
    hintBtnA->setStyleSheet("background:red");
    hintBtnB->setStyleSheet("background:red");
}
