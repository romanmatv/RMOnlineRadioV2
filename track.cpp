#include "track.h"

Track::Track(QUrl _path, QString _radioName){
    m_manager = new QNetworkAccessManager(this);
    path = _path;
    title = path.fileName();
    radioName = _radioName;
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(onPage_loaded(QNetworkReply*)));
}

Track::~Track()
{
    delete m_manager;
}

QString Track::captionFormated(QString format){
    QString fooString("{title}");
    format.replace(format.indexOf(fooString), fooString.size(), title);
    fooString = "{artist}";
    format.replace(format.indexOf(fooString), fooString.size(), artist);
    fooString = "{radioName}";
    format.replace(format.indexOf(fooString), fooString.size(), radioName);
    fooString = "{radioname}";
    format.replace(format.indexOf(fooString), fooString.size(), radioName);
    fooString = "{genre}";
    format.replace(format.indexOf(fooString), fooString.size(), genre);
    fooString = "{album}";
    format.replace(format.indexOf(fooString), fooString.size(), album);
    fooString = "{date}";
    format.replace(format.indexOf(fooString), fooString.size(), date);

    return format;
}

void Track::clear(){
    title.clear();
    artist.clear();
    album.clear();
    genre.clear();
    date.clear();
}

void Track::search_cover(QString search){
    last_search = search;
    qDebug() << "Пробуем загрузиться с яндекса: "<<search;
    parse("https://yandex.ru/images/search?text="+search+" cover art");
}

void Track::parse(QString url) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    m_manager->get(request);
}
void Track::onPage_loaded(QNetworkReply *reply) {
    QString buff = reply->readAll();

    qDebug()<<"Страница загружена: "<<buff;

    QRegExp iconsRegExp_yandex("\"origin\":{\"w\":\\d+,\"h\":\\d+,\"url\":\"([\\/:\\w\\d\\/\\?.;=\\-&%]+)\"},");
    iconsRegExp_yandex.setMinimal(true);

    QRegExp iconsRegExp_google("{\"clt\":\"n\",\"id\"([\\w:\",.]+)(\"ou\":\")([\\/:\\w\\d\\/\\?.;=\\-&%]+)(\",)\"");
    iconsRegExp_google.setMinimal(true);

    QStringList icons;

    qDebug()<<"RegExp_yandex";
    int lastPos = 0;
    while ((lastPos = iconsRegExp_yandex.indexIn(buff, lastPos)) != -1) {
        QStringList iconData;
        lastPos += iconsRegExp_yandex.matchedLength();

        icons.push_back("https:"+iconsRegExp_yandex.cap(1));
        qDebug() << iconsRegExp_google.cap(1);
    }

    qDebug()<<"RegExp_google";
    lastPos = 0;
    while ((lastPos = iconsRegExp_google.indexIn(buff, lastPos)) != -1) {
        QStringList iconData;
        lastPos += iconsRegExp_google.matchedLength();

        icons.push_back("https:"+iconsRegExp_google.cap(3));
        qDebug() << iconsRegExp_google.cap(3);
    }

    emit finished(icons);
    onFinish(icons);
    reply->deleteLater();
}

void Track::onFinish(QStringList ics){
    qDebug() << "загрузка иконки";
    if (ics.count()>0){
        m_pixmapLoader.load(ics[0]);
    }else{
        qDebug() << "иконок нет, грузим с гугла";
        if (!last_search.isEmpty()){
            parse("https://www.google.ru/search?q="+last_search+" cover art&tbm=isch");
            last_search.clear();
        }
    }
}

void Track::onPixmap_load(QPixmap pixmap) {
    qDebug() << "иконка загружена";
    cover = pixmap;
}

void Track::setLocalCover(QPixmap img){
    local_cover = img;
}

void Track::setLocalCover(QImage img){
    local_cover.convertFromImage(img);
}
