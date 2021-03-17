#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileSystemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionE_xit_triggered();
    void on_action_Expand_all_triggered();
    void on_action_Collapse_all_triggered();
    void on_actionCopy_directory_triggered();

private:
    Ui::MainWindow *ui;
    QString currentPath = "";
    QFileSystemModel *model_left;
    QFileSystemModel *model_right;
    void copyFolder(QString sourceFolder, QString destFolder);
};
#endif // MAINWINDOW_H
