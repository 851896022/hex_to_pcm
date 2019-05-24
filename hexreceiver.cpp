#include "hexreceiver.h"

hexReceiver::hexReceiver(QObject *parent) : QObject(parent)
{
    udpSocketReceiver=new QUdpSocket;
    connect(udpSocketReceiver,SIGNAL(readyRead()),this,SLOT(OnudpSocketReceive()));
    udpSocketReceiver->bind(g->port);

    udpSocketSender=new QUdpSocket;
    pcm_to_mp3_IP="127.0.0.1";
    //----------------------
    decoder[0]=new CMP3ToPCM;
    decoder[1]=new CMP3ToPCM;
    decoder[2]=new CMP3ToPCM;
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
    timer->start(20);
}
hexReceiver::~hexReceiver()
{
    delete decoder[0];
    delete decoder[1];
    delete decoder[2];
    delete udpSocketSender;
    delete udpSocketReceiver;
}
void hexReceiver::OnudpSocketReceive()
{

    unsigned char ch[65536]={0};
    int realsize=0;
    while(udpSocketReceiver->pendingDatagramSize()>0)
    {
        realsize=udpSocketReceiver->readDatagram((char*)ch,65536);
        dataCache.append((char*)ch,realsize);
//        while (realsize>51)
//        {
//            if((ch[0]==0xFB&&ch[1]==0)&&realsize==594)
//            {
//                unsigned char input[1152];
//                unsigned char output[23040];
//                int decoderNo=(ch[5]-1)/2;
//                int chNo=((ch[5]-1)*3)+(g->No*18);
//                int k=6;
//                size_t doneLen;

//                for(int i=0;i<6;i++)
//                {
//                    for (int m=0;m<97;m++)
//                    {
//                        input[m] = ch[k];
//                        k++;
//                    }


//                    decoder[decoderNo]->feedANDdecode(input,97,output,23040,&doneLen);//长度必须多一个，要不不工作

//                    //-----要做的事-------
//                    if(((i%2)==1))
//                    {
//                        int num=i/2;
//                        int port=50000+((num+chNo)*10);
//                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port);
//                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+1);
//                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+2);
//                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+3);
//                        ++g->rebootcount;
//                    }
//                    k++;
//                }

//                //=====结束=处理音频数据=========================
//                realsize-=594;
//                for(int i=0;i<realsize;i++)
//                {
//                    ch[i]=ch[i+594];
//                }
//                continue;
//            }
//            else if((ch[0]==0xFC&&ch[1]==0)&&realsize==52)
//            {

//                int apm[18];
//                for(int i=0;i<18;i++)
//                {
//                    apm[i]=0;
//                }


//                for(int i=0;i<18;i++)
//                {
//                    apm[i]=(ch[(2*i)+4]*256)+ch[(2*i)+5];
//                    if(apm[i]<150) apm[i]=1;
//                    if(apm[i]==0)  apm[i]=1;
//                    double ret=((1.42*20.0*log10(apm[i])-30.0)-30.0)*1.50*g->M;

//                    apm[i]=int(ret);
//                    if(apm[i]<0)  apm[i]=0;
//                    if(apm[i]>99) apm[i]=99;

//                }
//                QString data;
//                data+="0|";
//                data+=QString::number(g->No);
//                data+="|";
//                for(int i=0;i<18;i++)
//                {
//                    data+=QString::number(apm[i]);
//                    data+="|";
//                }
//                data+=";";
//                udpSocketSender->writeDatagram(data.toLatin1(),QHostAddress("127.0.0.1"),2500);
//                //=======结束=处理APM====================
//                realsize-=52;
//                for(int i=0;i<realsize;i++)
//                {
//                    ch[i]=ch[i+52];
//                }
//                continue;
//            }
//            else
//            {
//                qDebug()<<QString::number(00)<<"this package is Unkonw";
//                break;
//                realsize-=1;
//                for(int i=0;i<realsize;i++)
//                {
//                    ch[i]=ch[i+1];
//                }
//            }

//        }

    }
//    else
//    {
//        realsize=udpSocketReceiver->readDatagram((char*)ch,65536);
//    }
}
void hexReceiver::onTimerOut()
{
    timer->stop();
    while(dataCache.count()>51)
    {

        if((dataCache[0]==0xFB && dataCache[1]==0))//如果包头符合音频包格式
        {
           qDebug()<<"包头符合FB00";
           if(dataCache.count()>10)
           {
               qDebug()<<"长度大于10";
               if(dataCache[6]==0xFF && dataCache[7]==0xFB)
               {
                   qDebug()<<"符合FFFB";
                   if(dataCache.count()>=594)
                   {
                       qDebug()<<"处理音频594";
                       decode(QByteArray(dataCache.data(),594));
                       dataCache.remove(0,594);
                   }
                   else//长度太短，跳出
                   {
                       qDebug()<<"不足594";
                       break;
                   }
               }
               else//不符合，不认识
               {
                   qDebug()<<"不符合FFFB"<<QString::number( dataCache[7])<<QString::number(dataCache[8]);
                   dataCache.remove(0,1);
               }
           }
           else//长度太短，跳出
           {
               qDebug()<<"不足10";
               break;
           }
        }
        else if((dataCache[0]==0xFC && dataCache[1]==0))//如果包头符合幅度包格式
        {
            if(dataCache.count()>=54)
            {
                if((dataCache[52]==0xFB && dataCache[53]==0))
                {
                    decode(QByteArray(dataCache.data(),52));
                    dataCache.remove(0,52);
                }
                else//不符合，不认识
                {
                    dataCache.remove(0,1);
                }

            }
            else//长度太短，跳出
            {
                dataCache.remove(0,1);
            }

        }
        else//都不符合
        {
            dataCache.remove(0,1);
        }

    }

    timer->start(20);
}
void hexReceiver::decode(QByteArray ch)
{
    int realsize=ch.count();
    if(realsize>51)
    {
        if((ch[0]==0xFB&&ch[1]==0)&&realsize==594)
        {
            unsigned char input[1152];
            unsigned char output[23040];
            int decoderNo=(ch[5]-1)/2;
            int chNo=((ch[5]-1)*3)+(g->No*18);
            int k=6;
            size_t doneLen;

            for(int i=0;i<6;i++)
            {
                for (int m=0;m<97;m++)
                {
                    input[m] = ch[k];
                    k++;
                }


                decoder[decoderNo]->feedANDdecode(input,97,output,23040,&doneLen);//长度必须多一个，要不不工作

                //-----要做的事-------
                if(((i%2)==1))
                {
                    int num=i/2;
                    int port=50000+((num+chNo)*10);
                    udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port);
                    udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+1);
                    udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+2);
                    udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+3);
                    ++g->rebootcount;
                }
                k++;
            }

            //=====结束=处理音频数据=========================
        }
        else if((ch[0]==0xFC&&ch[1]==0)&&realsize==52)
        {

            int apm[18];
            for(int i=0;i<18;i++)
            {
                apm[i]=0;
            }


            for(int i=0;i<18;i++)
            {
                apm[i]=(ch[(2*i)+4]*256)+ch[(2*i)+5];
                if(apm[i]<150) apm[i]=1;
                if(apm[i]==0)  apm[i]=1;
                double ret=((1.42*20.0*log10(apm[i])-30.0)-30.0)*1.50*g->M;

                apm[i]=int(ret);
                if(apm[i]<0)  apm[i]=0;
                if(apm[i]>99) apm[i]=99;

            }
            QString data;
            data+="0|";
            data+=QString::number(g->No);
            data+="|";
            for(int i=0;i<18;i++)
            {
                data+=QString::number(apm[i]);
                data+="|";
            }
            data+=";";
            udpSocketSender->writeDatagram(data.toLatin1(),QHostAddress("127.0.0.1"),2500);
            //=======结束=处理APM====================

        }
        else
        {
            qDebug()<<QString::number(00)<<"this package is Unkonw";

        }

    }

}
