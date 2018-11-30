#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "mainwindow.h"
#include "crypto.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings;

    ui->checkBox_autoplay->setChecked(settings.value("play/autoplay", false).toBool());
    ui->checkBox_savelist->setChecked(settings.value("play/savelist", true).toBool());
    ui->checkBox_savestation->setChecked(settings.value("play/savestation", true).toBool());
    //ui->checkBox_kalmanfiltr->setChecked(settings.value("visual/filtr", true).toBool());

    if (settings.value("key/machine", "").toString().isEmpty()){
        settings.setValue("key/machine", Crypto::generateMachine());
        settings.sync();
    }

    ui->lineEdit_machineCode->setText(settings.value("key/machine", Crypto::generateMachine()).toString());
    ui->lineEdit_serialKey->setText(settings.value("key/serial", "").toString());
    ui->lineEdit_username->setText(settings.value("key/username", "").toString());
    if (Crypto::validSerial(settings.value("key/serial", "").toString(),settings.value("key/machine", Crypto::generateMachine()).toString(),settings.value("key/username", "").toString())){
        ui->label_activateStatus->setText("Программа активирована :)");
        ui->pushButton_activate->setEnabled(false);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Save){
        QSettings settings;
        settings.setValue("play/autoplay", ui->checkBox_autoplay->checkState());
        settings.setValue("play/savelist", ui->checkBox_savelist->checkState());
        settings.setValue("play/savestation", ui->checkBox_savestation->checkState());
        //settings.setValue("visual/filtr", ui->checkBox_kalmanfiltr->checkState());
        settings.sync();
    }
    SettingsDialog::close();
}

void SettingsDialog::on_pushButton_activate_clicked()
{
    QString serial, username, machine;
    serial = ui->lineEdit_serialKey->text();
    username = ui->lineEdit_username->text();
    machine = ui->lineEdit_machineCode->text();
    if (Crypto::validSerial(serial, machine, username)){
        QSettings settings;
        settings.setValue("key/serial", serial);
        settings.setValue("key/username", username);
        settings.sync();
        ui->label_activateStatus->setText("Программа активирована :)");
        ui->pushButton_activate->setEnabled(false);
    }else{
        QApplication::beep();
    }
}

void SettingsDialog::on_pushButton_deactivate_clicked()
{
    QSettings settings;
    ui->pushButton_activate->setEnabled(true);
    ui->label_activateStatus->setText("Программа не активирована. Вам доступно только 3 визуализации.");
    ui->lineEdit_serialKey->setText("");
    ui->lineEdit_username->setText("");
    settings.setValue("key/serial", "");
    settings.setValue("key/username", "");
    settings.sync();
}
