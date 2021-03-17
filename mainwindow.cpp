#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->currentPath.clear();
    this->setWindowTitle("Denka Commander");
    // Left side browser
    this->model_left = new QFileSystemModel();
    this->model_left->setRootPath(QDir::homePath());
    ui->treeLeft->setModel(this->model_left);
    ui->treeLeft->setRootIndex(this->model_left->index(QDir::homePath()));
    // Nothing selected on the left!
    ui->treeLeft->setCurrentIndex(this->model_left->index(0));
    // Right side browser
    this->model_right = new QFileSystemModel();
    this->model_right->setRootPath(QDir::homePath());
    ui->treeRight->setModel(this->model_right);
    ui->treeRight->setRootIndex(this->model_right->index(QDir::homePath()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Quit the application
 */
void MainWindow::on_actionE_xit_triggered()
{
    close();
}

/**
 * @brief Get the index of the currently selected item and open the folder
 */
void MainWindow::on_action_Expand_all_triggered()
{

    QModelIndex qi = ui->treeLeft->currentIndex();
    ui->treeLeft->expandRecursively(qi, -1);
}

/**
 * @brief Get the index of the currently selected item and close the folder
 */
void MainWindow::on_action_Collapse_all_triggered()
{
    QModelIndex qi = ui->treeLeft->currentIndex();
    ui->treeLeft->collapse(qi);
}

/**
 * @brief Call the copy directory method
 */
void MainWindow::on_actionCopy_directory_triggered()
{
    // First find out what is selected on the left side
    QModelIndex leftIndex = ui->treeLeft->currentIndex();
    QString leftPath = this->model_left->fileInfo(leftIndex).absoluteFilePath();

    // Something selected on the left side?
    if (leftIndex.isValid()) {
        // Check the right side now...
        QModelIndex rightIndex = ui->treeRight->currentIndex();
        // Have we a selection there?
        if (rightIndex.isValid()) {
            QString rightPath = this->model_left->fileInfo(rightIndex).absoluteFilePath();
            // We are not on the same path huh?
            if (rightPath == leftPath) {
                QMessageBox::warning(this, "Warning", "Source and target folders are the same!");
                return;
            }
            // Is the selection on the right side a folder?
            if (!this->model_right->isDir(rightIndex)) {
                QMessageBox::warning(this, "Warning", "Target is not a directory!");
                return;
            } else {
                // Are we copying a file or a directory?
                if (this->model_left->isDir(leftIndex)) {
                    // Start copying
                    qDebug() << leftPath << " -> " << rightPath;
                    this->copyFolder(leftPath, rightPath);
                    ui->statusbar->showMessage("Files copied to target directory!", 5000);
                } else {
                    // Must be a file
                    QFile::copy()
                }
            }
        } else {
            QMessageBox::information(this, "Info", "No valid target selection!");
        }
    } else {
        // No selection on the left
        QMessageBox::information(this,"Info", "No selection on source!");
    }
}


/**
 * @brief Recursively copy folder from left to right panel
 * @param sourceFolder
 * @param destFolder
 */
void MainWindow::copyFolder(QString sourceFolder, QString destFolder)
{
    // Get the directory on the left side
    QDir sourceDir(sourceFolder);
    if(!sourceDir.exists())
        return;
    // Directory on the right side (target)
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
    }
    // Get the files and copy them
    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++)
    {      
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);
        ui->statusbar->showMessage("Copying" + srcName + " to " + destName);
        QCoreApplication::processEvents();
    }
    files.clear();
    // In case of a subdirectory, use recursion
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        copyFolder(srcName, destName);
    }
}
