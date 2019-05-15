#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QUdpSocket>
#include <QFile>
#include <QDir>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QProcess>
#include <QApplication>
class global : public QObject
{
    Q_OBJECT
public:
    explicit global(QObject *parent = nullptr);
    ~global();
    int port;
    int No;
    QSharedMemory sharedMem[18];
    QString sharedMemKey[18];
    QProcess *pcmToMp3[200];
    int rebootcount=0;
    float M=1.0;
signals:

public slots:
    int initShareMem(QSharedMemory*,QString,int);
    int detachShareMem(QSharedMemory*);
    int writeShareMem(QSharedMemory*,const char *,int);
    int initPcmToMp3();
};
extern global *g;
#endif // GLOBAL_H
