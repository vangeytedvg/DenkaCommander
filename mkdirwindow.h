#ifndef MKDIRWINDOW_H
#define MKDIRWINDOW_H

#include <QDialog>

namespace Ui {
class mkdirwindow;
}

class mkdirwindow : public QDialog
{
    Q_OBJECT

public:
    explicit mkdirwindow(QWidget *parent = nullptr);
    ~mkdirwindow();

private:
    Ui::mkdirwindow *ui;
};

#endif // MKDIRWINDOW_H
