#include "mkdirwindow.h"
#include "ui_mkdirwindow.h"

mkdirwindow::mkdirwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mkdirwindow)
{
    ui->setupUi(this);
}

mkdirwindow::~mkdirwindow()
{
    delete ui;
}
