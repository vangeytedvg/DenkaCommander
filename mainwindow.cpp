#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renamewindow.h"
#include "codeeditor.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QModelIndex>
#include <QCloseEvent>
#include <QSettings>
#include <QDesktopWidget>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentPath.clear();
    setWindowTitle("Denka Commander");
    // Left side browser
    model_left = new QFileSystemModel();
    this->model_left->setRootPath(QDir::rootPath());
    //model_left->setRootPath(model_left->myComputer().toString());
    ui->treeLeft->setModel(this->model_left);
    //ui->treeLeft->setRootIndex(this->model_left->index(0));
    ui->treeLeft->setRootIndex(this->model_left->index("home"));
    ui->treeLeft->expandRecursively(QModelIndex(this->model_left->index(0)));
    ui->treeLeft->expand(QModelIndex(this->model_left->index("home")));
    model_left->sort(0, Qt::AscendingOrder);
    // Nothing selected on the left!
    ui->treeLeft->setCurrentIndex(this->model_left->index(0));
    // Right side browser
    model_right = new QFileSystemModel();
    model_right->setRootPath(QDir::homePath());
    ui->treeRight->setModel(this->model_right);
    model_right->sort(0, Qt::AscendingOrder);
    ui->treeRight->setRootIndex(this->model_right->index(QDir::homePath()));
    readSettings();
    setActiveModel(NULL);
    setActiveTreeview(ui->treeLeft);

    // Pfew this took some time to make this work
    connect(this->ui->treeLeft->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_TreeSelectionChanged(const QModelIndex &, const QModelIndex &)));
    connect(this->ui->treeRight->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_TreeSelectionChanged(const QModelIndex &, const QModelIndex &)));
}

MainWindow::~MainWindow()
{
    delete ui;
//    delete ed;
}

/**
 * @brief Quit the application
 */
void MainWindow::on_actionE_xit_triggered()
{  
    close();
}

/**
 * @brief MainWindow::on_TreeSelectionChanged
 * Keep track of what element is selected in the treeviews
 * @param current
 * @param previous
 */
void MainWindow::on_TreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous) {
    setSelectedTreeRow(current.row());
    //qDebug() << "Kwak" << " " << current << " " << previous << getSelectedTreeRow();
}

/**
 * @brief MainWindow::setupAdditionalUI
 */
void MainWindow::setupAdditionalUI() {
    // Add a progress bar to the statusbar...
    this->statusProgress = new QProgressBar();
    this->statusProgress->setMaximum(100);
    this->statusProgress->setMaximumWidth(100);
    ui->statusbar->addPermanentWidget(this->statusProgress);
    this->statusProgress->setValue(0);
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
}

/**
 * @brief MainWindow::getSelectedTreeRow
 * @return
 */
int MainWindow::getSelectedTreeRow() const
{
    return selectedTreeRow;
}

/**
 * @brief MainWindow::setSelectedTreeRow
 * @param value
 */
void MainWindow::setSelectedTreeRow(int value)
{
    selectedTreeRow = value;
}

/**
 * @brief MainWindow::closeEvent Overrides the close event of this form.
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event) {
//    if (ed) {
//        ed->close();
//    }
    saveSettings();
    event->accept();
}

/**
 * @brief MainWindow::getActiveModel
 * @return
 */
QFileSystemModel *MainWindow::getActiveModel() const
{
    return activeModel;
}

/**
 * @brief MainWindow::setActiveModel
 * @param value
 */
void MainWindow::setActiveModel(QFileSystemModel *value)
{
    activeModel = value;
}


/**
 * @brief Write the settings of the form to the .conf file
 */
void MainWindow::saveSettings() {
    QSettings settings("DenkaTech","Denka Commander");
    settings.beginGroup("MainWindow");
    settings.setValue("form/geometry", saveGeometry());
    settings.endGroup();
    settings.beginGroup("Navigators");
    settings.setValue("nav/treeleft", ui->treeLeft->header()->saveState());
    settings.setValue("nav/treeright", ui->treeRight->header()->saveState());
    settings.endGroup();
}

/**
 * @brief MainWindow::readSettings Read configuration information from the .conf file
 * @param all
 */
void MainWindow::readSettings() {
    QSettings settings("DenkaTech","Denka Commander");

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
    // Restore the configuration of the treeviews
    settings.beginGroup("Navigators");
    const QByteArray treeleft = settings.value("nav/treeleft", QByteArray()).toByteArray();
    ui->treeLeft->header()->restoreState(treeleft);
    const QByteArray treeright = settings.value("nav/treeright", QByteArray()).toByteArray();
    ui->treeRight->header()->restoreState(treeright);
    settings.endGroup();
}

/**
 * @brief Get the index of the currently selected item in the
 * active treeview and open the folder
 */
void MainWindow::on_action_Expand_all_triggered()
{
    // Find out who is the active treeview
    if (ActiveTreeview() != nullptr) {
        QModelIndex qi = this->ActiveTreeview()->currentIndex();
        this->ActiveTreeview()->expandRecursively(qi, -1);
    } else {
        QMessageBox::warning(this, "Warning", "Select a folder to expand!");
    }
}

/**
 * @brief Get the index of the currently selected item
 * in the active treeview and close the folder
 */
void MainWindow::on_action_Collapse_all_triggered()
{
    if (ActiveTreeview() != nullptr) {
        QModelIndex qi = this->ActiveTreeview()->currentIndex();
        this->ActiveTreeview()->collapse(qi);
    } else {
        QMessageBox::warning(this, "Warning", "Select a folder to collapse!");
    }
}

/**
 * @brief Call the copy directory method
 */
void MainWindow::on_actionCopy_directory_triggered()
{
    QModelIndex leftIndex = ui->treeLeft->currentIndex();
    QString leftPath = this->model_left->fileInfo(leftIndex).absoluteFilePath();
    this->m_totalNrOfFiles = 0;
    this->countFilesInFolder(leftPath);

    QString info = QString::number(this->m_totalNrOfFiles);
    ui->statusbar->showMessage("Nr of files to copy: " + info);
    QCoreApplication::processEvents();
    if (this->m_totalNrOfFiles > 1000) {
        QMessageBox::StandardButton answer;
        answer = QMessageBox::information(this,
                    "Copy",
                    "There are a large nr of files to copy, this can take a long time. Continue?",
                    QMessageBox::Yes|QMessageBox::No);
        if (answer == QMessageBox::No) {
            this->m_totalNrOfFiles = 0;
            return;
        }
    }
    // Something selected on the left side?
    if (leftIndex.isValid()) {
        // Check the right side now...
        QModelIndex rightIndex = ui->treeRight->currentIndex();
        // Have we a selection there?
        if (rightIndex.isValid()) {
            QString rightPath = this->model_left->fileInfo(rightIndex).absoluteFilePath();
            QFileInfo pathInfo(leftPath);            
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
                QCoreApplication::processEvents();
                ui->statusbar->showMessage("Copy started...");
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
                            ui->statusbar->showMessage("Copy cancelled...", 5000);
                            return;
                        }
                    }

                    this->progress = new QProgressDialog(this);
                    this->progress->setMaximum(this->m_totalNrOfFiles);
                    this->progress->setWindowTitle("Copying files");
                    this->progress->show();
                    this->copyFolder(leftPath, targetDir);
                    ui->statusbar->showMessage(info + " file(s) copied to target directory!", 5000);
                    this->progress->hide();
                    delete this->progress;
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
                            ui->statusbar->showMessage("Copy cancelled...", 5000);
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
    ui->statusbar->showMessage("Done", 5000);
}

/**
 * @brief MainWindow::countFilesInFolder
 * Counts the files in the selected path
 * @param path
 */
void MainWindow::countFilesInFolder(const QString &path)
{
    // Get the directory on the left side
    QDir sourceDir(path);
    // Get the files and copy them
    QStringList files = sourceDir.entryList(QDir::Files);
    this->m_totalNrOfFiles += files.count();

    files.clear();
    // In case of a subdirectory, use recursion
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = path + "/" + files[i];
        countFilesInFolder(srcName);
    }

    if (this->m_totalNrOfFiles % 5 == 0) {
        // Convert long to string in QT
        QString info = QString::number(this->m_totalNrOfFiles);
        ui->statusbar->showMessage("Nr of files :" + info);
        QCoreApplication::processEvents();
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

    this->m_totalCopied += 1;
    QCoreApplication::processEvents();
    this->progress->setValue(this->m_totalCopied);

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

    this->m_filesCopied += files.count();
    for(int i = 0; i < files.count(); i++)
    {      
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);      
        this->progress->setLabelText(tr("Copying %1").arg(files[i]));
    }
    files.clear();
    // In case of a subdirectory, use recursion
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    this->m_filesCopied += files.count();
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        copyFolder(srcName, destName);
    }
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
        setActiveTreeview(ui->treeLeft);
        setActiveModel(model_left);
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
        setActiveTreeview(ui->treeRight);
    }
}

/**
 * @brief MainWindow::on_action_Open_triggered
 * Open a file for viewing
 */
void MainWindow::on_action_Open_triggered()
{
    // Get the name of the selected file, if its a folder refuse that
    if (this->ActiveTreeview() != nullptr) {
        QModelIndex selectedIndex = this->m_ActiveTreeview->currentIndex();
        if (selectedIndex.isValid()) {
            QString leftPath = this->model_left->fileInfo(selectedIndex).absoluteFilePath();
            QFileInfo f(leftPath);
            if (f.isDir()) {
                QMessageBox::information(this,"Open", "Only files can be edited!");
                return;
            }
            // Like the f"" string in Python:
            QString sbarInfo = QString("Trying to open %1").arg(f.absoluteFilePath());
            ui->statusbar->showMessage(sbarInfo);
//            bool couldOpen = QDesktopServices::openUrl(QUrl::fromLocalFile(f.absoluteFilePath()));
            bool couldOpen = true;
            ed = new Editor(this);
            ed->setCurrentFile(f.absoluteFilePath());
            int res = ed->Open();
            qDebug() << res;
            if (!couldOpen) {
                QMessageBox::critical(this, "Info", "No ascociated application");
            } else {
                ui->statusbar->showMessage("File opening", 2000);
            }
        }
    }

}

/**
 * @brief MainWindow::on_actionRename_triggered
 * Rename the selection
 */
void MainWindow::on_actionRename_triggered()
{
    if (this->ActiveTreeview() != nullptr) {
        qDebug() << "Rename called";
        QModelIndex selectedIndex = m_ActiveTreeview->currentIndex();
        if (selectedIndex.isValid()) {
            QString thePath = activeModel->fileInfo(selectedIndex).absoluteFilePath();
            // Obtain info about the selection
            QFileInfo f(thePath);
            RenameWindow w;
            if (f.isDir()) {
                // We are renaming a directory here
                w.setMainTitle("Rename folder : " + activeModel->fileInfo(selectedIndex).baseName());
            } else if (f.isFile()) {
                w.setMainTitle("Rename file : " + activeModel->fileInfo(selectedIndex).baseName());
            }
            w.setOldname(thePath);
            w.setNewName(activeModel->fileInfo(selectedIndex).filePath());
            int result = w.exec();
            if (result == DIALOGRESULTOK) {
                // Try to rename the file
                QFile f(w.NewName());
                if (f.exists()) {
                    QMessageBox::warning(this, "Rename", "Such a file/folder already exists!");
                    return;
                }
                try {
                    renameDir(thePath, w.NewName());
                }  catch (...) {
                    QMessageBox::warning(this, "Rename", "An error occured!");
                }
            }
        }
    } else {
        QMessageBox::information(this, "Rename", "Please select a file or folder to rename!");
    }
}

/**
 * @brief MainWindow::renameDir
 * @abstract Rename a directory or file
 * @param oldName
 * @param newName
 * @return
 */
bool MainWindow::renameDir(const QString &oldName, const QString &newName) {
    // Cleanpath normalizes a path eg \ becomes /
    QString src = QDir::cleanPath(oldName);
    QString dst = QDir::cleanPath(newName);
    // Rename
    bool rc = QFile::rename(src, dst);
    if (!rc) {
        QMessageBox::information(this, "Rename", "Could not rename the file!");
    }
    return rc;
}

/**
 * @brief MainWindow::on_actionMkdir_triggered
 * Create a new subdirectory
 */
void MainWindow::on_actionMkdir_triggered()
{
    if (this->ActiveTreeview() != nullptr) {
        qDebug() << "MKDIR called";
        QModelIndex selectedIndex = m_ActiveTreeview->currentIndex();
        if (selectedIndex.isValid()) {
            QString thePath = activeModel->fileInfo(selectedIndex).absoluteFilePath();
            // Obtain info about the selection
            QFileInfo f(thePath);
            RenameWindow w;
            if (f.isDir()) {
                // We are renaming a directory here
                w.setMainTitle("MKDIR : " + activeModel->fileInfo(selectedIndex).baseName());
            } else {
                QMessageBox::information(this, "MKDIR", "Selection is not a folder");
                return;
            }
            w.setOldname(thePath);
            w.setNewName(activeModel->fileInfo(selectedIndex).filePath());
            int result = w.exec();
            if (result == DIALOGRESULTOK) {
                // Try to rename the file
                QFile f(w.NewName());
                if (f.exists()) {
                    QMessageBox::warning(this, "Rename", "Such a file/folder already exists!");
                    return;
                }
                try {
                    renameDir(thePath, w.NewName());
                }  catch (...) {
                    QMessageBox::warning(this, "Rename", "An error occured!");
                }
            }
        }
    } else {
        QMessageBox::information(this, "Rename", "Please select a file or folder to rename!");
    }
}
