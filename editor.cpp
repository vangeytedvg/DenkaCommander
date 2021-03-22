#include "editor.h"
#include "ui_editor.h"
#include "codeeditor.h"

Editor::Editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    ed = new CodeEditor();
    //ui->hlEditor->addWidget(ed);
    this->setCentralWidget(ed);
    connect(this->ed, SIGNAL(textChanged()), this, SLOT(on_textChanged()));
}

Editor::~Editor()
{
    delete ui;
}

void Editor::on_textChanged() {
    setIsDirty(true);
}

/**
 * @brief Editor::Open
 * Open the specified file
 */
int Editor::Open()
{
    /* Check if we have a file paramter set */
    if (currentFile.isEmpty()) {
        return -1;
    }
    /* Attempt to open the file */
    QFile myFile(currentFile);
    if (!myFile.open(QIODevice::ReadOnly | QFile::Text)) {
        return -2;
    }
    setWindowTitle(currentFile);
    QTextStream in(&myFile);
    QString contents = in.readAll();
    ed->setPlainText(contents);
    myFile.close();
    show();
    /* Indicate that no changes were made */
    isDirty=false;
    return 1;

}

QString Editor::getCurrentFile() const
{
    return currentFile;
}

void Editor::setCurrentFile(const QString &value)
{
    currentFile = value;
}

bool Editor::getIsDirty() const
{
    return isDirty;
}

void Editor::setIsDirty(bool value)
{
    isDirty = value;
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (isDirty) {
        int answer = QMessageBox::warning(this, "Attention", "Folder has changed save it?", QMessageBox::Yes|QMessageBox::No);
        if (answer == QMessageBox::No) {
            ui->statusbar->showMessage("Copy cancelled...", 5000);
            event->accept();
        } else {
            event->ignore();
            /* Save the file */
        }
    }
}
