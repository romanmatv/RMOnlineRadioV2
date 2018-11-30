#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
//#include "painter.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
    //void setPainter(Painter* paint){painter.reset(paint);}

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushButton_activate_clicked();

    void on_pushButton_deactivate_clicked();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
