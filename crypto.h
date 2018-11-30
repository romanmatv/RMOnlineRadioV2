#ifndef CRYPTO_H
#define CRYPTO_H
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QCryptographicHash>
class Crypto{
public:
    static QString generateMachine(){
        QSysInfo info;
        QString str = QString(info.machineUniqueId())+QString(info.kernelType())+QString(info.machineHostName())+QString(info.buildCpuArchitecture());
        QByteArray hash = QCryptographicHash::hash(QByteArray::fromStdString(str.toStdString()), QCryptographicHash::Md5);
        return hash.toHex();
    }

    static short int charToInt(char a){
        switch (a) {
            case '0': return 0;
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case '5': return 5;
            case '6': return 6;
            case '7': return 7;
            case '8': return 8;
            case '9': return 9;
        }
        return 0;
    }

    static bool validSerial(QString serial, QString machine, QString username){
        QString secret = "8867c7ebf004ed12616a0affbcc63a70677dcea7790fc8dfd1ab9a385c0bf6f80d755d7abdbf704901145e6bbc5e65bf7d8e897671bf7b4ef66ac97dbd3c6824";
        if (serial.isEmpty()||machine.isEmpty()||username.isEmpty()){
            return false;
        }
        QStringList parts = serial.split("-");
        QString key1,key2,key3,Rserial;

        short int itersA = charToInt(parts[0][0].toLatin1()); //A
        short int itersB = charToInt(parts[1][0].toLatin1()); //B
        short int itersC = charToInt(parts[2][0].toLatin1()); //C
        short int itersD = charToInt(parts[3][0].toLatin1()); //D
        short int i;

        key1 = QCryptographicHash::hash(QByteArray::fromStdString(secret.toStdString()), QCryptographicHash::Sha3_512).toHex();
        for (i=0; i<itersA;i++)
            key1 = QCryptographicHash::hash(QByteArray::fromStdString(key1.toStdString()), QCryptographicHash::Sha3_512).toHex();
        key2 = QCryptographicHash::hash(QByteArray::fromStdString(machine.toStdString()), QCryptographicHash::Sha3_512).toHex();
        for (i=0; i<itersB;i++)
            key2 = QCryptographicHash::hash(QByteArray::fromStdString(key2.toStdString()), QCryptographicHash::Sha3_512).toHex();
        key3 = QCryptographicHash::hash(QByteArray::fromStdString(username.toStdString()), QCryptographicHash::Sha3_512).toHex();
        for (i=0; i<itersC;i++)
            key3 = QCryptographicHash::hash(QByteArray::fromStdString(key3.toStdString()), QCryptographicHash::Sha3_512).toHex();
        Rserial = QCryptographicHash::hash(QByteArray::fromStdString(key1.toStdString()+key2.toStdString()+key3.toStdString()), QCryptographicHash::Sha3_512).toHex();
        for (i=0; i<itersD;i++)
            Rserial = QCryptographicHash::hash(QByteArray::fromStdString(Rserial.toStdString()), QCryptographicHash::Sha3_512).toHex();
        Rserial = QCryptographicHash::hash(QByteArray::fromStdString(Rserial.toStdString()), QCryptographicHash::Md5).toHex().toUpper();
        Rserial.replace(0,1,QString::number(itersA)[0]);
        Rserial.replace(4,1,QString::number(itersB)[0]);
        Rserial.replace(8,1,QString::number(itersC)[0]);
        Rserial.replace(12,1,QString::number(itersD)[0]);
        Rserial.insert(4,'-');
        Rserial.insert(9,'-');
        Rserial.insert(14,'-');
        Rserial.insert(19,'-');
        Rserial.insert(24,'-');
        Rserial.insert(29,'-');
        Rserial.insert(34,'-');
        return  (serial.compare(Rserial)==0);
    }
};

#endif // CRYPTO_H
