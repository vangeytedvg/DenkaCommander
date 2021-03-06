#include "editor.h"
#include "ui_editor.h"
#include "codeeditor.h"
#include <QtDebug>
#include <QDesktopWidget>
#include <QScreen>

Editor::Editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    ed = new CodeEditor();
    //ui->hlEditor->addWidget(ed);
    this->setCentralWidget(ed);
    QFont fira("Fira Code", 11, QFont::Normal);
    ed->setFont(fira);
    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    connect(ed, SIGNAL(textChanged()), this, SLOT(on_textChanged()));
}

Editor::~Editor()
{
    delete ui;
}

void Editor::on_textChanged() {
    this->texthasChanged(true);
    setIsDirty(true);
}

/**
 * @brief Editor::Open
 * Open the specified file
 */
int Editor::Open()
{
    if (isDirty) {
        int answer = QMessageBox::warning(this, "Attention", "File content was changed, save?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (answer == QMessageBox::Yes) {
            saveFile();
            emit texthasChanged(false);
            this->setIsDirty(false);
        } else if (answer == QMessageBox::Cancel) {
            return 0;
        } else if (answer == QMessageBox::No) {
           ed->clear();
           this->setIsDirty(false);
           emit texthasChanged(false);
        }
    }
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
    emit texthasChanged(false);
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
    emit texthasChanged(value);
    if (isDirty) {
        ui->statusbar->showMessage("File has been changed");
    } else {
        ui->statusbar->showMessage("");
    }
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
    emit texthasChanged(false);
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (isDirty) {
        int answer = QMessageBox::warning(this, "Attention", "File content was changed, save?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (answer == QMessageBox::No) {
            ui->statusbar->showMessage("Close cancelled...", 5000);
            emit texthasChanged(false);
            this->setIsDirty(false);
            event->accept();            
        } else if (answer == QMessageBox::Yes) {
            emit texthasChanged(false);
            this->setIsDirty(false);
            saveFile();
            event->accept();
        } else if (answer == QMessageBox::Cancel) {
            event->ignore();
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
