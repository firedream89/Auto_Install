#include "process.h"
#include "ui_process.h"

Process::Process(QWidget *parent,int nbAppli) :
    QDialog(parent),
    ui(new Ui::Process)
{
    ui->setupUi(this);

    ui->ProgressBar->setMinimum(0);
    ui->ProgressBar->setMaximum(nbAppli);
    ui->ProgressBar->setValue(0);
    ui->ShowText->clear();
    load = false;

    //connect(this,SIGNAL(loading()),this,SLOT(Loading()));
}

Process::~Process()
{
    delete ui;
}

void Process::AddText(QString text, bool newRow)
{
    if(newRow) {
       ui->ShowText->addItem(text);
       load = true;
       emit loading();
    }
    else {
        if(text.count() > 1)
            load = false;
        if(ui->ShowText->item(ui->ShowText->count()-1)->text().split(" ").count() > 1) {
            QString tmp = ui->ShowText->item(ui->ShowText->count()-1)->text();
            tmp.replace(tmp.split(" ").last(),text);
            ui->ShowText->item(ui->ShowText->count()-1)->setText(tmp);
        }
        else {
            ui->ShowText->item(ui->ShowText->count()-1)->setText(text);
        }
    }
}

void Process::Progress(int nb)
{
    if(nb == 0) {
        ui->ProgressBar->setValue(ui->ProgressBar->value()+1);
    }
    else {
        ui->ProgressBar->setValue(nb);
    }
}

void Process::UpdateText(QString arg1, QString arg2)
{
    QString tmp = ui->ShowText->item(ui->ShowText->count()-1)->text();
    tmp.replace("%0",arg1);
    tmp.replace("%1",arg2);
    ui->ShowText->item(ui->ShowText->count()-1)->setText(tmp);
}

void Process::DownProgress(qint64 actual, qint64 max)
{
    QString tmp = ui->ShowText->item(ui->ShowText->count()-1)->text();
    if(tmp.contains("%2")) {
        tmp.replace("%2",QString::number(double(actual / 1000)));
        tmp.replace("%3",QString::number(double(max / 1000)));
    }
    else {
        if(tmp.split(" ").count() > 1) {
            tmp.replace(tmp.split(" ").last(),QString::number(double(actual / 1000000)) + "/" + QString::number(double(max / 1000000)) + "Mo");
        }
    }
    ui->ShowText->item(ui->ShowText->count()-1)->setText(tmp);
}

void Process::ChangeMaxProgress(int max)
{
    ui->ProgressBar->setMaximum(max);
}

void Process::Reset()
{
    ui->ProgressBar->setMinimum(0);
    ui->ProgressBar->setValue(0);
    ui->ShowText->clear();
}

void Process::Loading()
{
    if(ui->ShowText->item(ui->ShowText->count()-1)->text().split(" ").count() < 2)
        ui->ShowText->item(ui->ShowText->count()-1)->setText(ui->ShowText->item(ui->ShowText->count()-1)->text() + " ");
    int act = 0;
    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    connect(&timer,SIGNAL(timeout()),&timer,SLOT(stop()));

    while(load) {
        qApp->processEvents();
        QString t = ui->ShowText->item(ui->ShowText->count()-1)->text().split(" ").at(1);
        act++;
        if(act > 3)
            act = 0;
        if(act == 0)
            t = "-";
        else if(act == 1)
            t = "\\";
        else if(act == 2)
            t = "|";
        else if(act == 3)
            t = "/";

        AddText(t,false);
        timer.start(50);
        loop.exec();
    }
}
