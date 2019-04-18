#ifndef REBOOTCONTROL_H
#define REBOOTCONTROL_H
#include "hexreceiver.h"
#include "global.h"
#include <QObject>
#include <QTimer>
class RebootControl : public QObject
{
    Q_OBJECT
public:
    explicit RebootControl(QObject *parent = nullptr);
    QTimer timer;
    hexReceiver *receiver;
signals:

public slots:
    void onTimerOut();
};

#endif // REBOOTCONTROL_H
