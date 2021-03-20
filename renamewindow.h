#ifndef RENAMEWINDOW_H
#define RENAMEWINDOW_H

#include <QDialog>

namespace Ui {
class RenameWindow;
}

class RenameWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RenameWindow(QWidget *parent = nullptr);
    ~RenameWindow();

    QString MainTitle() const;
    void setMainTitle(const QString &MainTitle);

    QString Oldname() const;
    void setOldname(const QString &Oldname);

    QString NewName();
    void setNewName(const QString &NewName);

private:
    QString m_MainTitle;
    QString m_Oldname;
    QString m_NewName;

    Ui::RenameWindow *ui;
};

#endif // RENAMEWINDOW_H
