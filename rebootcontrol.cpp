#include "rebootcontrol.h"

RebootControl::RebootControl(QObject *parent) : QObject(parent)
{
    receiver=new hexReceiver;
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
    timer.start(3000);
}
void RebootControl::onTimerOut()
{
    if(g->rebootcount<500)
    {
        receiver->deleteLater();
        receiver=new hexReceiver;
        qDebug()<<g->No<<"reboot hexReceiver";
    }
    g->rebootcount=0;
}
