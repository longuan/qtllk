#include "player.h"
#include "ui_player.h"

player::player(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::player)
{
    ui->setupUi(this);
    ui->easyGameBtn->setChecked(true);
    setFixedSize(width(), height());
}

player::~player()
{
    delete ui;
}

void player::on_buttonBox_accepted()
{
    playerName = ui->lineEdit->text();
}

void player::on_buttonBox_rejected()
{
    this->hide();
}

void player::on_easyGameBtn_clicked()
{
    mode = 0;
}

void player::on_hardGameBtn_clicked()
{
    mode = 1;
}
