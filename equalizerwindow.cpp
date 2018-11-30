#include "equalizerwindow.h"
#include "ui_equalizerwindow.h"

EqualizerWindow::EqualizerWindow(QWidget *parent, AudioThread *_audio) :
    QWidget(parent),
    ui(new Ui::EqualizerWindow)
{
    ui->setupUi(this);
    if (_audio){
        audio.reset(_audio);
    }


#ifndef win32
slash = "\\";
#else
slash = "/";
#endif
    localPath = QCoreApplication::applicationDirPath()+slash;

    QSettings eq_presets(localPath+"EQPresetsLibrary.ini", QSettings::IniFormat);
    QSettings sets;

    connect(ui->eq_0, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_1, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_2, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_3, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_4, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_5, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_6, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_7, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_8, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_9, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_10, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_11, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_12, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_13, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_14, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_15, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_16, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));
    connect(ui->eq_17, SIGNAL(sliderMoved(int)), this, SLOT(on_eq_0_sliderMoved(int)));

    ui->combo_presets->addItem("Свое");
    ui->combo_presets->addItems(eq_presets.childGroups());

    enabledEQ = sets.value("eq/enabled", enabledEQ).toBool();
    ui->eq_enabled->setChecked(enabledEQ);

    updateSliders();
    updateGraph();
}

EqualizerWindow::~EqualizerWindow()
{
    QSettings sets;
    sets.sync();
    delete ui;
}

void EqualizerWindow::updateSliders(){
    QSettings sets;
    QSlider *findObj;

    for (int i=0;i<18;i++){
        findObj = this->findChild<QSlider *>("eq_"+QString::number(i));
        findObj->setValue(sets.value("eq/bands/"+QString::number(i), 0).toInt());
    }
}

void EqualizerWindow::on_eq_0_sliderMoved(int position)
{
    QSettings sets;
    QString obj = sender()->objectName();
    obj = obj.split("eq_")[1];
    sets.setValue("eq/bands/"+obj, position);
    sets.setValue("eq/selected", 1);
    saveEqBand(obj.toInt(), position, "eq/custom");
    updateGraph();
    if (enabledEQ){
        audio.data()->updateDSP_EQ(&sets);
    }
}

void EqualizerWindow::updateGraph(){
    QSettings sets;

    //Рисуем график y=x*x

    //Сгенерируем данные
    //Для этого создадим два массива точек:
    //один для созранения x координат точек,
    //а второй для y соответственно

    double a = 0; //Начало интервала, где рисуем график по оси Ox
    double b =  17; //Конец интервала, где рисуем график по оси Ox
    double h = 1; //Шаг, с которым будем пробегать по оси Ox

    int N=(b-a)/h + 2; //Вычисляем количество точек, которые будем отрисовывать
    QVector<double> x(N), y(N); //Массивы координат точек

    //Вычисляем наши данные
    int i=0;
    for (double X=a; X<=b; X+=h)//Пробегаем по всем точкам
    {
        x[i] = i;
        y[i] = sets.value("eq/bands/"+QString::number(i), 0).toInt(); //Формула нашей функции
        i++;
    }

    ui->eq_plot->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    ui->eq_plot->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    ui->eq_plot->graph(0)->setData(x, y);

    ui->eq_plot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точки
    ui->eq_plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    //формируем вид точек
    ui->eq_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    //Подписываем оси Ox и Oy
    ui->eq_plot->xAxis->setLabel("Bands");
    ui->eq_plot->yAxis->setLabel("Gain");

    //Установим область, которая будет показываться на графике
    ui->eq_plot->xAxis->setRange(a, b);//Для оси Ox
    ui->eq_plot->yAxis->setRange(-15, 15);//Для оси Oy

    //И перерисуем график на нашем widget
    ui->eq_plot->replot();
}

void EqualizerWindow::rowEqToArr(float *arr, QString preset){
    QSettings eq_presets(localPath+"EQPresetsLibrary.ini", QSettings::IniFormat);
    for (int i=0; i<18; i++){
        arr[i] = eq_presets.value(preset+"/Band"+QString::number(i)).toFloat();
    }
}

void EqualizerWindow::saveEqBands(float *arr, QString preset){
    QSettings set;
    for (int i=0; i<18; i++){
        set.setValue(preset+"/Band"+QString::number(i), arr[i]);
    }
}

void EqualizerWindow::saveEqBand(int band, float gain, QString preset){
    QSettings set;
    set.setValue(preset+"/Band"+QString::number(band), gain);
}

void EqualizerWindow::on_combo_presets_currentIndexChanged(int index)
{
    QSettings sets;
    float bands[18];
    QString arg1;
    arg1 = ui->combo_presets->itemData(index).toString();
    sets.setValue("eq/selected", index+2);
    rowEqToArr(bands, arg1);
    saveEqBands(bands, "eq");
    updateGraph();
    if (enabledEQ){
        audio.data()->updateDSP_EQ(&sets);
    }
}

void EqualizerWindow::on_eq_enabled_clicked()
{
    QSettings sets;
    enabledEQ = ui->eq_enabled->isChecked();
    sets.setValue("eq/enabled", enabledEQ);

    if (enabledEQ){
        qDebug() << "установка DSP";
        audio.data()->setDSP_EQ(&sets);
    }else{
        qDebug() << "снятие DSP";
        audio.data()->unsetDSP_EQ();
    }
}
