#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QTextEdit>
#include <QDebug>
#include <QKeyEvent>
#include "client.h"
#include "mytextedit.h"
#include <QIcon>
#include "Popup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void ConnectInit(QString User_ID,QString User_password,QString User_name = "");
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Client* p_client;

};

#endif // MAINWINDOW_H
