#include "hexreceiver.h"

hexReceiver::hexReceiver(QObject *parent) : QObject(parent)
{
    udpSocketReceiver=new QUdpSocket;
    connect(udpSocketReceiver,SIGNAL(readyRead()),this,SLOT(OnudpSocketReceive()));
    udpSocketReceiver->bind(g->port);

    udpSocketSender=new QUdpSocket;
    {
        QString data;
        data+="0|";
        data+=QString::number(g->No);
        data+="|";
        for(int i=0;i<18;i++)
        {
            data+=QString::number(0);
            data+="|";
        }
        data+=";";
        udpSocketSender->writeDatagram(data.toLatin1(),QHostAddress("127.0.0.1"),2500);
    }

    //udpSocketSender->bind(QHostAddress(g->serverIP),(49990+g->No));
    pcm_to_mp3_IP="127.0.0.1";
    //----------------------
    decoder[0]=new CMP3ToPCM;
    decoder[1]=new CMP3ToPCM;
    decoder[2]=new CMP3ToPCM;
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
    if(udpSocketReceiver->pendingDatagramSize()>0)
    {
        realsize=udpSocketReceiver->readDatagram((char*)ch,65536);
        while (realsize>51)
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
#ifdef debbb
                    if((ch[5]==1&&i==1)&&false)
                    {
                        qDebug()<<k<<decoderNo<<"inputlen:"<<sizeof(input)<<"doneLen"<<doneLen;
                        QByteArray aslkdj;
                        for(int i=0;i<97;i++)
                        {
                            aslkdj.append((input[i]));
                        }
                        //qDebug()<<aslkdj.toHex();
                        aslkdj.clear();
                        for(int i=0;i<2304;i++)
                        {
                            aslkdj.append((output[i]));
                        }
                        //qDebug()<<aslkdj.toHex();
                        udpSocketSender->writeDatagram((char*)output,2304,QHostAddress("127.0.0.1"),2501);
                        qDebug()<<g->writeShareMem(&g->sharedMem[(0)],(char*)output,(int)2304);
                    }
#endif
                    //-----要做的事-------
                    if(((i%2)==1))
                    {
                        int num=i/2;
                        int port=50000+((num+chNo)*10);
                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port);
                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+1);
                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress(g->alarmIP),port+2);
                        udpSocketSender->writeDatagram((char*)output,(int)doneLen,QHostAddress("127.0.0.1"),port+3);
                        ++g->rebootcount;
                    }
                    k++;
                }

                //=====结束=处理音频数据=========================
                realsize-=594;
                for(int i=0;i<realsize;i++)
                {
                    ch[i]=ch[i+594];
                }
                continue;
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
                realsize-=52;
                for(int i=0;i<realsize;i++)
                {
                    ch[i]=ch[i+52];
                }
                continue;
            }
            else
            {
                qDebug()<<QString::number(00)<<"this package is Unkonw";
                break;
                realsize-=1;
                for(int i=0;i<realsize;i++)
                {
                    ch[i]=ch[i+1];
                }
            }

        }
    }
    else
    {
        realsize=udpSocketReceiver->readDatagram((char*)ch,65536);
    }
}
