#ifndef HEXRECEIVER_H
#define HEXRECEIVER_H

#include <QObject>
#include "global.h"
#include "lib/mp3TOpcm.h"
class hexReceiver : public QObject
{
    Q_OBJECT
public:
    explicit hexReceiver(QObject *parent = nullptr);
    ~hexReceiver();
    QUdpSocket *udpSocketReceiver;//接收socket
    QUdpSocket *udpSocketSender;// 发送 socket
    QString pcm_to_mp3_IP;//一般为127.0.0.1
    CMP3ToPCM *decoder[3];//3个解码器
signals:

public slots:
    void OnudpSocketReceive();
};

#endif // HEXRECEIVER_H
