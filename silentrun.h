#ifndef SILENTRUN_H
#define SILENTRUN_H

#include <QtCore>
#include "mainwindow.h"
#include "ui_mainwindow.h"

class SilentRun : public QThread
{
public:
    SilentRun();
protected:
    void run();
};

#endif // SILENTRUN_H
