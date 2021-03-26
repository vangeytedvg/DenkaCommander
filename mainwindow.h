#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QString>
#include <QFileSystemModel>
#include <QProgressBar>
#include <QTreeView>
#include <QModelIndex>
#include <QCloseEvent>
#include <QProgressDialog>
#include "editor.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QSettings>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QPoint>
#include <QPropertyAnimation>
#include "renamewindow.h"
#include "mkdirwindow.h"
#include "codeeditor.h"
#include "extensionseditor.h"

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
    int getSelectedTreeRow() const;
    void setSelectedTreeRow(int value);
    bool getCanClose() const;
    void setCanClose(bool value);
    void showErrorMessageBox(QString theMessage);
    void hideErrorMessageBox();

private slots:
    void on_actionE_xit_triggered();
    void on_action_Expand_all_triggered();
    void on_action_Collapse_all_triggered();
    void on_actionCopy_directory_triggered();
    void on_treeLeft_clicked(const QModelIndex &index);
    void on_treeRight_clicked(const QModelIndex &index);
    void on_action_Open_triggered();
    void on_actionRename_triggered();
    void on_actionMkdir_triggered();
    void on_actionDelete_triggered();
    void onChildTextChanged(bool state);
    void customMenuRequested(QPoint pos);
    void on_treeLeft_entered(const QModelIndex &index);
    void on_treeRight_entered(const QModelIndex &index);
    void on_action_About_Denka_Commander_triggered();
    void on_actionAbout_Qt5_triggered();
    void on_action_Options_triggered();
    void on_actionMove_triggered();
    void on_toolButton_CloseErrorBox_clicked();

public slots:
    void onIconSizeChange(int);

private:
    Ui::MainWindow *ui;
    QString currentPath = "";
    QFileSystemModel *model_left;
    QFileSystemModel *model_right;
    QFileSystemModel *activeModel;
    QProgressDialog *progress;    
    QProgressBar *statusProgress;
    QTreeView *m_ActiveTreeview;
    QString m_Tree;
    QList<QString> extensions;
    QPropertyAnimation *animateHeight;
    QSlider *f;
    Editor *ed;
    RenameWindow *renameWindow;
    MkdirWindow *mkdirWindow;  
    void copyFolder(QString sourceFolder, QString destFolder);
    void readSettings();
    void saveSettings();
    void setupAdditionalUI();
    void addExtensions();
    bool isInVector(QString extension);
    void countFilesInFolder(const QString &path);
    bool renameDir(const QString &oldName, const QString &newName);
    void verifySelection();
    int selectedTreeRow = 0;
    long m_totalNrOfFiles = 0;
    long m_filesCopied = 0;
    long m_totalCopied = 0;
    bool canClose;
    bool animatedErrorVisible = false;

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
