#include "download.h"

//utilisation de clubic pour les téléchargements

void Download::SetList(QStringList list1)
{
    list = list1;
}

Download::Download()
{
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    connect(&timer,SIGNAL(timeout()),&timer,SLOT(stop()));
    actual = "";
    web = new QWebEngineView;
    web->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages,false);
    web->page()->settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled,false);
    connect(web->page()->profile(),SIGNAL(downloadRequested(QWebEngineDownloadItem*)),this,SLOT(StartDownload(QWebEngineDownloadItem*)));
    connect(this,SIGNAL(Finished(QString)),&loop,SLOT(quit()));
    statusDown = no_Error;
}

void Download::StartDown(QStringList link)
{
    //Add script to change page(info > download)
    link = PrepareDownload(link);

    while(link.count() > 0) {
        connect(web,SIGNAL(loadProgress(int)),this,SIGNAL(LoadProgress(int)));
        actual = link.at(0).split("|").at(0);
        emit SendText("Téléchargement de " + actual + " Préparation",true);
        web->blockSignals(false);
        statusDown = no_Error;
        QString dLink = link.at(0).split("|").at(1);
        web->load(QUrl(dLink));
        timer.start(30000);
        loop.exec();
        if(statusDown != QWebEngineDownloadItem::DownloadCompleted) {
            if(link.at(0).contains("clubic.com")) {
                emit SendText("Error downloading file, try to find direct link");

                SaveHtml();

                QFile f("web_Temp.txt");
                if(!f.open(QIODevice::ReadOnly)) {
                    qDebug() << "Fail to open file web_Temp.txt";
                }
                QTextStream flux(&f);

                while(!flux.atEnd()) {
                    QString line = flux.readLine();
                    if(line.contains("class=\"new-dl-btn\"")) {
                        if(line.split("\"").count() >= 4) {
                            link.append(link.at(0).split("|").at(0) + "|" + line.split("\"").at(3));
                            link.removeFirst();
                            qDebug() << link;
                        }
                        break;
                    }
                }
                f.close();
            }
            else {
                emit SendText("Error downloading file");
                emit NextProgress(actual);
            }
        }
        else {
            emit SendText("Finished");
            emit SendInstall("Temp/" + actual);
            emit NextProgress(actual);
            link.removeFirst();
        }        
    }
}

void Download::StartDownload(QWebEngineDownloadItem *down)
{
    timer.stop();
    connect(down,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(DownProgress(qint64,qint64)));
    connect(down,SIGNAL(stateChanged(QWebEngineDownloadItem::DownloadState)),this,SLOT(StateDownload(QWebEngineDownloadItem::DownloadState)));
    qDebug() << down->state();

    if(down->path().split(".").last().count() <= 3) {
        actual += "." + down->path().split(".").last();
    }
    else {
        actual += ".exe";
    }
    down->setPath("Temp/" + actual);
    down->accept();
    disconnect(web,SIGNAL(loadProgress(int)),this,SIGNAL(LoadProgress(int)));
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
        statusDown = finished;
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
    connect(web,SIGNAL(loadProgress(int)),this,SIGNAL(LoadProgress(int)));
    QStringList finalList;
    connect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
    for(int i = 0;i<list.count();i++) {
        QString name = list.at(i).split("|").at(0);
        QString link = list.at(i).split("|").at(1);
        emit SendText("Préparation du téléchargement de " + name + " load",true);
        qDebug() << link;
        web->load(QUrl(link));
        timer.start(30000);
        loop.exec();
        qDebug() << "Save Html";
        if(!timer.isActive()) {
            qDebug() << "error load page (timeout)";
        }
        SaveHtml();

        QFile f("web_Temp.txt");
        if(!f.open(QIODevice::ReadOnly)) {
            qDebug() << "Fail to open file web_Temp.txt";
        }
        QTextStream flux(&f);

        while(!flux.atEnd()) {
            QString line = flux.readLine();
            if(line.contains("downloadUrl")) {
                if(line.split("\"").count() >= 6) {
                    finalList.append(name + "|http://www.clubic.com" + line.split("\"").at(5));
                    emit SendText("Finished");
                }
                else {
                    emit SendText("Fail");
                }
                break;
            }
        }
        f.close();
    }
    disconnect(web,SIGNAL(loadFinished(bool)),&loop,SLOT(quit()));
qDebug() << list << finalList;
    disconnect(web,SIGNAL(loadProgress(int)),this,SIGNAL(LoadProgress(int)));
    return finalList;
}
