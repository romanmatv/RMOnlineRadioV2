#include "trackmodel.h"

TrackModel::TrackModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

TrackModel::~TrackModel()
{

}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    return list.size();
}

void TrackModel::deleteAllTracks() {
    beginResetModel();
    list.clear();
    endResetModel();
}

void TrackModel::appendTrack(QUrl path, QString name)
{
    Track *track = new Track(path, name);
    beginInsertRows(QModelIndex(),list.size(),list.size());
    list.insert(list.size(), track);
    endInsertRows();
}

void TrackModel::removeTrack(int pos){
    beginRemoveRows(QModelIndex(),pos,pos);
    list.removeAt(pos);
    endRemoveRows();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= list.size())
        return QVariant();
    if (role == Qt::DisplayRole){
        /* Чтобы произвести преобразования QVariant::fromValue(list.at(index.row()))
         * нужно было регистрировать наш тип макросом Q_DECLARE_METATYPE(MyItem *)
         */
        return QVariant::fromValue(list.at(index.row()));
    }else{
        return QVariant();
    }
}

void TrackModel::updateTrack(Track *nwtr){
    if (actived>-1){
        list[actived]->title = nwtr->title;
        list[actived]->artist = nwtr->artist;
        list[actived]->album = nwtr->album;
        if (list[actived]->radioName.isEmpty())
            list[actived]->radioName = nwtr->radioName;
    }
}

Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled ;
}
