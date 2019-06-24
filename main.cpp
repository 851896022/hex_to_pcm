#include <QApplication>
#include "global.h"
#include "hexreceiver.h"
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include "rebootcontrol.h"
global *g;
void setDebugOutput(const QString &targetFilePath, const bool &argDateFlag = false);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    QString log_dir;
    log_dir=qApp->applicationDirPath()+"/log_hex_to_pcm/%1.log";
    //setDebugOutput( log_dir, true );





    g=new global;
    if(argc==3)
    {
        g->port=QString(argv[1]).toInt();
        g->No=QString(argv[2]).toInt();
        qDebug()<<g->port<<g->No;
    }
    else
    {
        g->port=2000;
        g->No=0;
    }
    g->initPcmToMp3();
    RebootControl *rebootControl=new RebootControl;
    return a.exec();
}
//日志生成
void setDebugOutput(const QString &rawTargetFilePath_, const bool &argDateFlag_)
{
    static QString rawTargetFilePath;
    static bool argDateFlag;

    rawTargetFilePath = rawTargetFilePath_;
    argDateFlag = argDateFlag_;

    class HelperClass
    {
    public:
        static void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &message_)
        {
            QString message;

            switch ( type )
            {
                case QtDebugMsg:
                {
                    message = message_;
                    break;
                }
                case QtWarningMsg:
                {
                    message.append("Warning: ");
                    message.append(message_);
                    break;
                }
                case QtCriticalMsg:
                {
                    message.append("Critical: ");
                    message.append(message_);
                    break;
                }
                case QtFatalMsg:
                {
                    message.append("Fatal: ");
                    message.append(message_);
                    break;
                }
                default: { break; }
            }

            QString currentTargetFilePath;

            if ( argDateFlag )
            {
                currentTargetFilePath = rawTargetFilePath.arg( ( ( argDateFlag ) ? ( QDateTime::currentDateTime().toString("yyyy_MM_dd") ) : ( "" ) ) );
            }
            else
            {
                currentTargetFilePath = rawTargetFilePath;
            }

            if ( !QFileInfo::exists( currentTargetFilePath ) )
            {
                QDir().mkpath( QFileInfo( currentTargetFilePath ).path() );
            }

            QFile file( currentTargetFilePath );
            file.open( QIODevice::WriteOnly | QIODevice::Append );

            QTextStream textStream( &file );
            textStream << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) << ": " << message << "\r\n";
        }
    };

    qInstallMessageHandler( HelperClass::messageHandler );
}
