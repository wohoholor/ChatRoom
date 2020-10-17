#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QTextEdit>
#include <QDebug>
#include <QKeyEvent>
#include "client.h"
#include "mytextedit.h"
#include <QLineEdit>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void ConnectInit(const QString &name);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Client* p_client;

};

#endif // MAINWINDOW_H
