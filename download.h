#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QObject>
#include <QEventLoop>
#include <QThread>
#include <QTimer>
#include <QFile>

enum status
{
    no_Error,
    progress,
    Finish,
    error
};

class Download : public QThread
{
    Q_OBJECT
public:
    void run();
    Download();
    void Start(QStringList link);
    void Stop();
    void RemoveOne(QStringList link);
    bool SaveHtml();

public slots:
    void FindLink(QString name);

signals:
    void DownProgress(qint64 actual, qint64 max);
    void NextProgress(QString name);
    void Finished(QString state);
    void SendText(QString text,bool newRow = false);
    void FindingLink(QString name,QString link);

private slots:
    void StartDownload(QWebEngineDownloadItem *down);
    void StateDownload(QWebEngineDownloadItem::DownloadState state);
    QStringList PrepareDownload(QStringList list);

private:
    QWebEngineView *web;
    QString actual;
    QEventLoop loop;
    int statusDown;
};

#endif // DOWNLOAD_H
