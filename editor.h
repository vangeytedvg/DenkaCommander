#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include "codeeditor.h"
#define FILENOTFOUND -1
#define FILENAMENOTSET = -2
#define FILENORMAL = 1

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_textChanged();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_action_Save_triggered();

    void on_actionExit_triggered();

    void on_action_Undo_triggered();

    void on_actionRedo_triggered();

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    int Open();
    QString getCurrentFile() const;
    void setCurrentFile(const QString &value);
    bool getIsDirty() const;
    void setIsDirty(bool value);

signals:
    void texthasChanged(bool);

private:
    Ui::Editor *ui;
    CodeEditor *ed;
    QString currentFile;
    bool isDirty = false;
    void saveFile();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // EDITOR_H
