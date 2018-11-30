#ifndef PAINTER_H
#define PAINTER_H

#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QLibrary>
#include <AudioThread/AudioThread>
#include <QMouseEvent>
#include "crypto.h"

class Painter : public QObject{
    Q_OBJECT
public:
    Painter(QSize Wsize = QSize(200,200), AudioThread *_tread = nullptr);
    ~Painter();
    static long map(long x, long in_min, long in_max, long out_min, long out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    static float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    struct Info {
        bool operator == (const Info &rhs) const{
            return (this->author==rhs.author && this->description==rhs.description && this->name == rhs.name && this->version == rhs.version);
        }
        bool isEmpty(){
            return  (name.isEmpty()&&author.isEmpty()&&description.isEmpty()&&version.isEmpty()&&filename.isEmpty());
        }
        void clear(){
            name.clear();author.clear();description.clear();version.clear();filename.clear();
        }
        QString name;
        QString author;
        QString description;
        QString version;
        QString filename;
        int msAni = 30; //частота вызова анимации, 0 = default = 30
    };

public slots:
    //Слоты анимации//
    void animate();
    void paint(QPainter*,QPaintEvent*);
    void setSize(QSize);
    //Слоты библиотек//
    void scanLibs();
    QList<Info> getLibs(){return libs;}
    bool setLib(int i){return setLib(libs[i].filename);}
    bool setLib(Info lib){return setLib(lib.filename);}
    bool setLib(QString name);
    void unloadLib();
    Info curLib(){return currentLib;}
    bool curLib(Info lib){return lib==currentLib;}
    //Прочие слоты//
    void metaUpdated(Track*);
    void mouseLeftDoubleClicked(QPointF);
    void mouseRightDoubleClicked(QPointF);
    void mouseLeftClicked(QPointF);
    void mouseRightClicked(QPointF);
Q_SIGNALS:
    void animateTimeChange(int);

protected:
    //Типизация функций библиотек
    typedef void (*Fpaint)(QPainter*, QPaintEvent*);
    typedef void (*Fvoid)();
    typedef void (*Finit)(QSize, AudioThread*);
    typedef void (*Fsize)(QSize);
    typedef void (*Fint)(int);
    typedef Info (*Finfo)();
    typedef void (*Fmouse)(QPointF);
    //Функуции библиотек
    Fpaint OFpaint;
    Fvoid OFanimate;
    Finit OFinit;
    Fsize OFsetWinSize;
    Fint OFsetLeftLevel, OFsetRightLevel;
    Finfo OFgetInfo;

    Fmouse OFmouseLeftDoubleClicked;
    Fmouse OFmouseRightDoubleClicked;
    Fmouse OFmouseLeftClicked;
    Fmouse OFmouseRightClicked;

    Info currentLib;
    QList<Info> libs;
    QLibrary hinstLib;
    QTimer *nulledTime = nullptr;
    Track *meta = nullptr;
    AudioThread *audio;
    QSettings settings;
    QSize winSize;

    //
    QTimer *unload_timer = nullptr;
    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;
    int animateTimer;
    QPixmap no_art = QPixmap(":/icons/no_art.jpg");

private slots:
    void timeoutUnload();

};

#endif // PAINTER_H
