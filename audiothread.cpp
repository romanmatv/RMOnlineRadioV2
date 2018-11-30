#include "audiothread.h"
#include "tags.h"

bool endOfMusic;
Track *curTrack;
bool changedMeta;

void __stdcall syncFunc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
    qDebug() << "syncFunc syncMeta: handle="<<handle<<"; channel="<<channel<<"; data="<<data<<"; user="<<user;
    //BASS_ChannelRemoveSync(channel, handle);
    //BASS_ChannelStop(channel);
    //qDebug() << "End of playback!";
    //endOfMusic = true;
}

void __stdcall syncMeta(HSYNC handle, DWORD channel, DWORD data, void *user){
    Q_UNUSED(handle);
    Q_UNUSED(data);
    Q_UNUSED(user);
    curTrack = AudioThread::getMeta(channel);
    changedMeta = true;
}

void __stdcall StatusProc(const void *buffer, DWORD length, void *user)
{
    //if (buffer && !length && (DWORD)user==req) // got HTTP/ICY tags, and this is still the current request
        //MESS(32,WM_SETTEXT,0,buffer); // display status
    //qDebug() << "StatusProc: length="<<length<<"; user="<<user<<"; buffer="<<buffer;
}

AudioThread::AudioThread(QObject *parent, int freq) :
    QThread(parent)
{
    DWORD _freq;
    if (freq>=0 && freq<4){
        _freq = freqs[freq];
    }else{
        _freq = freq;
    }

    if (!BASS_Init(-1, _freq, 0, NULL, NULL)){
        qDebug() << "Cannot initialize device";
    }else{
        BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,1); // enable playlist processing
        BASS_SetConfig(BASS_CONFIG_NET_PREBUF_WAIT,0); // disable BASS_StreamCreateURL pre-buffering

        BASS_PluginLoad("bass_aac.dll",0); // load BASS_AAC (if present) for AAC support on older Windows
        BASS_PluginLoad("basshls.dll",0); // load BASSHLS (if present) for HLS support
        BASS_PluginLoad("bass_ape.dll",0);
        BASS_PluginLoad("bass_ac3.dll",0);
        BASS_PluginLoad("bass_mpc.dll",0);
        BASS_PluginLoad("bassalac.dll",0);
        BASS_PluginLoad("bassflac.dll",0);
        BASS_PluginLoad("bassopus.dll",0);
        BASS_PluginLoad("basswasapi.dll",0);
        BASS_PluginLoad("basswma.dll",0);
        BASS_PluginLoad("basswv.dll",0);
        BASS_PluginLoad("tags.dll",0);
    }
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(signalUpdate()));
    endOfMusic = true;
    changedMeta = false;
}

void AudioThread::setFreq(int freq){
    t->stop();
    BASS_Free();

    DWORD _freq;
    if (freq>=0 && freq<4){
        _freq = freqs[freq];
    }else{
        _freq = freq;
    }

    if (!BASS_Init(-1, _freq, 0, NULL, NULL)){
        qDebug() << "Cannot initialize device";
    }else{
        BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,1); // enable playlist processing
        BASS_SetConfig(BASS_CONFIG_NET_PREBUF_WAIT,0); // disable BASS_StreamCreateURL pre-buffering

        BASS_PluginLoad("bass_aac.dll",0); // load BASS_AAC (if present) for AAC support on older Windows
        BASS_PluginLoad("basshls.dll",0); // load BASSHLS (if present) for HLS support
        BASS_PluginLoad("bass_ape.dll",0);
        BASS_PluginLoad("bass_ac3.dll",0);
        BASS_PluginLoad("bass_mpc.dll",0);
        BASS_PluginLoad("bassalac.dll",0);
        BASS_PluginLoad("bassflac.dll",0);
        BASS_PluginLoad("bassopus.dll",0);
        BASS_PluginLoad("basswasapi.dll",0);
        BASS_PluginLoad("basswma.dll",0);
        BASS_PluginLoad("basswv.dll",0);
        BASS_PluginLoad("tags.dll",0);
    }
}

AudioThread::~AudioThread(){
    t->stop();
    delete t;
    BASS_Free();
}

void AudioThread::play(Track *track){
    //curTrack = &track;
    play(track->path.toString());
}

void AudioThread::play(QString filename)
{
    BASS_ChannelStop(chan);
    //if (!(chan = BASS_StreamCreateURL(filename.toLocal8Bit(),0,BASS_STREAM_BLOCK|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE,&StatusProc,(void*)r)))
    //if (!(chan = BASS_StreamCreateFile(false, (filename.toLocal8Bit()).data(), 0, 0, 0)))

    if (!(chan = BASS_StreamCreateURL(filename.toLocal8Bit(),0,BASS_STREAM_BLOCK|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE,&StatusProc,0))){
        QUrl url = QUrl::fromUserInput(filename);
        if (!(chan = BASS_StreamCreateFile(false, (url.toLocalFile().toLocal8Bit()).data(), 0, 0, 0))){
            return;
        }
    }

    endOfMusic = false;
    BASS_ChannelPlay(chan, true);
    emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
    //playing = true;
    t->start(100);
    BASS_ChannelSetSync(chan,BASS_SYNC_META,0,syncMeta,0); // Shoutcast
    BASS_ChannelSetSync(chan,BASS_SYNC_OGG_CHANGE,0,syncMeta,0); // Icecast/OGG
    BASS_ChannelSetSync(chan,BASS_SYNC_HLS_SEGMENT,0,syncMeta,0); // HLS
    curTrack = AudioThread::getMeta(chan);
    changedMeta = true;
}

void AudioThread::pause()
{
    BASS_ChannelPause(chan);
    t->stop();
    //playing = false;
    emit pauseOfPlayback();
}

void AudioThread::resume()
{
    if (!BASS_ChannelPlay(chan, false))
        qDebug() << "Error resuming";
    else
    {
        t->start(100);
        emit startOfPlayback(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
        //playing = true;
    }
}

void AudioThread::playOrPause(QString filename) {
    if (BASS_ChannelIsActive(chan)==1) {
        //Playing
        pause();
    }
    else {
        if (BASS_ChannelIsActive(chan)==3) {//paused
            resume();
        }
        else {
            play(filename);
        }
    }
}

void AudioThread::stop()
{
    BASS_ChannelStop(chan);
    t->stop();
    //playing = false;
}

void AudioThread::signalUpdate()
{
    if (endOfMusic == false)
    {
        //playing = true;
        emit curPos(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetPosition(chan, BASS_POS_BYTE)),
                    BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
    }
    else
    {
        emit endOfPlayback();
        //playing = false;
    }

    if (changedMeta){
        if (curTrack){
            qDebug() << "Вызов сигнала Meta";
            qDebug() << curTrack->title;
            emit metaChanged(curTrack);
            changedMeta = false;
        }else{
            qDebug() << "Мета якобы обновлена, но нет трека";
        }
    }
}

void AudioThread::changePosition(int position)
{
    BASS_ChannelSetPosition(chan, BASS_ChannelSeconds2Bytes(chan, position), BASS_POS_BYTE);
}

QString AudioThread::getDuration(QString path) {
    BASS_StreamFree(chan);
    chan = BASS_StreamCreateFile(false,path.toLocal8Bit(), 0, 0, 0);
    return formattedTime(BASS_ChannelBytes2Seconds(chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE)));
}

QString AudioThread::formattedTime(double t) {
    int position = (int) (0.5+t);
    int min = (int) position / 60 % 60;
    int sec = (int) position % 60;
    QString s;
    s.sprintf("%02d:%02d",min,sec);
    return s;
}

void AudioThread::run()
{
    while (1);
}

QList<float> AudioThread::getFFT(unsigned long length){
    int len = 0;
    if (length == BASS_DATA_FFT256)
        len = 256;
    if (length == BASS_DATA_FFT512)
        len = 256;
    if (length == BASS_DATA_FFT1024)
        len = 1024;
    if (length == BASS_DATA_FFT2048)
        len = 2048;
    if (length == BASS_DATA_FFT4096)
        len = 4096;
    if (length == BASS_DATA_FFT8192)
        len = 8192;
    if (length == BASS_DATA_FFT16384)
        len = 16384;
    if (len>0){
        float *fft = new float[length];
        BASS_ChannelGetData(chan, fft, length);
        QList<float> fft_list;
        for (int i=0; i<len; i++){
            fft_list.append(fft[i]);
        }
        return fft_list;
    }else{
        return QList<float>();
    }
}

void AudioThread::getFFT(float *fft, unsigned long length){
    BASS_ChannelGetData(chan, fft, length);
}

Track *AudioThread::getMeta(unsigned long chan){
    Track *track = new Track();

    TAGS_SetUTF8(true);

    track->title = QString::fromLocal8Bit(TAGS_Read(chan, "%TITL" ));
    track->artist = QString::fromLocal8Bit(TAGS_Read(chan, "%ARTI" ));
    track->album = QString::fromLocal8Bit(TAGS_Read(chan, "%ALBM" ));
    track->genre = QString::fromLocal8Bit(TAGS_Read(chan, "%GNRE" ));
    track->date = QString::fromLocal8Bit(TAGS_Read(chan, "%YEAR" ));

    const char *icy=BASS_ChannelGetTags(chan,BASS_TAG_ICY);
    if (!icy) icy=BASS_ChannelGetTags(chan,BASS_TAG_HTTP); // no ICY tags, try HTTP
    if (icy) {
        for (;*icy;icy+=strlen(icy)+1) {
            if (!strnicmp(icy,"icy-name:",9)){
                if (track->radioName.isEmpty()){
                    track->radioName = QString(icy+9);
                }
            }
        }
    }

    if (track->title.isEmpty() && track->artist.isEmpty()){
        const char *meta=BASS_ChannelGetTags(chan,BASS_TAG_META);
        if (meta) { // got Shoutcast metadata
            const char *p=strstr(meta,"StreamTitle='"); // locate the title
            if (p) {
                const char *p2=strstr(p,"';"); // locate the end of it
                if (p2) {
                    char *t=strdup(p+13);
                    t[p2-(p+13)]=0;
                    track->title = t;
                    free(t);
                }
            }
        } else {
            meta=BASS_ChannelGetTags(chan,BASS_TAG_OGG);
            if (meta) { // got Icecast/OGG tags
                const char *artist=NULL,*title=NULL,*p=meta;
                for (;*p;p+=strlen(p)+1) {
                    if (!strnicmp(p,"artist=",7)) // found the artist
                        artist=p+7;
                    if (!strnicmp(p,"title=",6)) // found the title
                        title=p+6;
                }
                if (title) {
                    if (artist) {
                        track->title = title;
                        track->artist = artist;
                    } else {
                        track->title = title;
                    }
                }
            } else {
                meta=BASS_ChannelGetTags(chan,BASS_TAG_HLS_EXTINF);
                if (meta) { // got HLS segment info
                    const char *p=strchr(meta,',');
                    if (p)
                        track->title = QString::fromLocal8Bit(p+1);
                }
            }
        }
    }

    return track;
}
