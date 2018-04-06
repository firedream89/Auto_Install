#include "mainwindow.h"
#include "ui_mainwindow.h"

QString version("1.0");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    process = new Process(this);
    Init();
    QDir dir;
    dir.mkdir("Temp");

    connect(ui->actionOptions,SIGNAL(triggered(bool)),this,SLOT(Show_Options()));
    connect(ui->actionA_Propos_de_Qt,SIGNAL(triggered(bool)),qApp,SLOT(aboutQt()));
    connect(ui->actionA_Propos,SIGNAL(triggered(bool)),this,SLOT(About()));
    connect(ui->Start,SIGNAL(clicked(bool)),this,SLOT(Start()));

    connect(&down,SIGNAL(SendText(QString,bool)),process,SLOT(AddText(QString,bool)));
    connect(&down,SIGNAL(DownProgress(qint64,qint64)),process,SLOT(DownProgress(qint64,qint64)));
    connect(&down,SIGNAL(NextProgress(QString)),process,SLOT(Progress()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init()
{
    //Open config file
    QFile file("Config.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,"","Impossible d'ouvrir le fichier 'Config.txt'");
        return;
    }
    QTextStream f(&file);

    //Création dynamique des items suivant le fichier
    /*
     * Type :
     * {Tab}
     * [itemName|itemType|nameList]
    */
    while(!f.atEnd()) {
        QString tmp = f.readLine();
        if(tmp.at(0) == "{") { //Create Tab
            QString tmp_split = tmp.split("{").at(1).split("}").at(0);
            QWidget *widget = new QWidget;
            QFormLayout *l = new QFormLayout(widget);
            widget->setObjectName(tmp_split);
            ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+1,widget,tmp_split);
            ui->tabWidget->setCurrentWidget(widget);
        }
        else if(tmp.at(0) == "[") { //Create checkbox
            QStringList item = tmp.split("[").at(1).split("]").at(0).split("|");
            if(item.at(1).toInt() == checkBox) {
                QCheckBox *cBox = new QCheckBox;
                cBox->setObjectName(item.at(0));
                cBox->setText(item.at(0));
                cBox->setAccessibleName(item.at(3));
                ui->tabWidget->currentWidget()->layout()->addWidget(cBox);
            }
            else if(item.at(1).toInt() == list) { //Create list
                if(!ui->tabWidget->currentWidget()->findChild<QComboBox*>(item.at(2))) { //if exist
                    QComboBox *combo = new QComboBox;
                    combo->setObjectName(item.at(2));
                    combo->addItem("Aucun");
                    combo->addItem(item.at(0));
                    combo->setItemData(combo->count()-1,item.at(3));
                    QFormLayout *l = reinterpret_cast<QFormLayout*>(ui->tabWidget->currentWidget()->layout());
                    l->addRow(item.at(2),combo);
                }
                else { //else
                    ui->tabWidget->currentWidget()->findChild<QComboBox*>(item.at(2))->addItem(item.at(0));
                    ui->tabWidget->currentWidget()->findChild<QComboBox*>(item.at(2))->setItemData(
                                ui->tabWidget->currentWidget()->findChild<QComboBox*>(item.at(2))->count()-1,item.at(3));
                }
            }
        }
    }
}

void MainWindow::Show_Options()
{
    Options *o = new Options(this);
    connect(o,SIGNAL(findLink(QString)),this,SLOT(TransmitFindLink(QString)));
    connect(&down,SIGNAL(FindingLink(QString,QString)),o,SLOT(ReceptLink(QString,QString)));
    o->show();
}

void MainWindow::About()
{
    QFormLayout *layout = new QFormLayout;
    QLabel vVersion(version);
    QLabel auteur("Kévin BRIAND");
    QLabel licence("Ce logiciel est sous licence GNU LGPLv3");
    QLabel github("<a href='https://github.com/firedream89/DealeurGame'>ici</a>");
    github.setOpenExternalLinks(true);
    layout->addRow("Version",&vVersion);
    layout->addRow("Auteur",&auteur);
    layout->addRow("Licence",&licence);
    layout->addRow("Sources",&github);
    QDialog *fen = new QDialog;
    fen->setLayout(layout);
    fen->setWindowTitle("A Propos de Auto Install");
    fen->exec();
}

void MainWindow::Start()
{
    QStringList list;
    for(int i = 0;i<this->findChildren<QCheckBox*>().count();i++) {
        QCheckBox *box = this->findChildren<QCheckBox*>().at(i);
        if(box->isChecked()) {
            list.append(box->text() + "|" + box->accessibleName());
        }
    }
    for(int i = 0;i<this->findChildren<QComboBox*>().count();i++) {
        QComboBox *box = this->findChildren<QComboBox*>().at(i);
        if(box->currentText() != "Aucun") {
            list.append(box->currentText() + "|" + box->itemData(box->currentIndex()).toString());
        }
    }
    qDebug() << list;

    process->Reset();
    process->show();
    process->ChangeMaxProgress(list.count());

    down.Start(list);
}

void MainWindow::TransmitFindLink(QString link)
{
    down.FindLink(link);
}
