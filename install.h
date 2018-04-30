#ifndef INSTALL_H
#define INSTALL_H

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <windows.h>
#include <shellapi.h>

class Install : public QObject
{
    Q_OBJECT
public:
    void Stop();
    bool NextInstall(QString link);

signals:
    void Finish(QString appli,int exitCode);

public slots:
    void run();
    bool AddAppli(QString link);

private slots:
    void InstallFinished(int exitcode);
private:
    QStringList appli;
    bool isActive;
};

#endif // INSTALL_H
