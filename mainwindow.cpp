#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    trackModel(new TrackModel(this)),
    audioThread(new AudioThread(this)),
    sets_timer(new QTimer())
{
    //Инициализируем основные элементы
    ui->setupUi(this);
#ifndef win32
slash = "\\";
#else
slash = "/";
#endif
    localPath = QCoreApplication::applicationDirPath()+slash;
    hasOpenGL = QGLFormat::hasOpenGL();

    trackModel->load(QUrl::fromLocalFile(localPath+"local.list"));
    lbl_mdeiaStatus = new QLabel(this);
    ui->statusBar->addWidget(lbl_mdeiaStatus);

    // Настройка таблицы плейлиста
    ui->playlistView->setModel(trackModel);    // Устанавливаем модель данных в TableView
    ui->playlistView->setItemDelegate(new TrackDelegate);
    setAcceptDrops(true);

    //Соединяем сигналы
    connect(ui->playlistView, &QListView::activated, this, &MainWindow::jump);
    connect(audioThread, SIGNAL(metaChanged(Track*)), this, SLOT(metaUpdated(Track*)));

    connect(audioThread, SIGNAL(leftLevelChanged(int)), ui->progressBar, SLOT(setValue(int)));
    connect(audioThread, SIGNAL(rightLevelChanged(int)), ui->progressBar_2, SLOT(setValue(int)));

    connect(audioThread, SIGNAL(leftLevelChanged(int)), this, SLOT(leftChan(int)));
    connect(audioThread, SIGNAL(rightLevelChanged(int)), this, SLOT(rightChan(int)));

    connect(audioThread, SIGNAL(startOfPlayback(double)), this, SLOT(onStartPlay()));
    connect(audioThread, SIGNAL(endOfPlayback()), this, SLOT(onStopPlay()));
    connect(audioThread, SIGNAL(pauseOfPlayback()), this, SLOT(onStopPlay()));

    connect(sets_timer, SIGNAL(timeout()), this, SLOT(readAndApplaySets()));

    //Читаем легкие настройки
    ui->volumeSlider->setValue(settings.value("play/volume", 50).toInt());
    setVolume();

    this->setGeometry(settings.value("window/geometry", this->rect()).toRect());
    this->setWindowState(intToWindowState(settings.value("window/state", windowStateToInt(this->windowState())).toInt()));

    if (settings.value("play/savestation", false).toBool()){
        position = settings.value("play/curentstation", 0).toInt();
        if (trackModel->count()==0) position = -1; else if (position>=trackModel->count()) position = 0;
        trackModel->setActive(position);
    }

    //Читаем тяжелые настройки через таймер
    sets_timer->start(100);
}

MainWindow::~MainWindow()
{
    qDebug() << "Начинаем завершение";

    if (sf_win){
        sf_win->stop();
        disconnect(sf_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
        disconnect(sf_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));
        ui->w_painter->removeWidget(sf_win);
        //delete sf_win;
    }
    if (gl_win){
        gl_win->stop();
        disconnect(gl_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
        disconnect(gl_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));
        ui->w_painter->removeWidget(gl_win);
        //delete gl_win;
    }

    audioThread->stop();
    audioThread->free();
    audioThread->terminate();
    qDebug() << "Остановили аудио трэд";

    delete ui;
    qDebug() << "Выпилили: ui";

    if (settings.value("play/savelist", true).toBool()) {
        trackModel->save(QUrl::fromLocalFile(localPath+"local.list"), "m3u");
    }
    qDebug() << "Записали: savelist";
    if (settings.value("play/savestation", false).toBool()){
        settings.setValue("play/curentstation", position);
    }
    qDebug() << "Записали: savestation";
    if (this->windowState()!=Qt::WindowMaximized){
        settings.setValue("window/width", this->width());
        settings.setValue("window/height", this->height());

        settings.setValue("window/geometry", this->geometry());
    }
    qDebug() << "Записали: window";
    settings.setValue("window/state", windowStateToInt(this->windowState()));
    qDebug() << "Записали: state";
    settings.sync();
    qDebug() << "Записали: sync";

    delete trackModel;
    qDebug() << "Выпилили: trackModel";
    delete audioThread;
    qDebug() << "Выпилили: audioThread";
    delete lbl_mdeiaStatus;
    qDebug() << "Выпилили: lbl_mdeiaStatus";

    delete painter;
    qDebug() << "Выпилили: painter";
}

void MainWindow::readAndApplaySets(){

    painter = new Painter(QSize(200,200),audioThread);

    if (settings.value("play/autoplay", false).toBool()){
        if (position>-1)
            audioThread->play(trackModel->getCurrent());
    }

    if (settings.value("visual/opengl", true).toBool() && hasOpenGL){
        turnHardRender();
    }else{
        turnSoftRender();
    }

    if (settings.value("visual/scene", "").toString() != ""){
        if (!painter->setLib(settings.value("visual/scene", "").toString())){
            ui->statusBar->showMessage("Сцена не загружена!", 600);
        }
    }

    updateScences();

    sets_timer->stop();
}

bool MainWindow::turnSoftRender(){

    if (sf_win!=nullptr && gl_win == nullptr) return true;

    if (gl_win!=nullptr){
        disconnect(gl_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
        disconnect(gl_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));
        ui->w_painter->removeWidget(gl_win);
        gl_win = nullptr;
    }

    sf_win = new SFPaint(this, painter);
    ui->w_painter->addWidget(sf_win);
    sf_win->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sf_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
    connect(sf_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));

    settings.setValue("visual/opengl", false);
    return true;
}

bool MainWindow::turnHardRender(){

    if (gl_win!=nullptr && sf_win == nullptr) return true;
    if (!hasOpenGL) return false;
    if (sf_win!=nullptr){
        disconnect(sf_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
        disconnect(sf_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));
        ui->w_painter->removeWidget(sf_win);
        sf_win = nullptr;
    }

    QSurfaceFormat fmt;
    fmt.setSamples(2); //2, 4, 8, 16

    gl_win = new GLPaint(this, painter);
    gl_win->setFormat(fmt);
    gl_win->setMaximumSize(QSize(16777215,16777215));

    ui->w_painter->addWidget(gl_win);
    gl_win->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(gl_win, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(VisContextMenu(const QPoint &)));
    connect(gl_win, SIGNAL(mouseLeftDoubleClicked(QPointF)), this, SLOT(swichFullScreen()));
    settings.setValue("visual/opengl", true);
    return true;
}

void MainWindow::on_btn_PlayPause_clicked(){
    if (position>=0&&position<trackModel->count()){
        audioThread->playOrPause(trackModel->list[position]);
    }else return;
}

void MainWindow::on_btn_Stop_clicked(){
    audioThread->stop();
}

void MainWindow::on_volumeSlider_sliderMoved(int position){
    ui->btn_mute->setIcon(QIcon(":/interfece/Olha/unmute.png"));
    ui->btn_mute->setChecked(false);
    setVolume(position);
    settings.setValue("play/volume", position);
    settings.sync();
}

void MainWindow::on_playlistLoaded(){
    qDebug() << "Плейлист загружен\r\n";
}

static bool isPlaylist(const QUrl &url){
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    if (fileInfo.exists()){
        if (!fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive)) return true;
        if (!fileInfo.suffix().compare(QLatin1String("pls"), Qt::CaseInsensitive)) return true;
        if (!fileInfo.suffix().compare(QLatin1String("xspf"), Qt::CaseInsensitive)) return true;
    }
    return false;
}

void MainWindow::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            trackModel->load(url);
        else
            trackModel->appendTrack(url);
    }
}

void MainWindow::metaUpdated(Track*meta){
    //qDebug() << "Вызов слота Meta";
    if (meta){
        trackModel->updateTrack(meta);
        ui->musicCaptionLabel->setText(meta->radioName + " " + meta->artist + " " + meta->title);
    }
}

void MainWindow::setTrackInfo(const QString &info)
{
    m_trackInfo = info;
    ui->musicCaptionLabel->setText(info);

    if (ui->statusBar) {
        //ui->statusBar->showMessage(m_trackInfo);
        //m_statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle(m_trackInfo);
    }
}

void MainWindow::setStatusInfo(const QString &info)
{
    m_statusInfo = info;

    if (ui->statusBar) {
        ui->statusBar->showMessage(m_trackInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle(m_trackInfo);
    }
}

void MainWindow::playlistPositionChanged(int currentItem)
{
    ui->playlistView->setCurrentIndex(trackModel->index(currentItem, 0));
    settings.setValue("play/curentstation", currentItem);
}

void MainWindow::jump(const QModelIndex &index)
{
    jumpi(index.row());
}

void MainWindow::jumpi(int index)
{
    if (index<0 && trackModel->count()>=0){
        index = 0;
    }
    if (index>=0 && index<trackModel->count()) {
        trackModel->setActiveDeactivate(index, position);
        position = index;
        audioThread->play(trackModel->list.at(position));
        ui->playlistView->update();
    }
}

void MainWindow::on_actionOpenSettings_triggered()
{
    SettingsDialog *setsF = new SettingsDialog();
    setsF->show();
}

void MainWindow::on_btn_mute_clicked()
{
    if (!ui->btn_mute->isChecked()){
        ui->btn_mute->setIcon(QIcon(":/interfece/Olha/unmute.png"));
        setVolume();
    }else{
        ui->btn_mute->setIcon(QIcon(":/interfece/Olha/mute.png"));
        setVolume(0);
    }
}

void MainWindow::on_btn_add_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Добавить ссылку"),
                                         tr("URL:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()){
        trackModel->appendTrack(text);
    }
}

void MainWindow::on_btn_open_clicked()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Открыть"));
    QStringList supportedMimeTypes;
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    fileDialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

void MainWindow::setVolume()
{
    audioThread->setVolume(ui->volumeSlider->value());
}

void MainWindow::setVolume(int value)
{
    audioThread->setVolume(value);
}

void MainWindow::onPaused(bool p)
{
    if (p) {
        ui->btn_PlayPause->setIcon(QIcon(":/interfece/Olha/play.png"));
    } else {
        ui->btn_PlayPause->setIcon(QIcon(":/interfece/Olha/pause.png"));
    }
}

void MainWindow::onStartPlay()
{
    //mFile = av_player->file(); //open from EventFilter's menu
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
        mTitle = QFileInfo(mFile).fileName();
    setWindowTitle(mTitle);

    ui->btn_PlayPause->setIcon(QIcon(":/interfece/Olha/pause.png"));
}

void MainWindow::onStopPlay()
{
    ui->btn_PlayPause->setIcon(QIcon(":/interfece/Olha/play.png"));
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить плэйлист"), "",
        tr("*.m3u;;*.pls;;*.xspf"));

    if (fileName != "") {
        QUrl url = QUrl::fromLocalFile(fileName);
        QString format = TrackModel::format(fileName);
        trackModel->save(url, format.toLatin1());
    }
}

void MainWindow::on_btn_Next_clicked()
{
    jumpi(position+1);
}

void MainWindow::on_btn_Prew_clicked()
{
    jumpi(position-1);
}

void MainWindow::on_playlistView_customContextMenuRequested(const QPoint &pos)
{
    /* Создаем объект контекстного меню */
    QMenu * menu = new QMenu(this);
    /* Создаём действия для контекстного меню */
    QAction * editDevice = new QAction("Редактировать", this);
    QAction * deleteDevice = new QAction("Удалить", this);
    /* Подключаем СЛОТы обработчики для действий контекстного меню */
    connect(editDevice, SIGNAL(triggered()), this, SLOT(changeListValue()));     // Обработчик вызова диалога редактирования
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(deleteListValue())); // Обработчик удаления записи
    /* Устанавливаем действия в меню */
    menu->addAction(editDevice);
    menu->addAction(deleteDevice);
    /* Вызываем контекстное меню */
    menu->popup(ui->playlistView->viewport()->mapToGlobal(pos));
}

void MainWindow::changeListValue(){
    QModelIndex index = ui->playlistView->currentIndex();
    Track *cur = trackModel->list[index.row()];

    QList<inputDialog::Input> inputs = {
        inputDialog::Input{"Название радио",cur->radioName},
        inputDialog::Input{"URL",cur->path.toString()}
    };

    inputDialog* pInputDialog = new inputDialog(this, inputs, "Редактирование");
    if (pInputDialog->exec() == QDialog::Accepted) {
        QStringList list = pInputDialog->getInputs();
        trackModel->list[index.row()]->radioName = list[0];
        trackModel->list[index.row()]->path = QUrl::fromUserInput(list[1]);
    }
    delete pInputDialog;
}

void MainWindow::deleteListValue(){
    QMessageBox::StandardButton reply;
    QModelIndex index = ui->playlistView->currentIndex();

    reply = QMessageBox::question(this, "Удаление", "Удалить "+trackModel->list[index.row()]->radioName+"?",
                            QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        trackModel->removeTrack(index.row());
    }
}

void MainWindow::VisContextMenu(const QPoint &pos)
{
    updateScences();
   QMenu contextMenu(tr("Context menu"), this);

   QAction *action1;

   if (gl_win!=nullptr){
       action1 = new QAction("Переключить на софтовый", this);
       connect(action1, SIGNAL(triggered()), this, SLOT(turnSoftRender()));
       contextMenu.addAction(action1);
       //qDebug() << "Переключалка на софтовый";
   }else{
       action1 = new QAction("Переключить на OpenGL", this);
       connect(action1, SIGNAL(triggered()), this, SLOT(turnHardRender()));
       contextMenu.addAction(action1);
   }

   contextMenu.addSeparator();

   QMenu senceMenu(tr("Выбор сцен"), this);


   senceMenu.addActions(actions);

   contextMenu.addMenu(&senceMenu);

   contextMenu.addSeparator();

   if (gl_win!=nullptr){
       contextMenu.exec(gl_win->mapToGlobal(pos));
   }else{
       contextMenu.exec(sf_win->mapToGlobal(pos));
   }
}

void MainWindow::switchScene(){

    if (painter){
        QList<Painter::Info> libs = painter->getLibs();

        if (sender() == actions[0]){
            painter->unloadLib();
            settings.setValue("visual/scene", "");
        }

        for (short int i=0;i<libs.count();i++){
            if (sender() == actions[i+1]){
                actions[i+1]->setChecked(true);
                painter->setLib(i);
            }else{
                actions[i+1]->setChecked(false);
            }
        }

        if (painter->curLib().isEmpty()){
            actions[0]->setChecked(true);
        }else{
            actions[0]->setChecked(false);
        }
    }
}

void MainWindow::updateScences(){
    actions.clear();

    QList<Painter::Info> libs;
    Painter::Info curLib;

    if (painter){
        painter->scanLibs();
        libs = painter->getLibs();
        curLib = painter->curLib();

        actions.append(new QAction("Альбом арт", this));
        actions[0]->setCheckable(true);
        if (curLib.isEmpty()) actions[0]->setChecked(true);
        connect(actions[0], SIGNAL(triggered()), this, SLOT(switchScene()));

        for (int i=0; i<libs.count(); i++){
            actions.append(new QAction(libs[i].name));
            actions[i+1]->setCheckable(true);
            if (libs[i].filename == "-1"){
                actions[i+1]->setDisabled(true);
            }
            if (curLib == libs[i]){
             actions[i+1]->setChecked(true);
            }
            connect(actions[i+1], SIGNAL(triggered()), this, SLOT(switchScene()));
        }
    }    
}

void MainWindow::swichFullScreen(){
    qDebug() << "fullscreen";
    if (gl_win!=nullptr){
        qDebug() << "HARDWARE";
        //ui->w_painter->removeWidget(gl_win);
        gl_win->setWindowState(Qt::WindowFullScreen);
        gl_win->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }else{
        qDebug() << "SOFTWARE";
        sf_win->setWindowState(Qt::WindowFullScreen);
        sf_win->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        sf_win->showFullScreen();
    }
}

void MainWindow::on_btn_eq_clicked()
{
    EqualizerWindow *eq_win = new EqualizerWindow(nullptr, audioThread);
    eq_win->show();
    //Dialog *dial = new Dialog();
    //dial->show();
}

void MainWindow::leftChan(int v){
    //qDebug() << "left: "<<v;
}

void MainWindow::rightChan(int v){
    //qDebug() << "right: "<<v;
}
