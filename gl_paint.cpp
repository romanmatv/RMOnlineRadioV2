/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gl_paint.h"

//! [0]
GLPaint::GLPaint(QWidget *parent, Painter *pain)
    : QOpenGLWidget(parent), paint_timer(new QTimer(this))
{
    setAutoFillBackground(true);
    connect(paint_timer, &QTimer::timeout, this, &GLPaint::animate);
    paint_timer->start(30);
    setMinimumSize(QSize(200,200));
    setMaximumSize(QSize(16777215,16777215));
    paint.reset(pain);
}

void GLPaint::animate()
{
    if (winSize!=this->size()){
        //qDebug() << this->size().width() << "x" << this->size().height();
        winSize = this->size();
        paint.data()->setSize(winSize);
    }
    paint.data()->animate();
    update();
}
//! [1]

//! [2]
void GLPaint::paintEvent(QPaintEvent *event)
{
    QPainter qpainter;
    qpainter.begin(this);
    qpainter.setRenderHint(QPainter::HighQualityAntialiasing);
    //Рисовальная магия//////
    paint.data()->paint(&qpainter, event);
    //Рисовальная магия//////
    qpainter.end();
}
//! [2]

void GLPaint::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    glHint(GL_SAMPLES, 16);

    glEnable(GL_MULTISAMPLE);
}

void GLPaint::start(int msec){
    paint_timer->start(msec);
}

void GLPaint::stop(){
    paint_timer->stop();
}

void GLPaint::setSize(QSize size){
    setFixedSize(size.width(), size.height());
    paint.data()->setSize(size);
}

void GLPaint::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        Q_EMIT mouseLeftDoubleClicked(e->localPos());
        paint.data()->mouseLeftDoubleClicked(e->localPos());
    }
    if ( e->button() == Qt::RightButton ){
        Q_EMIT mouseRightDoubleClicked(e->localPos());
        paint.data()->mouseRightDoubleClicked(e->localPos());
    }
}

void GLPaint::mouseReleaseEvent(QMouseEvent *e){
    if ( e->button() == Qt::LeftButton )
    {
       Q_EMIT mouseLeftClicked(e->localPos());
        paint.data()->mouseLeftClicked(e->localPos());
    }
    if ( e->button() == Qt::RightButton ){
        Q_EMIT mouseRightClicked(e->localPos());
        paint.data()->mouseRightClicked(e->localPos());
    }
}
