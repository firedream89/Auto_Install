#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    LoadFile();
    ChangeItemTab();
    LoadCombo();
    ui->checkbox->setChecked(true);
    ShowNameCombo(false);

    connect(ui->cNew,SIGNAL(stateChanged(int)),this,SLOT(ChangeItemTab()));
    connect(ui->add,SIGNAL(clicked(bool)),this,SLOT(AddItem()));
    connect(ui->combobox,SIGNAL(toggled(bool)),this,SLOT(ShowNameCombo(bool)));
    connect(ui->findLink,SIGNAL(clicked(bool)),this,SLOT(FindLink()));
    connect(ui->bUpdate,SIGNAL(clicked(bool)),this,SLOT(UpdateFile()));

    this->show();
}

Options::~Options()
{
    delete ui;
}

void Options::LoadFile()
{
    AddCmd("Load File...");
    ui->ShowFile->clear();

    QFile f("Config.txt");
    if(!f.open(QIODevice::ReadOnly)) {
        AddCmd("Load File...Echec",true);
    }
    else {
        QTextStream flux(&f);
        ui->ShowFile->insertPlainText(flux.readAll());
        AddCmd("Load File...Ok",true);
    }
}

void Options::AddCmd(QString text,bool lastRow)
{
    if(lastRow) {
        ui->Cmd->item(ui->Cmd->count()-1)->setText(text);
    }
    else {
        ui->Cmd->addItem(text);
    }
    ui->Cmd->scrollToBottom();
}

void Options::AddItem()
{
    if(ui->link->text().isEmpty() || ui->name->text().isEmpty()) {
        AddCmd("Tout les champs doivent être rempli !");
        return;
    }
    QFile f("Config.txt");
    if(!f.open(QIODevice::ReadWrite)) {
        AddCmd("Echec open file");
    }
    else {
        int item = 0;
        if(ui->combobox->isChecked()) {
            item = 1;
        }
        else {
            item = 0;
        }

        QTextStream flux(&f);

        QString final;
        bool find(false);
        while(!flux.atEnd()) {
            QString tmp = flux.readLine();
            if(!tmp.isEmpty()) {
                final += "\n" + tmp;
            }
            if((tmp.contains("{" + ui->comboTab->currentText() + "}") && !ui->cNew->isChecked()) || (tmp.contains("{" + ui->newTab->text() + "}") && !ui->newTab->text().isEmpty())) {
                final += "\n[" + ui->name->text() + "|" + QString::number(item) + "|" + ui->nameCombo->text() + "|" + ui->link->text() + "]";
                find = true;
            }
        }
        if(!find) {
            final += "\n{" + ui->newTab->text() + "}";
            final += "\n[" + ui->name->text() + "|" + QString::number((item)) + "|" + ui->nameCombo->text() + "|" + ui->link->text() + "]";
        }
        f.resize(0);
        flux << final;

    }
    AddCmd("Ajout de " + ui->name->text() + " réussis !");
    ui->cNew->setChecked(false);
    ui->newTab->clear();
    ChangeItemTab();
    LoadFile();
    LoadCombo();
}

void Options::FindLink()
{
    AddCmd("Recherche d'un lien de téléchargement...");
    ui->findLink->setEnabled(false);
    emit findLink(ui->name->text());
}

void Options::ChangeItemTab()
{
    if(ui->cNew->isChecked()) {
        ui->comboTab->setVisible(false);
        ui->newTab->setVisible(true);
    }
    else {
        ui->comboTab->setVisible(true);
        ui->newTab->setVisible(false);
    }
}

void Options::LoadCombo()
{
    ui->comboTab->clear();

    QFile f("Config.txt");
    if(!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Echec open file";
    }
    else {
        QTextStream flux(&f);
        while(!flux.atEnd()) {
            QString tmp = flux.readLine();
            if(tmp.contains("{")) {
                ui->comboTab->addItem(tmp.split("{").at(1).split("}").at(0));
            }
        }
    }
}

void Options::ShowNameCombo(bool visible)
{
    if(visible) {
        ui->nameCombo->setVisible(true);
        ui->labelNameCombo->setVisible(true);
    }
    else {
        ui->nameCombo->setVisible(false);
        ui->labelNameCombo->setVisible(false);
    }
}

void Options::UpdateFile()
{
    QString tmp = ui->ShowFile->toPlainText();

    QFile f("Config.txt");
    f.resize(0);
    if(!f.open(QIODevice::WriteOnly)) {
        AddCmd("Echec ouverture fichier");
        return;
    }
    QTextStream flux(&f);
    flux << ui->ShowFile->toPlainText();
    AddCmd("Fichier mis à jour avec succès");

    LoadFile();
    LoadCombo();
}

void Options::ReceptLink(QString name, QString link)
{
    AddCmd(ui->Cmd->item(ui->Cmd->count()-1)->text() + "Finished",true);
    if(link.isEmpty() || link == "http://www.clubic.com") {
        AddCmd("Aucun lien trouvé");
    }
    ui->link->setText(link);
    ui->findLink->setEnabled(true);
}

void Options::Loading()
{
    QString tmp = ui->Cmd->item(ui->Cmd->count()-1)->text().split(" ").at(0);
    if(ui->Cmd->item(ui->Cmd->count()-1)->text().split(" ").count() < 2)
        ui->Cmd->item(ui->Cmd->count()-1)->setText(ui->Cmd->item(ui->Cmd->count()-1)->text() + " ");
    int act = 0;
    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    connect(&timer,SIGNAL(timeout()),&timer,SLOT(stop()));

    while(ui->Cmd->item(ui->Cmd->count()-1)->text().split(" ").at(0) == tmp) {
        qApp->processEvents();
        QString t = ui->Cmd->item(ui->Cmd->count()-1)->text().split(" ").at(1);
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

        ui->Cmd->item(ui->Cmd->count()-1)->setText(tmp + " " + t);
        timer.start(50);
        loop.exec();
    }
    ui->Cmd->item(ui->Cmd->count()-2)->setText(tmp + " Finished");
}

void Options::DownProgress(qint64 actual, qint64 max)
{
    AddCmd(QString::number(actual) + "/" + QString::number(max),true);
}
