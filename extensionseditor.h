#ifndef EXTENSIONSEDITOR_H
#define EXTENSIONSEDITOR_H

#include <QDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

namespace Ui {
class ExtensionsEditor;
}

class ExtensionsEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ExtensionsEditor(QWidget *parent = nullptr);
    ~ExtensionsEditor();

private slots:
    void on_pushButton_Save_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::ExtensionsEditor *ui;
    void loadExtensions();
};

#endif // EXTENSIONSEDITOR_H
