#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>

namespace Ui {
class inputDialog;
}

class inputDialog : public QDialog
{
    Q_OBJECT

public:
    struct Input{
        QString title;
        QString value = "";
    };
    explicit inputDialog(QWidget *parent = nullptr, QList<Input> = QList<Input>(), QString title = "Диалог");
    void setInputs(QList<Input>);
    void setInputs(Input);
    void addInputs(Input);
    void addInputs(QList<Input>);
    QStringList getInputs();
    ~inputDialog();

private:
    Ui::inputDialog *ui;
    QList<Input> inputs;
    QList<QLineEdit*> edits;
    QList<QLabel*> labels;
    void clearInputs();
    void rebuildInputs();
    void destroyInputs();
};

#endif // INPUTDIALOG_H
