#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QSpacerItem>
#include <options.h>
#include <QLabel>
#include <process.h>
#include <QThread>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Init();

private slots:
    void Show_Options();
    void About();
    void Start();
    void TransmitFindLink(QString link);


private:
    Ui::MainWindow *ui;
    Download down;
    Process *process;
};

#endif // MAINWINDOW_H
