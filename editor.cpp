#include "editor.h"
#include "ui_editor.h"
#include "codeeditor.h"
#include <QtDebug>

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
    setIsDirty(false);
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

void Editor::saveFile()
{
    // If we don't have a filename from before, get one.
    QString fileName = currentFile;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ed->toPlainText();
    out << text;
    file.close();
    setIsDirty(false);
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (isDirty) {
        int answer = QMessageBox::warning(this, "Attention", "File content was changed, save?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (answer == QMessageBox::No) {
            ui->statusbar->showMessage("Copy cancelled...", 5000);
            event->accept();
        } else {           
            saveFile();
        }
    }
}

void Editor::on_actionCut_triggered()
{
    #if QT_CONFIG(clipboard)
        ed->cut();
    #endif
}

void Editor::on_actionCopy_triggered()
{
#if QT_CONFIG(clipboard)
    ed->copy();
#endif
}

void Editor::on_actionPaste_triggered()
{
#if QT_CONFIG(clipboard)
    ed->paste();
#endif
}

void Editor::on_action_Save_triggered()
{
    saveFile();
}

void Editor::on_actionExit_triggered()
{
    this->close();
}

void Editor::on_action_Undo_triggered()
{
    ed->undo();
}

void Editor::on_actionRedo_triggered()
{
    ed->redo();
}
