#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>
#include <QCloseEvent>
#include <QSettings>
#include <QRect>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->currentPath.clear();
    this->setWindowTitle("Denka Commander");
    this->setupAdditionalUI();
    // Left side browser
    this->model_left = new QFileSystemModel();
    this->model_left->setRootPath(QDir::homePath());
    ui->treeLeft->setModel(this->model_left);
    ui->treeLeft->setRootIndex(this->model_left->index(QDir::homePath()));
    // Nothing selected on the left!
    ui->treeLeft->setCurrentIndex(this->model_left->index(0));
    // Right side browser
    this->model_right = new QFileSystemModel();
    this->model_right->setRootPath(QDir::homePath());
    ui->treeRight->setModel(this->model_right);
    ui->treeRight->setRootIndex(this->model_right->index(QDir::homePath()));
    this->readSettings();
    this->setActiveTreeview(NULL);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::setupAdditionalUI
 */
void MainWindow::setupAdditionalUI() {
    // Add a progress bar to the statusbar...
    statusProgress = new QProgressBar();
    statusProgress->setMaximum(100);
    statusProgress->setMaximumWidth(100);
    ui->statusbar->addPermanentWidget(statusProgress);
    statusProgress->setValue(0);
}

/**
 * @brief MainWindow::ActiveTreeview
 * Handle treeview selection (left or right)
 * @return
 */
QTreeView *MainWindow::ActiveTreeview() const
{
    return m_ActiveTreeview;
}

/**
 * @brief MainWindow::setActiveTreeview
 * Tell the system what is the active treeview
 * @param ActiveTreeview
 */
void MainWindow::setActiveTreeview(QTreeView *ActiveTreeview)
{
    m_ActiveTreeview = ActiveTreeview;
//    if (m_ActiveTreeview) {
//        This is how to get the name of an object
//        qDebug() << m_ActiveTreeview->objectName();
//    }
}


/**
 * @brief Quit the application
 */
void MainWindow::on_actionE_xit_triggered()
{
    close();
}

/**
 * @brief MainWindow::closeEvent Overrides the close event of this form.
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "Shutdown app...";
    saveSettings();
    event->accept();
}

/**
 * @brief Write the settings of the form to the .conf file
 */
void MainWindow::saveSettings() {
    QSettings settings("DenkaTech","Denka Commander");
    settings.beginGroup("MainWindow");
    qDebug() << "Accessing ini at : " << settings.fileName();
    settings.setValue("form/geometry", saveGeometry());
    settings.endGroup();
}

/**
 * @brief MainWindow::readSettings Read configuration information from the .conf file
 * @param all
 */
void MainWindow::readSettings() {
    QSettings settings("DenkaTech","Denka Commander");
    qDebug() << "Reading config values in " << settings.fileName();
    settings.beginGroup("MainWindow");
    const QByteArray geometry = settings.value("form/geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        // If we have no settings yet, make sure the window has a reasonable size
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
    settings.endGroup();
}

/**
 * @brief Get the index of the currently selected item in the
 * active treeview and open the folder
 */
void MainWindow::on_action_Expand_all_triggered()
{
    // Find out who is the active treeview
    QModelIndex qi = this->ActiveTreeview()->currentIndex();
    this->ActiveTreeview()->expandRecursively(qi, -1);
}

/**
 * @brief Get the index of the currently selected item
 * in the active treeview and close the folder
 */
void MainWindow::on_action_Collapse_all_triggered()
{
    QModelIndex qi = this->ActiveTreeview()->currentIndex();
    this->ActiveTreeview()->collapse(qi);
}

/**
 * @brief Call the copy directory method
 */
void MainWindow::on_actionCopy_directory_triggered()
{
    // First find out what is selected on the left side
    QModelIndex leftIndex = ui->treeLeft->currentIndex();
    QString leftPath = this->model_left->fileInfo(leftIndex).absoluteFilePath();

    // Something selected on the left side?
    if (leftIndex.isValid()) {
        // Check the right side now...
        QModelIndex rightIndex = ui->treeRight->currentIndex();
        // Have we a selection there?
        if (rightIndex.isValid()) {
            QString rightPath = this->model_left->fileInfo(rightIndex).absoluteFilePath();
            QFileInfo pathInfo(leftPath);
            qDebug() << "WAZAAA" << pathInfo.baseName();

            // We are not on the same path huh?
            if (rightPath == leftPath) {
                QMessageBox::warning(this, "Warning", "Source and target folders are the same!");
                return;
            }
            // Is the selection on the right side a folder?
            if (!this->model_right->isDir(rightIndex)) {
                QMessageBox::warning(this, "Warning", "Target is not a directory!");
                return;
            } else {
                // Are we copying a file or a directory?
                if (this->model_left->isDir(leftIndex)) {
                    // Before starting the copy, we must make a directory in the target section,
                    // caution we need to take the name of the root that we copy and give it to
                    // the copy function.  Otherwise only the contents will be copied and not
                    // create a subdirectory in the target!
                    QString targetDir = rightPath + QDir::separator() + pathInfo.baseName();
                    if (QFile::exists(targetDir)) {
                        QMessageBox::StandardButton answer;
                        answer = QMessageBox::warning(this, "Attention", "Folder exists in target, overwrite?", QMessageBox::Yes|QMessageBox::No);
                        if (answer == QMessageBox::No) {
                            return;
                        }
                    }
                    this->copyFolder(leftPath, targetDir);
                    ui->statusbar->showMessage("Files copied to target directory!", 5000);
                } else {
                    // Must be a file if we get here, get the base filename
                    QFileInfo fi(leftPath);
                    QString absPath = fi.baseName();
                    QString targetPath = rightPath + QDir::separator() + absPath;
                    qDebug() << leftPath << " -> " << rightPath << " --> " << absPath;
                    if (QFile::exists(targetPath)) {
                        QMessageBox::StandardButton answer;
                        answer = QMessageBox::warning(this, "Attention", "File exists in target, overwrite?", QMessageBox::Yes|QMessageBox::No);
                        if (answer == QMessageBox::No) {
                            return;
                        }
                    }
                    QFile::copy(leftPath, targetPath);
                    ui->statusbar->showMessage("Files " + absPath + " has been copied to target directory!", 5000);
                }
            }
        } else {
            QMessageBox::information(this, "Info", "No valid target selection!");
        }
    } else {
        // No selection on the left
        QMessageBox::information(this,"Info", "No selection on source!");
    }
}

/**
 * @brief Recursively copy folder from left to right panel
 * @param sourceFolder
 * @param destFolder
 */
void MainWindow::copyFolder(QString sourceFolder, QString destFolder)
{
    // Get the directory on the left side
    QDir sourceDir(sourceFolder);

    if(!sourceDir.exists())
        return;
    // Directory on the right side (target)
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
    }
    // Get the files and copy them
    QStringList files = sourceDir.entryList(QDir::Files);
    qDebug() << "Nr of files " << files.count();
    double maxFiles = (double)files.count();
    for(int i = 0; i < files.count(); i++)
    {      
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);
        ui->statusbar->showMessage("Copying" + srcName + " to " + destName);
        double res = ((double)i / maxFiles) * 100;
        statusProgress->setValue(res);
        QCoreApplication::processEvents();

    }
    files.clear();
    // In case of a subdirectory, use recursion
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        copyFolder(srcName, destName);
    }
    statusProgress->setValue(0);
}


/**
 * @brief MainWindow::on_treeLeft_clicked
 * Track the fact that we entered the LEFT treeview and make that
 * treeview the active one in m_ActiveTreeview
 * @param index
 */
void MainWindow::on_treeLeft_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        this->setActiveTreeview(ui->treeLeft);
        qDebug() << this->m_ActiveTreeview->currentIndex();
    }
}

/**
 * @brief MainWindow::on_treeRight_clicked
 * Track the fact that we entered the RIGHT treeview and make that
 * treeview the active one in m_ActiveTreeview
 * @param index
 */
void MainWindow::on_treeRight_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        this->setActiveTreeview(ui->treeRight);
        qDebug() << this->m_ActiveTreeview->currentIndex();
    }
}
