#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QTreeView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QTreeView *ActiveTreeview() const;
    void setActiveTreeview(QTreeView *ActiveTreeview);

private slots:
    void on_actionE_xit_triggered();
    void on_action_Expand_all_triggered();
    void on_action_Collapse_all_triggered();
    void on_actionCopy_directory_triggered();

    void on_treeLeft_clicked(const QModelIndex &index);

    void on_treeRight_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QString currentPath = "";
    QFileSystemModel *model_left;
    QFileSystemModel *model_right;
    void copyFolder(QString sourceFolder, QString destFolder);
    void readSettings();
    void saveSettings();
    void setupAdditionalUI();
    QProgressBar *statusProgress;
    QTreeView *m_ActiveTreeview;
    QString m_Tree;

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
