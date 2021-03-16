#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->currentPath.clear();
    this->setWindowTitle("Denka Commander");
    auto azen = QDir::homePath();
    this->model_left->setRootPath(QDir::homePath());

    ui->treeLeft->setModel(this->model_left);
    ui->treeLeft->setRootIndex(this->model_left->index(QDir::homePath()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionE_xit_triggered()
{
    close();
}

void MainWindow::on_action_Expand_all_triggered()
{
    // Get the index of the currently selected item and open the folder
    QModelIndex qi = ui->treeLeft->currentIndex();
    ui->treeLeft->expandRecursively(qi, -1);
}
