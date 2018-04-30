#include "install.h"

void Install::run()
{
    isActive = true;
    QTimer timer;
    QEventLoop loop;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    while(isActive) {
        qDebug() << "Wait for next install " << appli;
        while(appli.count() > 0) {
            qDebug() << "Install " << appli.at(0);
            if(NextInstall(appli.at(0))) {
                QFile f(appli.at(0));
                f.remove();
            }
            appli.removeFirst();
        }
        timer.start(2000);
        loop.exec();
    }
}

void Install::Stop()
{
    isActive = false;
}

bool Install::NextInstall(QString link)
{
    QProcess process;
    connect(&process,SIGNAL(finished(int)),this,SLOT(InstallFinished(int)));
    process.start(link);
    process.waitForStarted();
    process.waitForFinished();
    qDebug() << "install finished" << process.errorString() << process.error();
    if(process.error() == QProcess::UnknownError) {
        return true;
    }
    else {
        InstallFinished(-1);
        return false;
    }
}

void Install::InstallFinished(int exitcode)
{
    qDebug() << "InstallFinished";
    if(exitcode == 0) {
        emit Finish(appli.at(0).split("/").last(),0);
    }
    else {
        emit Finish(appli.at(0).split("/").last(),-1);
    }
}

bool Install::AddAppli(QString link)
{
    appli.append(link);
    qDebug() << "Add appli";
    return true;
}
