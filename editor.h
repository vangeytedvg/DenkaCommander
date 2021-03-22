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

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    int Open();

    QString getCurrentFile() const;
    void setCurrentFile(const QString &value);

    bool getIsDirty() const;
    void setIsDirty(bool value);

private:
    Ui::Editor *ui;
    CodeEditor *ed;
    QString currentFile;
    bool isDirty = false;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // EDITOR_H
