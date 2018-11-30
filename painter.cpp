#include "painter.h"

Painter::Painter(QSize Wsize, AudioThread *_tread)  :
    winSize(Wsize)
{
    audio = _tread;
    unload_timer = new QTimer();
    unload_timer->start(30*3);
}

Painter::~Painter(){
    unload_timer->stop();
    delete unload_timer;
    unloadLib();
    hinstLib.unload();
}

void Painter::setSize(QSize size){
    winSize = size;
    if (hinstLib.isLoaded() && OFsetWinSize != nullptr){
        OFsetWinSize(size);
    }
}

void Painter::animate(){
    if (hinstLib.isLoaded() && OFanimate != nullptr){
        OFanimate();
    }
}

void Painter::paint(QPainter *qpainter, QPaintEvent *event){
    if (hinstLib.isLoaded() && OFpaint != nullptr){
        OFpaint(qpainter, event);
    }else{
        if (qpainter->renderHints()!=(QPainter::HighQualityAntialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform)){
            qpainter->setRenderHints(QPainter::HighQualityAntialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
        }
        qpainter->fillRect(event->rect(), background);

        qpainter->translate(0, 0);

        qpainter->setPen(textPen);
        qpainter->setFont(textFont);

        QSize PicSize(winSize.width()+2, winSize.height()+2);
        if (meta){
            if (meta->cover.isNull()){
                qpainter->drawPixmap(0,0, no_art.scaled(PicSize,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            }else{
                qpainter->drawPixmap(0,0, meta->cover.scaled(PicSize,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            }
        }else{
            qpainter->drawPixmap(0,0, no_art.scaled(PicSize,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }
    }
}

bool Painter::setLib(QString name){
    bool islib = false;
#ifndef win32
islib = hinstLib.isLibrary(QCoreApplication::applicationDirPath()+"\\visuals\\"+name);
#else
islib = hinstLib.isLibrary(QCoreApplication::applicationDirPath()+"/visuals/"+name);
#endif
    if (islib){
#ifndef win32
hinstLib.setFileName(QCoreApplication::applicationDirPath()+"\\visuals\\"+name);
#else
hinstLib.setFileName(QCoreApplication::applicationDirPath()+"/visuals/"+name);
#endif
        hinstLib.load();

        OFpaint = (Fpaint)hinstLib.resolve("paint");
        OFanimate = (Fvoid)hinstLib.resolve("animate");
        OFinit = (Finit)hinstLib.resolve("init");
        OFsetWinSize = (Fsize)hinstLib.resolve("setWinSize");
        OFgetInfo = (Finfo)hinstLib.resolve("getInfo");

        OFmouseLeftDoubleClicked = (Fmouse)hinstLib.resolve("mouseLeftDoubleClicked");
        OFmouseRightDoubleClicked = (Fmouse)hinstLib.resolve("mouseRightDoubleClicked");
        OFmouseLeftClicked = (Fmouse)hinstLib.resolve("mouseLeftClicked");
        OFmouseRightClicked = (Fmouse)hinstLib.resolve("mouseRightClicked");

        if (OFgetInfo){
            Info inf = OFgetInfo();
            OFinit(winSize, audio);

            currentLib = inf;
            if (inf.msAni>0){
                if (inf.msAni!=animateTimer)
                    Q_EMIT animateTimeChange(inf.msAni);
                animateTimer = inf.msAni;
            }else{
                if (animateTimer!=30){
                    animateTimer = 30;
                    Q_EMIT animateTimeChange(animateTimer);
                }
                inf.msAni = 30;
            }
            settings.setValue("visual/scene",name);
            return true;
        }else{
            unloadLib();
            return false;
        }

    }else return false;
}

void Painter::unloadLib(){

    currentLib.clear();
    //settings.setValue("visual/scene", "");
    OFpaint = nullptr;
    OFanimate = nullptr;
    OFinit = nullptr;
    OFsetWinSize = nullptr;
    OFsetLeftLevel = nullptr;
    OFsetRightLevel = nullptr;
    OFgetInfo = nullptr;

    //hinstLib.unload();
}

void Painter::timeoutUnload(){
    if (OFpaint == nullptr && OFanimate == nullptr){
        if (hinstLib.isLoaded())
            hinstLib.unload();
    }
}

void Painter::scanLibs(){
    libs.clear();
#ifndef win32
QDir dir(QCoreApplication::applicationDirPath()+"\\visuals");
#else
QDir dir(QCoreApplication::applicationDirPath()+"/visuals");
#endif

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name | QDir::IgnoreCase);

#ifndef win32
dir.setNameFilters(QStringList {"*.dll"});
#else
dir.setNameFilters(QStringList {"*.so"});
#endif

    QFileInfoList list = dir.entryInfoList();
    int count = list.size();
    int ogran = 0;
    QString serial, username, machine;
    serial = settings.value("key/serial","").toString();
    username = settings.value("key/username","").toString();
    machine = settings.value("key/machine","").toString();

    if (!Crypto::validSerial(serial, machine, username)){
        ogran = 3;
    }

    for (int i = 0; i < count; ++i) {
        QFileInfo fileInfo = list.at(i);

#ifndef win32
OFgetInfo = (Finfo)QLibrary::resolve(QCoreApplication::applicationDirPath()+"\\visuals\\"+fileInfo.fileName(), "getInfo");
#else
OFgetInfo = (Finfo)QLibrary::resolve(QCoreApplication::applicationDirPath()+"/visuals/"+fileInfo.fileName(), "getInfo");
#endif

        if (OFgetInfo != nullptr){
            Info inf = OFgetInfo();
            if (ogran>0 && i>ogran-1){
                inf.filename = "-1";
            }else{
                inf.filename = fileInfo.fileName();
            }
            libs << inf;
        }
    }
}

void Painter::metaUpdated(Track* tr){
    meta = tr;
}

void Painter::mouseLeftDoubleClicked(QPointF p){
    if (hinstLib.isLoaded() && OFmouseLeftDoubleClicked != nullptr){
        OFmouseLeftDoubleClicked(p);
    }
}
void Painter::mouseRightDoubleClicked(QPointF p){
    if (hinstLib.isLoaded() && OFmouseRightDoubleClicked != nullptr){
        OFmouseRightDoubleClicked(p);
    }
}
void Painter::mouseLeftClicked(QPointF p){
    if (hinstLib.isLoaded() && OFmouseLeftClicked != nullptr){
        OFmouseLeftClicked(p);
    }
}
void Painter::mouseRightClicked(QPointF p){
    if (hinstLib.isLoaded() && OFmouseRightClicked != nullptr){
        OFmouseRightClicked(p);
    }
}
