#ifndef EQUALIZERWINDOW_H
#define EQUALIZERWINDOW_H

#include <QWidget>
#include <AudioThread/AudioThread>

namespace Ui {
class EqualizerWindow;
}

class EqualizerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EqualizerWindow(QWidget *parent = nullptr, AudioThread *_audio = nullptr);
    ~EqualizerWindow();
    void rowEqToArr(float *arr, QString preset);
    void saveEqBands(float *arr, QString preset);
    void saveEqBand(int band, float gain, QString preset);

private slots:
    void on_eq_0_sliderMoved(int position);
    void on_combo_presets_currentIndexChanged(int index);

    void on_eq_enabled_clicked();

private:
    Ui::EqualizerWindow *ui;
    QScopedPointer<AudioThread> audio;
    void updateGraph();
    void updateSliders();
    QString slash, localPath;
    bool enabledEQ = false;
};

#endif // EQUALIZERWINDOW_H
