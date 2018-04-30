#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QWebEngineSettings>
#include <QObject>
#include <QEventLoop>
#include <QThread>
#include <QTimer>
#include <QFile>

enum status
{
    no_Error,
    progress,
    finished,
    error,
    prepare
};

class Download : public QObject
{
    Q_OBJECT
public:
    Download();
    void StartDown(QStringList link);
    void Stop();
    void RemoveOne(QStringList link);
    bool SaveHtml();
    void SetList(QStringList list);

public slots:
    void FindLink(QString name);

signals:
    void DownProgress(qint64 actual, qint64 max);
    void NextProgress(QString name);
    void Finished(QString state);
    void SendText(QString text,bool newRow = false);
    void FindingLink(QString name,QString link);
    void SendInstall(QString link);
    void LoadProgress(int load);

private slots:
    void StartDownload(QWebEngineDownloadItem *down);
    void StateDownload(QWebEngineDownloadItem::DownloadState state);
    QStringList PrepareDownload(QStringList list);

private:
    QWebEngineView *web;
    QString actual;
    QEventLoop loop;
    int statusDown;
    QStringList list;
    QTimer timer;
};

#endif // DOWNLOAD_H
