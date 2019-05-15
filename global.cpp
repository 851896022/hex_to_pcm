#include "global.h"

global::global(QObject *parent) : QObject(parent)
{
    //隐藏功能，修改彩条高度
    M=1.0;
    {
        QString filename;
        filename+=(qApp->applicationDirPath()+"/sav/multiple.sav");
        //判断文件是否存在
        QFile *file = new QFile(filename);
        if(file->open(QIODevice::ReadOnly))
        {

                {
                    QString ba(file->readLine());
                    ba=QString::fromStdString( ba.toStdString());
                    float tmp=ba.toFloat();
                    if(tmp>0 && tmp <10)
                    {
                        M=tmp;
                    }

                }



            file->close();
        }
        file->deleteLater();
    }


}
global::~global()
{
    for(int i=0;i<18;i++)
    {
        //detachShareMem(&sharedMem[i]);
        pcmToMp3[i]->kill();
    }


}
/************************************************************
功能：初始化共享内存
参数：QSharedMemory指针
        key字符串
        长度
返回：>=0，成功；
     <0，失败
作者：付润豪 2018年9月28日
************************************************************/
int global::initShareMem(QSharedMemory *m_sharedMem,QString key,int len)
{
    int ret = -1;
    m_sharedMem->setKey(key);
    if (m_sharedMem->isAttached())
    {
        if(!m_sharedMem->detach())
        {
            qWarning("WriteSharedMemory : detach failed.");
            return ret;
        }
    }

    //尝试连接共享内存
    bool isAttached = m_sharedMem->attach(QSharedMemory::ReadWrite);
    if(isAttached)
    {
        qWarning("shared memory is exist");
        return 0;
    }

    //创建共享内存
    ret--;
    if (!m_sharedMem->create(len,QSharedMemory::ReadWrite) )
    {
        qDebug()<<m_sharedMem->errorString();
        qWarning("WriteSharedMemory::Unable to create shared memory segment.");
        return ret;
    }

    return 0;
}

/************************************************************
功能：释放共享内存
参数：QSharedMemory指针
返回：>=0，成功；
     <0，失败
作者：付润豪 2018年9月28日
************************************************************/
int global::detachShareMem(QSharedMemory *m_sharedMem)
{
    int ret = -1;
    if (m_sharedMem->isAttached())
    {
        qWarning("ShareMem is attached");
        if(!m_sharedMem->detach())
        {
            qWarning("WriteSharedMemory:: detach failed.");
            return ret;
        }
        else
        {
            qWarning("WriteSharedMemory:: detached success.");
        }
    }

    return 0;
}
/************************************************************
功能：修改共享内存
参数：QSharedMemory指针
    数据char数组
    长度
返回：>=0，成功；
     <0，失败
作者：付润豪 2018年9月28日
************************************************************/
int global::writeShareMem(QSharedMemory *m_sharedMem,const char *text,int len)
{

    int ret = -1;
    if(NULL == text)    return ret;
    // Write into the shared memory
    //m_sharedMem->lock();
    char *buf = (char*)m_sharedMem->data();
    memcpy( buf, text, 2304);
    //char *ch = (char*)m_sharedMem->constData();
    //m_sharedMem->unlock();
    len=len;
    return 0;
}
/************************************************************
功能：创建编码进程
参数：
返回：>=0，成功；
     <0，失败
作者：付润豪 2018年10月9日
************************************************************/
#include <QFile>
int global::initPcmToMp3()
{
    //======读取名称==================
    QStringList nameList;
    //================================================================
        {
            QString filename;
            filename+=(qApp->applicationDirPath()+"/sav/equ.sav");
            //判断文件是否存在
            QFile *file = new QFile(filename);
            if(file->open(QIODevice::ReadOnly))
            {
                for(int i=0;i<10;i++)
                {
                    {
                        QString ba(file->readLine());
                        ba=QString::fromStdString( ba.toStdString());
                        QStringList list=ba.split("|");
                        qDebug()<<ba;
                        for(int j=0;j<18;j++)
                        {
                            nameList.append(list.at(j+1));
                        }
                    }


                    if(file->atEnd())break;
                }
                file->close();
            }
            file->deleteLater();
        }
    int c=200-nameList.count();
    for(int i=0;i<(c);i++)
    {
        nameList.append(QString(""));
    }
    //======新建进程==================
    for(int i=0;i<18;i++)
    {
        QString workdir;
        workdir=qApp->applicationDirPath();
        QString program;
        program="pcm_to_mp3.exe";
        //端口
        //名称
        //存储位置
        QStringList arguments;

        QString tmp;//端口
        tmp=QString::number((((this->No*18)+i)*10)+50000+1);

        arguments.append(tmp);
        QString tmp1;//名称
        tmp1="ch"+QString("%1").arg(((this->No*18)+i+1),3,10,QLatin1Char('0'));
        tmp1+=nameList.at(((this->No*18)+i));
        tmp1=QByteArray(tmp1.toStdString().data()).toBase64();
        arguments.append(tmp1);
        //qDebug()<<tmp1<<QString(QByteArray::fromBase64(tmp1.toLatin1()));
        QString tmp2;//存储位置

        QString filename;
        filename+=(qApp->applicationDirPath()+"/sav/savedir.sav");
        QFile *file = new QFile(filename);
        if(file->open(QIODevice::ReadOnly))
        {

            QString ba(file->readLine());
            tmp2=ba;

            file->close();
        }
        file->deleteLater();

        if(tmp2==QString(""))
        {
            tmp2="D:/mp3";
        }

        arguments.append(tmp2);
        qDebug()<<No<<arguments;
        pcmToMp3[i]=new QProcess(this);
        pcmToMp3[i]->setWorkingDirectory(workdir);
        pcmToMp3[i]->setProgram(program);
        pcmToMp3[i]->setArguments(arguments);
        pcmToMp3[i]->start();
        pcmToMp3[i]->waitForBytesWritten();
        qDebug()<<pcmToMp3[i]->state();
    }
    return 0;
}
