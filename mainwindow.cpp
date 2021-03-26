#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Read the settings for the application */
    readSettings();
    canClose=true;
    currentPath.clear();
    setWindowTitle("Denka Commander");

    /* Left side browser */
    model_left = new QFileSystemModel();
    model_left->setRootPath(QDir::rootPath());

    /* Context menu for treeLeft */
    ui->treeLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeLeft->setModel(model_left);
    ui->treeLeft->setRootIndex(model_left->index("home"));
    ui->treeLeft->expandRecursively(QModelIndex(model_left->index(0)));
    ui->treeLeft->expand(QModelIndex(model_left->index("home")));
    model_left->sort(0, Qt::AscendingOrder);
    ui->treeLeft->setCurrentIndex(model_left->index(0));

    /* Right side browser */
    /* Context menu for treeRight */
    ui->treeRight->setContextMenuPolicy(Qt::CustomContextMenu);
    model_right = new QFileSystemModel();
    model_right->setRootPath(QDir::homePath());
    ui->treeRight->setModel(model_right);
    model_right->sort(0, Qt::AscendingOrder);
    ui->treeRight->setRootIndex(model_right->index(QDir::homePath()));

    setActiveModel(NULL);
    setActiveTreeview(ui->treeLeft);
    setupAdditionalUI();
    addExtensions();
    ed = new Editor(this);
    /* SIGNALS AND SLOTS */
    connect(ui->treeLeft, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));
//    connect(ui->treeLeft->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_TreeSelectionChanged(const QModelIndex &, const QModelIndex &)));
//    connect(ui->treeRight->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_TreeSelectionChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui->treeRight, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));
    // Pfew this took some time to make this work
    connect(ed, SIGNAL(texthasChanged(bool)), this, SLOT(onChildTextChanged(bool)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * @brief Quit the application
 */
void MainWindow::on_actionE_xit_triggered()
{  
    close();
}

/**
 * @brief MainWindow::addExtensions
 * The extensions list contains the extensions the
 * internal editor will handle
 */
void MainWindow::addExtensions()
{
    /* Open the extensions.txt file and read it in */
    QFile myFile("extensions.txt");
    // clear all items before reading file (avoid doubles)
    extensions.clear();
    if (!myFile.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this,"Extensions settings", "Can not open extensions.txt!");
        return;
    }
    QTextStream in(&myFile);
    while (!in.atEnd()) {
        QString contents = in.readLine();
        // Add to the qlist
        extensions.append(contents);
    }
    myFile.close();
}

bool MainWindow::isInVector(QString extension)
{
    // Check if this extension is in the commander editor list
    qDebug() << "isInVector called " << extension;
    bool result = false;
    // foreach is a Qt macro
    foreach(QString h, extensions) {
        if (h == extension) {
            result = true;
        }
    }
    qDebug() << " Result " << result;
    return result;
}

void MainWindow::verifySelection()
{
    QModelIndex selectedIndex = this->m_ActiveTreeview->currentIndex();
    QString filePath = activeModel->fileInfo(selectedIndex).absoluteFilePath();
    QFileInfo pathInfo(filePath);
    if (pathInfo.isExecutable()) {
        ui->actionMkdir->setEnabled(false);
        ui->action_Open->setEnabled(false);
    }
    if (pathInfo.isDir()) {
        // Can't edit a folder
        ui->action_Open->setEnabled(false);
        ui->actionMkdir->setEnabled(true);
    }
    if (pathInfo.isFile()) {
        ui->actionMkdir->setEnabled(false);
        ui->action_Open->setEnabled(true);
    }

}

bool MainWindow::getCanClose() const
{
    return canClose;
}

void MainWindow::setCanClose(bool value)
{
    canClose = value;
}

/**
 * @brief MainWindow::showErrorMessageBox
 * Show a nice error message
 * @param theMessage
 */
void MainWindow::showErrorMessageBox(QString theMessage)
{
    ui->label_Error->setText(theMessage);
    // Make it look nice when the error appears.
    animateHeight = new QPropertyAnimation(ui->frame_Error, "maximumHeight");
    animateHeight->setDuration(200);
    animateHeight->setStartValue(0);
    animateHeight->setEndValue(60);
    animateHeight->start();
    animatedErrorVisible = true;
}

void MainWindow::hideErrorMessageBox()
{
    animateHeight = new QPropertyAnimation(ui->frame_Error, "maximumHeight");
    animateHeight->setDuration(200);
    animateHeight->setStartValue(60);
    animateHeight->setEndValue(0);
    animateHeight->start();
    animatedErrorVisible = false;
}

/**
 * @brief MainWindow::setupAdditionalUI
 */
void MainWindow::setupAdditionalUI() {
    // Add a progress bar to the statusbar...
    QLabel *sliderLabel = new QLabel("Icon size");
    f = new QSlider();
    f->setMaximum(120);
    f->setOrientation(Qt::Horizontal);
    f->setMinimumWidth(40);
    f->setTickInterval(5);
    f->setMaximumWidth(40);
    f->setSingleStep(5);
    f->setMinimum(16);
    f->setValue(32);
    f->show();
    ui->toolBar_Sizer->addWidget(sliderLabel);
    ui->toolBar_Sizer->addWidget(f);
    connect(f, SIGNAL(valueChanged(int)), this, SLOT(onIconSizeChange(int)));
}

void MainWindow::onIconSizeChange(int) {
    QSize t(f->value(), f->value());
    ActiveTreeview()->setIconSize(t);
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


void MainWindow::customMenuRequested(QPoint pos)
{
    QModelIndex index = this->ActiveTreeview()->indexAt(pos);
    QMenu *menu = new QMenu(this);
    QAction *copyFileOrFolder = new QAction("Copy", this);
    QAction *renameFileOrFolder = new QAction("Rename", this);
    QAction *deleteFileOrFolder = new QAction("Delete", this);
    QAction *openFile = new QAction("Open", this);
    QAction *mkDir = new QAction("Mkdir", this);
    // Add custom menu actions
    menu->addAction(copyFileOrFolder);
    menu->addAction(renameFileOrFolder);
    menu->addAction(deleteFileOrFolder);
    menu->addAction(openFile);
    menu->addAction(mkDir);
    // connect signals and slots
    connect(renameFileOrFolder, SIGNAL(triggered()), this, SLOT(on_actionRename_triggered()));
    connect(copyFileOrFolder, SIGNAL(triggered()), this, SLOT(on_actionCopy_directory_triggered()));
    connect(deleteFileOrFolder, SIGNAL(triggered()), this, SLOT(on_actionDelete_triggered()));
    connect(openFile, SIGNAL(triggered()), this, SLOT(on_action_Open_triggered()));
    connect(mkDir, SIGNAL(triggered()), this, SLOT(on_actionMkdir_triggered()));
    // Do some housekeeping based on the type of file
    QString filePath = activeModel->fileInfo(index).absoluteFilePath();
    QFileInfo pathInfo(filePath);
    if (pathInfo.isExecutable()) {
        mkDir->setEnabled(false);
        openFile->setEnabled(false);
    }
    if (pathInfo.isDir()) {
        // Can't edit a folder
        openFile->setEnabled(false);
        mkDir->setEnabled(true);
    }
    if (pathInfo.isFile()) {
        mkDir->setEnabled(false);
        openFile->setEnabled(true);
    }
    // Activate the context menu
    menu->popup(ActiveTreeview()->viewport()->mapToGlobal(pos));
}

/**
 * @brief Write the settings of the form to the .conf file
 */
void MainWindow::saveSettings() {
    QSettings settings("DenkaTech","Denka Commander");
    settings.beginGroup("MainWindow");
    settings.setValue("form/geometry", saveGeometry());
    settings.setValue("form/settings", this->saveState());
    settings.endGroup();
    settings.beginGroup("Navigators");
    settings.setValue("nav/treeleft", ui->treeLeft->header()->saveState());
    settings.setValue("nav/treeleftsg", ui->treeLeft->iconSize());
    settings.setValue("nav/treeright", ui->treeRight->header()->saveState());
    settings.setValue("nav/treerightsg", ui->treeRight->iconSize());
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
    const QByteArray settingsF = settings.value("form/settings", QByteArray()).toByteArray();
    restoreState(settingsF);
    settings.endGroup();
    // Restore the configuration of the treeviews
    settings.beginGroup("Navigators");
    const QByteArray treeleft = settings.value("nav/treeleft", QByteArray()).toByteArray();
    QSize treelefttsg = settings.value("nav/treeleftsg").toSize();
    ui->treeLeft->setIconSize(QSize(treelefttsg));
    qDebug() << treelefttsg;

    ui->treeLeft->header()->restoreState(treeleft);
    const QByteArray treeright = settings.value("nav/treeright", QByteArray()).toByteArray();
    QSize treerightsg = settings.value("nav/treerightsg").toSize();
    ui->treeRight->setIconSize(QSize(treerightsg));

    qDebug() << treerightsg;
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
                } else if(this->model_left) {
                    // Must be a file if we get here, get the base filename
                    QFileInfo fi(leftPath);
                    QString absPath = fi.fileName();
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
        f->setValue(ui->treeLeft->iconSize().height());
    }
    verifySelection();

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
        setActiveModel(model_right);
        f->setValue(ui->treeRight->iconSize().height());
    }
    verifySelection();
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
            // Get the extension of this file
            QString extension = f.suffix();
            /* Now check if this file extension is in our private list */
            if (isInVector(extension)) {
                ed->setCurrentFile(f.absoluteFilePath());
                int res = ed->Open();
                qDebug() << res;
            } else {
                QString sbarInfo = QString("Trying to open %1").arg(f.absoluteFilePath());
                ui->statusbar->showMessage(sbarInfo);
                bool couldOpen = QDesktopServices::openUrl(QUrl::fromLocalFile(f.absoluteFilePath()));

                qDebug() << couldOpen;
                if (!couldOpen) {
                    QMessageBox::critical(this, "Info", "No ascociated application");
                } else {
                    ui->statusbar->showMessage("File opening", 2000);
                }
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

            renameWindow = new RenameWindow();
            if (f.isDir()) {
                // We are renaming a directory here
                renameWindow->setMainTitle("Rename folder : " + activeModel->fileInfo(selectedIndex).baseName());
            } else if (f.isFile()) {
                renameWindow->setMainTitle("Rename file : " + activeModel->fileInfo(selectedIndex).baseName());
            }
            renameWindow->setOldname(thePath);
            renameWindow->setNewName(activeModel->fileInfo(selectedIndex).filePath());
            int result = renameWindow->exec();
            if (result == DIALOGRESULTOK) {
                // Try to rename the file
                QFile f(renameWindow->NewName());
                if (f.exists()) {
                    QMessageBox::warning(this, "Rename", "Such a file/folder already exists!");
                    return;
                }
                renameDir(thePath, renameWindow->NewName());
            }
            delete renameWindow;
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
    if (!QFile::rename(src, dst)) {
        QMessageBox::information(this, "Rename", "Could not rename the file!");
    }
    return true;
}

/**
 * @brief MainWindow::on_actionMkdir_triggered
 * Create a new subdirectory
 */
void MainWindow::on_actionMkdir_triggered()
{
    if (this->ActiveTreeview() != nullptr) {
        QModelIndex selectedIndex = m_ActiveTreeview->currentIndex();
        if (selectedIndex.isValid()) {
            QString thePath = activeModel->fileInfo(selectedIndex).absoluteFilePath();            
            mkdirWindow = new MkdirWindow();
            mkdirWindow->setParentFolder(QDir::cleanPath(thePath));
            int result = mkdirWindow->exec();
            if (result == DIALOGRESULTOK) {
                // Go to the path
                QDir current(thePath);
                if (!current.mkdir(QDir::cleanPath(mkdirWindow->getNewFoldername()))) {
                    QMessageBox::warning(this,"MKDIR", "Could not create new subfolder!");
                }
            }
            delete mkdirWindow;
        }
    } else {
        QMessageBox::information(this, "MKDIR", "Please select a folder to create a subfolder in!");
    }
}

/**
 * @brief MainWindow::on_actionDelete_triggered
 * Remove the selected file/folder
 */
void MainWindow::on_actionDelete_triggered()
{
    /* Check if selection is file or folder */
    QModelIndex selectedIndex = m_ActiveTreeview->currentIndex();
    QString thePath = activeModel->fileInfo(selectedIndex).absoluteFilePath();
    // Obtain info about the selection
    QFileInfo selection(thePath);
    if (selection.isFile()) {
        qDebug() << "a File";
        QFile f(thePath);
        if (QMessageBox::question(this,"Delete file", "Delete " + thePath + "?", QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (f.remove()) {
                QMessageBox::information(this, "Delete", "File has been removed");
            }
        }
    } else if (selection.isDir()) {
        if (QMessageBox::warning(this, "Delete", "Delete this folder?", QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            QDir s(thePath);
            if (!s.removeRecursively()) {
                QMessageBox::warning(this, "Delete", "Could not complete removal!");
            } else {
                QMessageBox::information(this, "Delete", "Folder removed!");
            }
        }
    }
}

/**
 * @brief MainWindow::onChildTextChanged
 * Depending on the state of the child editor decide if we can close or not
 * @param state
 */
void MainWindow::onChildTextChanged(bool state)
{
    qDebug() << "TETTERKEN " << state;
    setCanClose(state);
    qDebug() << "TUTTER " << canClose;
}



void MainWindow::on_treeLeft_entered(const QModelIndex &index)
{
    if (index.isValid()) {
        setActiveTreeview(ui->treeLeft);
        setActiveModel(model_left);
    }
}

void MainWindow::on_treeRight_entered(const QModelIndex &index)
{
    if (index.isValid()) {
        setActiveTreeview(ui->treeRight);
        setActiveModel(model_right);
    }
}

/**
 * @brief MainWindow::on_action_About_Denka_Commander_triggered
 * Show an about box
 */
void MainWindow::on_action_About_Denka_Commander_triggered()
{
    QMessageBox::information(this, "About", "<b>Denka Commander</b><br>A "
        "system navigator based on Midnight Commander"
        "<br>By <b>DenkaTech</b> (Danny Van Geyte)<br>"
        "Copyright(c) 2021");
}

void MainWindow::on_actionAbout_Qt5_triggered()
{
    QApplication::aboutQt();
}

/**
 * @brief MainWindow::on_action_Options_triggered
 * Open options form
 */
void MainWindow::on_action_Options_triggered()
{
    ExtensionsEditor ed(this);
    int res = ed.exec();
    if (res == 0) {
        addExtensions();
    }
}

/**
 * @brief MainWindow::on_actionMove_triggered
 * Move a file/folder to a new location.  This is done
 * by renaming the file/folder!
 */
void MainWindow::on_actionMove_triggered()
{
    QModelIndex leftIndex = ui->treeLeft->currentIndex();
    QModelIndex rightIndex = ui->treeRight->currentIndex();
    if (leftIndex.isValid() && rightIndex.isValid()) {
        if (animatedErrorVisible) {
            hideErrorMessageBox();
        }
        QString leftPath = this->model_left->fileInfo(leftIndex).absoluteFilePath();
        QString rightPath = this->model_right->fileInfo(rightIndex).absoluteFilePath();
        QString newFileName = rightPath + QDir::separator() + this->model_left->fileInfo(leftIndex).baseName();
        QMessageBox::StandardButton answer;
        answer = QMessageBox::information(this, "Move",
                        "Move?", QMessageBox::Yes|QMessageBox::No);
        if (answer == QMessageBox::No) {
            return;
        }
        // Rename the file / Folder (this actually moves it)
        QFile fileToRename(leftPath);
        fileToRename.rename(newFileName);
    } else {

        showErrorMessageBox("Please select a file/folder on the left and a folder on the right!");
    }
}

/**
 * @brief MainWindow::on_toolButton_CloseErrorBox_clicked
 * Button to close the error label messagebox frame
 */
void MainWindow::on_toolButton_CloseErrorBox_clicked()
{
    hideErrorMessageBox();
}
