#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QString>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QTreeView>
#include <QProgressDialog>

#define DIALOGRESULTOK 1
#define DIALOGRESULTCANCEL 0

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


    QFileSystemModel *getActiveModel() const;
    void setActiveModel(QFileSystemModel *value);

private slots:
    void on_actionE_xit_triggered();
    void on_action_Expand_all_triggered();
    void on_action_Collapse_all_triggered();
    void on_actionCopy_directory_triggered();
    void on_treeLeft_clicked(const QModelIndex &index);
    void on_treeRight_clicked(const QModelIndex &index);
    //void on_rowChanged(const QItemSelection &a, const QItemSelection &b);

    void on_action_Open_triggered();

    void on_actionRename_triggered();

private:
    Ui::MainWindow *ui;
    QString currentPath = "";
    QFileSystemModel *model_left;
    QFileSystemModel *model_right;
    QFileSystemModel *activeModel;
    QProgressDialog *progress;
    void copyFolder(QString sourceFolder, QString destFolder);
    void readSettings();
    void saveSettings();
    void setupAdditionalUI();
    void countFilesInFolder(const QString &path);
    bool renameDir(const QString &oldName, const QString &newName);
    QProgressBar *statusProgress;
    QTreeView *m_ActiveTreeview;
    QString m_Tree;
    long m_totalNrOfFiles = 0;
    long m_filesCopied = 0;
    long m_totalCopied = 0;

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
