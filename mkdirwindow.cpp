#include "mkdirwindow.h"
#include "ui_mkdirwindow.h"

MkdirWindow::MkdirWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MkdirWindow)
{
    ui->setupUi(this);
}

MkdirWindow::~MkdirWindow()
{
    delete ui;
}

QString MkdirWindow::getParentFolder() const
{
    return parentFolder;
}

void MkdirWindow::setParentFolder(const QString &value)
{
    parentFolder = value;
    ui->lineEdit_parentFolder->setText(parentFolder);
}

QString MkdirWindow::getNewFoldername()
{
    newFoldername = ui->lineEdit_newFolder->text();
    return newFoldername;
}

void MkdirWindow::setNewFoldername(const QString &value)
{
    newFoldername = value;
}
