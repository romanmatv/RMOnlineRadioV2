#include "inputdialog.h"
#include "ui_inputdialog.h"

inputDialog::inputDialog(QWidget *parent, QList<Input> inptList, QString title) :
    QDialog(parent),
    ui(new Ui::inputDialog),
    inputs(inptList)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    if (inputs.count()>0)
        rebuildInputs();
}

inputDialog::~inputDialog()
{
    destroyInputs();
    delete ui;
}


void inputDialog::setInputs(QList<Input> inpts){
    inputs = inpts;
    rebuildInputs();
}

void inputDialog::setInputs(Input inpts){
    inputs = QList<Input>{inpts};
    rebuildInputs();
}

void inputDialog::addInputs(Input inpts){
    inputs.append(inpts);
    rebuildInputs();
}

void inputDialog::addInputs(QList<Input> inpts){
    inputs.append(inpts);
    rebuildInputs();
}

void inputDialog::clearInputs(){
    inputs.clear();
    destroyInputs();
}

void inputDialog::rebuildInputs(){
    destroyInputs();
    for (int i=0; i<inputs.count(); i++){
        edits.append(new QLineEdit);
        labels.append(new QLabel);

        edits[i]->setText(inputs[i].value);
        labels[i]->setText(inputs[i].title);

        ui->lay->addWidget(labels[i]);
        ui->lay->addWidget(edits[i]);
    }
}

QStringList inputDialog::getInputs(){
    QStringList values;
    for(int i=0; i<edits.count(); i++){
        values.append(edits[0]->text());
    }
    return values;
}

void inputDialog::destroyInputs(){
    if (edits.count()>0){
        for (int i=0; i<edits.count(); i++)
            delete edits[i];
    }
    edits.clear();
    if (labels.count()>0){
        for (int i=0; i<labels.count(); i++)
            delete labels[i];
    }
    labels.clear();
}
