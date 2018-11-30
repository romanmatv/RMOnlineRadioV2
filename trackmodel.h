#ifndef TRACKMODEL_H
#define TRACKMODEL_H
#include <QAbstractListModel>
#include <QList>
#include "track.h"

class TrackModel: public QAbstractListModel
{
    Q_OBJECT
public:

    enum plsformat{
        m3u,
        pls,
        xspf,
        line
    };

    explicit TrackModel(QObject *parent = 0);
    ~TrackModel();
    QList<Track*> list;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void appendTrack(QUrl path, QString name = "");
    void removeTrack(int pos);
    void deleteAllTracks();
    void updateTrack(Track*);
    int actived =-1;
    int count(){
        return list.count();
    }

    void setActive(int in){
        if (in>=0 && in<list.count()){
            list[in]->active = true;
            actived = in;
        }
    }

    void setDeactivete(int in){
        if (in>=0 && in<list.count())
            list[in]->active = false;
    }

    void setActiveDeactivate(int a, int b){
        if (a!=b){
            setActive(a);
            setDeactivete(b);
        }else{
            setActive(a);
        }
    }

    int getActive(){
        return actived;
    }

    Track *getCurrent(){
        if (actived>=0)
            return list[actived];
        else return list[0];
    }

    bool save(const QUrl &location, const char *format = "m3u"){
        QFile fileOut(location.toLocalFile()); // Связываем объект с файлом fileout.txt
        if(fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        { // Если файл успешно открыт для записи в текстовом режиме
            QTextStream writeStream(&fileOut); // Создаем объект класса QTextStream

            writeStream.setCodec(QTextCodec::codecForName("UTF-8"));

            writeStream << list_begin(format);

            for (int i=0; i<list.size(); i++) {
                writeStream << track_by_format(format, list[i], i);
            }

            writeStream << list_end(format);

            fileOut.close(); // Закрываем файл
            return true;
        }else{
            return false;
        }
    }

    void load(const QUrl &location){
        QFile file(location.toLocalFile());
        if (file.open(QIODevice::ReadOnly))
        {
            QString name;
            int tp = pars_frormat(file.readLine());

            switch (tp){
            case 0:{

                    while (!file.atEnd()){
                        QString line = file.readLine();
                        if (line.compare("#EXTINF:") == 1){
                            QStringList line_s = line.split(",");
                            name = line_s[1];
                            line_s = name.split("\r");
                            name = line_s[0];
                            continue;
                        }
                        if (line.isEmpty() || line.isNull() || line.compare("\r\n")==0 || line.compare("\r")==0 || line.compare("\n")==0){
                            continue;
                        }
                        if (name.isEmpty()){
                            appendTrack(QUrl::fromUserInput(line));
                        }else{
                            appendTrack(QUrl::fromUserInput(line), name);
                        }
                    }
                    break;
            }
            case 1:{
                    while (!file.atEnd()){
                        QString line = file.readLine();
                        if (line.compare("Title") == 1){
                            QStringList list = line.split("=");
                            if (!list[list.count()-1].isEmpty())
                                name = list[list.count()-1];
                        }
                        if (line.compare("File") == 1){
                            QStringList list = line.split("=");
                            if (!list[list.count()-1].isEmpty()){
                                appendTrack(QUrl::fromUserInput(list[list.count()-1]), name);
                                name = "";
                            }
                        }
                    }
                    break;
            }
            case 2:{
                    while (!file.atEnd()){
                        QString line = file.readLine();
                        if (line.compare("<title>") == 1){
                            QStringList list = line.split("title>");
                            if (!list[list.count()-1].isEmpty()){
                                list = line.split("</title");
                                if (!list[0].isEmpty()){
                                    name = list[0];
                                }
                            }
                        }
                        if (line.compare("<location>") == 1){
                            QStringList list = line.split("location>");
                            if (!list[list.count()-1].isEmpty()){
                                list = line.split("</location");
                                if (!list[0].isEmpty()){
                                    appendTrack(QUrl::fromUserInput(list[0]), name);
                                    name = "";
                                }
                            }
                        }

                    }
                    break;
            }
            case 3:{
                    while (!file.atEnd()){
                        QString line = file.readLine();
                        if (line.compare("#EXTINF") == 1) continue;
                        if (line.isEmpty() || line.isNull()) continue;
                        appendTrack(QUrl::fromUserInput(line));
                    }
                    break;
            }
            }
            pars_frormat(file.readLine());
        }else return;
    }

    QString track_by_format(QString format, Track *tr, int id){
        QString track;

        QString tit;
        if (!tr->artist.isEmpty() && !tr->title.isEmpty()){
            tit = tr->artist + " - "+tr->title;
        }else{
            tit = tr->radioName;
        }

        if (format == "m3u"){
            track += "#EXTINF:-1,"+tit+"\n";
            track += tr->path.toString() +"\n";
        }else if (format == "pls"){
            track += QString("File") + QString::number(id+1) + QString("=") + tr->path.toString() + QString("\n");
            track += QString("Title") + QString::number(id+1) + QString("=") + tit + QString("\n");
            track += QString("Length") + QString::number(id+1) + QString("=-1\n");
        }else if (format == "xspf"){
            track += QString("<track>\n")+
                    QString("<title>") + tit + QString("</title>\n")+
                    QString("<location>") + tr->path.toString() + QString("</location>\n")+
                    QString("</track>\n");
        }else if (format == "line"){
            track += tr->path.toString()+"\n";
        }else{
            track += "#EXTINF:-1,"+tit+"\n";
            track += tr->path.toString() +"\n";
        }
        return track;
    }

    QString list_begin(QString format){
        QString bg;
        if (format == "m3u"){
            bg += "#EXTM3U\n";
        } else if (format == "pls"){
            bg += "[Playlist]\n";
            bg += "NumberOfEntries=" + QString::number(list.count()+1) + "\n";
        } else if (format == "xspf"){
            bg = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
                            + QString("<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n")
                                + QString("<trackList>\n");
        } else if(format == "line"){
            bg = "";
        } else {
            bg += "#EXTM3U\n";
        }
        return bg;
    }

    QString list_end(QString format){
        QString bg = "";
        if (format == "xspf")
        {
            bg = "</trackList>\n</playlist>\n";
        }
        return bg;
    }

    QString static format(QString filename){
        QStringList list = filename.split(".",QString::SkipEmptyParts);
        return list[list.count()-1];
    }

    plsformat static pars_frormat(QString line){
        if (line.compare("#EXTM3U") == 0 || line.compare("#EXTM3U") == 1){
            return plsformat::m3u;
        }else if (line.compare("[Playlist]") == 0 || line.compare("[Playlist]") == 1){
            return plsformat::pls;
        }else if (line.compare("<?xml")==1){
            return plsformat::xspf;
        }else{
           return plsformat::line;
        }
    }
signals:

public slots:
};

#endif // TRACKMODEL_H
