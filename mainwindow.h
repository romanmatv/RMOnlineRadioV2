#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtCore/qglobal.h>
#include <QMainWindow>
#include <QSettings>
#include <QLabel>
#include <QGLWidget>
#include <QtOpenGL>
#include <QtMultimedia/QMediaPlaylist>
#include <QMessageBox>
#include <MetaData/MetaData>
#include <QFileDialog>
#include <AudioThread/AudioThread>
#include "settingsdialog.h"
#include "sf_paint.h"
#include "gl_paint.h"
#include "inputdialog.h"
#include "trackmodel.h"
#include "trackdelegate.h"
#include "equalizerwindow.h"

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addToPlaylist(const QList<QUrl> &urls);
public slots:
    bool turnSoftRender();
    bool turnHardRender();
    void updateScences();

private slots:
    void on_btn_PlayPause_clicked();
    void on_btn_Stop_clicked();
    void on_playlistLoaded();
    void on_volumeSlider_sliderMoved(int position);
    void playlistPositionChanged(int);
    void jump(const QModelIndex &index);
    void jumpi(int index);
    void on_actionOpenSettings_triggered();
    void on_btn_mute_clicked();
    void on_btn_add_clicked();
    void setVolume();
    void setVolume(int value);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void on_btn_open_clicked();
    void on_actionSave_triggered();
    void on_btn_Next_clicked();
    void on_btn_Prew_clicked();
    void changeListValue();
    void deleteListValue();
    void on_playlistView_customContextMenuRequested(const QPoint &pos);
    void VisContextMenu(const QPoint &pos);
    void swichFullScreen();
    void switchScene();
    void metaUpdated(Track*);
    void on_btn_eq_clicked();
    void readAndApplaySets();

    void leftChan(int);
    void rightChan(int);

    void on_actionAbout_triggered();

    void on_action_aboutQt_triggered();

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void updateDurationInfo(qint64 currentInfo);
    QString updateMetaData();

    static long map(long x, long in_min, long in_max, long out_min, long out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    static float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    static int map(float x, float in_min, float in_max, int out_min, int out_max)
    {
      return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    }
    template <typename T>
    static T map(T x, T in_min, T in_max, T out_min, T out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    Qt::WindowStates intToWindowState(int state){
        switch(state){
            case 0x00000000: return Qt::WindowNoState;
            case 0x00000001: return Qt::WindowMinimized;
            case 0x00000002: return Qt::WindowMaximized;
            case 0x00000004: return Qt::WindowFullScreen;
            case 0x00000008: return Qt::WindowActive;
        }
        return Qt::WindowNoState;
    }

    int windowStateToInt(Qt::WindowStates state){
        switch(state){
            case Qt::WindowNoState : return 0x00000000;
            case Qt::WindowMinimized : return 0x00000001;
            case Qt::WindowMaximized : return 0x00000002;
            case Qt::WindowFullScreen : return 0x00000004;
            case Qt::WindowActive : return 0x00000008;
        }
        return 0x00000000;
    }

    QSettings settings;

    Ui::MainWindow *ui;
    const qreal kVolumeInterval = 0.04;
    QLabel* lbl_mdeiaStatus = nullptr;

    QString m_trackInfo;
    QString m_statusInfo;

    QString mFile;
    QString mTitle;

    TrackModel *trackModel = nullptr;
    AudioThread *audioThread = nullptr;
    int position = -1;

    SFPaint *sf_win = nullptr;
    GLPaint *gl_win = nullptr;
    Painter *painter = nullptr;

    QString slash, localPath;
    bool hasOpenGL;

    QList<QAction*> actions;

    QTimer *sets_timer;

};

#endif // MAINWINDOW_H
