#ifndef MKDIRWINDOW_H
#define MKDIRWINDOW_H

#include <QDialog>

namespace Ui {
class MkdirWindow;
}

class MkdirWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MkdirWindow(QWidget *parent = nullptr);
    ~MkdirWindow();

    QString getParentFolder() const;
    void setParentFolder(const QString &value);

    QString getNewFoldername();
    void setNewFoldername(const QString &value);

private:
    Ui::MkdirWindow *ui;
    QString parentFolder;
    QString newFoldername;
};

#endif // MKDIRWINDOW_H
