#include "extensionseditor.h"
#include "ui_extensionseditor.h"

ExtensionsEditor::ExtensionsEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionsEditor)
{
    ui->setupUi(this);
    this->loadExtensions();

}

ExtensionsEditor::~ExtensionsEditor()
{
    delete ui;
}

/**
 * @brief ExtensionsEditor::loadExtensions
 * Load file with extensions that the internal editor will handle
 */
void ExtensionsEditor::loadExtensions()
{
    QFile f("./extensions.txt");
    if (f.exists()) {
        if (!f.open(QIODevice::ReadOnly | QFile::Text)) {
            return;
        }
        ui->edit_Extensions->setPlainText(f.readAll());
        f.close();
    }
}

void ExtensionsEditor::on_pushButton_Save_clicked()
{
    QFile file("./extensions.txt");
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    QTextStream out(&file);
    QString text = ui->edit_Extensions->toPlainText();
    out << text;
    file.close();
    close();
}

void ExtensionsEditor::on_pushButton_Cancel_clicked()
{
    close();
}
