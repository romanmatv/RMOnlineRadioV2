#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QSettings gr("C:\\Users\\Roman\\YandexDisk\\Cpp\\QtProjects\\RMOnlineRadio\\gradient.ini", QSettings::IniFormat);
    //QPointF(50, -20), QPointF(80, 20)
    ui->lineEdit->setText(gr.value("gr/x1", 50).toString());
    ui->lineEdit_2->setText(gr.value("gr/y1", -20).toString());
    ui->lineEdit_3->setText(gr.value("gr/x2", 80).toString());
    ui->lineEdit_4->setText(gr.value("gr/y2", 20).toString());
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_clicked(QAbstractButton *button)
{

}

void Dialog::on_buttonBox_accepted()
{

}

void Dialog::on_buttonBox_rejected()
{
    close();
}

void Dialog::on_pushButton_clicked()
{
    QSettings gr("C:\\Users\\Roman\\YandexDisk\\Cpp\\QtProjects\\RMOnlineRadio\\gradient.ini", QSettings::IniFormat);
    gr.setValue("gr/x1", ui->lineEdit->text());
    gr.setValue("gr/y1", ui->lineEdit_2->text());
    gr.setValue("gr/x2", ui->lineEdit_3->text());
    gr.setValue("gr/y2", ui->lineEdit_4->text());
    gr.sync();
}
