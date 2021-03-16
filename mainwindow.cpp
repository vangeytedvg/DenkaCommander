#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->currentPath.clear();
    this->setWindowTitle("Denka Commander");
    this->model_left->setRootPath(QDir::currentPath());
    ui->treeLeft->setModel(this->model_left);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionE_xit_triggered()
{
    close();
}
