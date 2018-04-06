#include "download.h"

void Download::run()
{

}

Download::Download()
{
    actual = "";
    web = new QWebEngineView;
    connect(web->page()->profile(),SIGNAL(downloadRequested(QWebEngineDownloadItem*)),this,SLOT(StartDownload(QWebEngineDownloadItem*)));
    connect(this,SIGNAL(Finished(QString)),&loop,SLOT(quit()));
    statusDown = no_Error;
}

void Download::Start(QStringList link)
{
    //Add script to change page(info > download)
    link = PrepareDownload(link);

    while(link.count() > 0) {
        actual = link.at(0).split("|").at(0);
        emit SendText("Téléchargement de " + actual + " Préparation",true);
        statusDown = no_Error;
        QString dLink = link.at(0).split("|").at(1);
        web->load(QUrl(dLink));
        loop.exec();
        if(!statusDown == QWebEngineDownloadItem::DownloadCompleted) {
            emit SendText("Error downloading file");
        }
        else {
            emit SendText("Finished");
        }
        emit NextProgress(actual);
        link.removeFirst();
    }
}

void Download::StartDownload(QWebEngineDownloadItem *down)
{
    connect(down,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(DownProgress(qint64,qint64)));
    connect(down,SIGNAL(stateChanged(QWebEngineDownloadItem::DownloadState)),this,SLOT(StateDownload(QWebEngineDownloadItem::DownloadState)));
    qDebug() << down->state();
    down->setPath("Temp/" + actual + ".exe");
    down->accept();
}

void Download::StateDownload(QWebEngineDownloadItem::DownloadState state)
{
    switch (state) {
    case QWebEngineDownloadItem::DownloadInProgress:
        qDebug() << "Down en cours";
        statusDown = progress;
        break;
    case QWebEngineDownloadItem::DownloadCancelled:
        statusDown = error;
        emit Finished(actual);
        break;
    case QWebEngineDownloadItem::DownloadCompleted:
        qDebug() << "Down terminé";
        statusDown = Finish;
        emit Finished(actual);
        break;
    case QWebEngineDownloadItem::DownloadInterrupted:
        qDebug() << "Down interrompu";
        statusDown = error;
        emit Finished(actual);
        break;
    default:
        qDebug() << "Down inconnu";
        break;
    }
}

void Download::FindLink(QString name)
{
    qDebug() << "findLink";
    connect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
    name.replace(" ","-");
    web->load(QUrl("http://www.clubic.com/r/" + name + "/telecharger/"));
    loop.exec();
    SaveHtml();

    QFile f("web_Temp.txt");
    if(!f.open(QIODevice::ReadOnly)) {
        qDebug() << "fail to open web_Temp.txt";
    }
    QTextStream flux(&f);
    QString tmp;
    while(!flux.atEnd()) {
        tmp = flux.readLine();
        qDebug() << tmp;
        if(tmp.contains("href=\"/telecharger-fiche") && tmp.contains(name.toLower())) {
            tmp = tmp.split("\"").at(1);
            break;
        }
        else {
            tmp.clear();
        }
    }

    disconnect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
    emit FindingLink(name,"http://www.clubic.com" + tmp);
}

bool Download::SaveHtml()
{
    bool end(false);
    QFile fichier("web_Temp.txt");
    fichier.resize(0);
    if(fichier.open(QIODevice::WriteOnly) == false) { return false; }

    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    connect(&timer,SIGNAL(timeout()),&timer,SLOT(stop()));
    web->page()->toHtml([&fichier,&end](const QString &result){ fichier.write(result.toUtf8()); end = true; });
    while(end == false)
    {
        timer.start(500);
        loop.exec();
    }
    fichier.close();
    return true;
}

QStringList Download::PrepareDownload(QStringList list)
{
    QStringList finalList;
    connect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
    for(int i = 0;i<list.count();i++) {
        QString name = list.at(i).split("|").at(0);
        QString link = list.at(i).split("|").at(1);
        emit SendText("Préparation du téléchargement de " + name,true);
        web->load(QUrl(link));
        loop.exec();
        SaveHtml();

        QFile f("web_Temp.txt");
        if(!f.open(QIODevice::ReadOnly)) {
            qDebug() << "Fail to open file web_Temp.txt";
        }
        QTextStream flux(&f);

        while(!flux.atEnd()) {
            QString line = flux.readLine();
            if(line.contains("downloadUrl")) {
                finalList.append(name + "|http://www.clubic.com" + line.split("\"").at(5));
                break;
            }
        }
        f.close();
    }
    disconnect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
qDebug() << list << finalList;
    return finalList;
}
