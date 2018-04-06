#ifndef PROCESS_H
#define PROCESS_H

#include <QDialog>
#include <QDebug>
#include <QTimer>

namespace Ui {
class Process;
}

class Process : public QDialog
{
    Q_OBJECT

public:
    explicit Process(QWidget *parent, int nbAppli = 0);
    ~Process();

public slots:
    void AddText(QString text,bool newRow = false);
    void Progress(int nb = 0);
    void UpdateText(QString arg1,QString arg2);
    void DownProgress(qint64 actual,qint64 max);
    void ChangeMaxProgress(int max);
    void Reset();
    void Loading();

signals:
    void loading();

private:
    Ui::Process *ui;
    bool load;
};

#endif // PROCESS_H
