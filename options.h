#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <download.h>

enum object {
    checkBox,
    list
};

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();
    void LoadFile();
    void LoadCombo();
    void AddCmd(QString text, bool lastRow = false);
    void Loading();

public slots:
    void ReceptLink(QString name, QString link);

private slots:
    void FindLink();
    void AddItem();
    void ChangeItemTab();
    void ShowNameCombo(bool visible);
    void UpdateFile();
    void DownProgress(qint64 actual,qint64 max);

signals:
    void findLink(QString name);

private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
