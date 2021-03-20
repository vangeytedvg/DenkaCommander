#include "renamewindow.h"
#include "ui_renamewindow.h"

/**
 * @brief RenameWindow::RenameWindow
 * CTOR
 * @param parent
 */
RenameWindow::RenameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameWindow)
{
    ui->setupUi(this);
}

/**
 * @brief RenameWindow::~RenameWindow
 * DTOR
 */
RenameWindow::~RenameWindow()
{
    delete ui;
}

/**
 * @brief RenameWindow::MainTitle
 * Get the MainTitle
 * @return
 */
QString RenameWindow::MainTitle() const
{
    return m_MainTitle;
}

/**
 * @brief RenameWindow::setMainTitle
 * Set the title of the form
 * @param MainTitle
 */
void RenameWindow::setMainTitle(const QString &MainTitle)
{
    m_MainTitle = MainTitle;
    setWindowTitle(m_MainTitle);
}

/**
 * @brief RenameWindow::Oldname
 * @return The old name of the file or folder
 */
QString RenameWindow::Oldname() const
{
    return m_Oldname;
}

/**
 * @brief RenameWindow::setOldname
 * Set the old name of the file
 * @param Oldname
 */
void RenameWindow::setOldname(const QString &Oldname)
{
    m_Oldname = Oldname;
    ui->lineEdit_OldName->setText(m_Oldname);
}

/**
 * @brief RenameWindow::NewName
 * @return The newname of the file
 */
QString RenameWindow::NewName()
{
    m_NewName = ui->lineEdit_NewName->text();
    return m_NewName;
}

/**
 * @brief RenameWindow::setNewName
 * @param NewName Set the new name of the file or folder
 */
void RenameWindow::setNewName(const QString &NewName)
{
    m_NewName = NewName;
    ui->lineEdit_NewName->setText(m_NewName);
}
