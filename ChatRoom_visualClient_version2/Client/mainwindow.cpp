#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    QToolBar* Tbar = new QToolBar();
//    addToolBar(Qt::RightToolBarArea,Tbar);
//    Tbar->setMovable(false);

    QWidget* Loginwindow = new QWidget();
    Loginwindow->resize(300,200);
    QLineEdit *nameeidt = new QLineEdit("请输入你的昵称",Loginwindow);
    nameeidt->move(55,40);
    nameeidt->resize(200,40);
    QPushButton* loginbutton = new QPushButton("确认登陆",Loginwindow);
    loginbutton->resize(150,40);
    loginbutton->move(80,120);
    connect(ui->login_action,&QAction::triggered,Loginwindow,&QWidget::show);
    connect(loginbutton,&QPushButton::released,this,[=]()
    {
        Loginwindow->close();
        ui->login_action->setEnabled(false);
        ConnectInit(nameeidt->text());
    });
    ui->FileIcon->setIcon(QIcon(":/pic/file.jpg"));
    ui->SmileIcon->setIcon(QIcon(":/pic/smile.jpeg"));
    ui->FileIcon->setCursor(QCursor(Qt::PointingHandCursor));
    ui->SmileIcon->setCursor(QCursor(Qt::PointingHandCursor));
    ui->QQxiu->setStyleSheet("border-image:url(:/pic/timg.jpg)");
    ui->TextB->setFontPointSize(12);

}

MainWindow::~MainWindow()
{
    closesocket(p_client->sockfd);
    delete ui;
}

void MainWindow::ConnectInit(const QString &name)
{
    p_client = new Client(name);
    connect(p_client,&Client::GetMsg,ui->TextB,[=]()
    {
        if (p_client->MyColor)
            ui->TextB->setTextColor(QColor(Qt::GlobalColor::black));
        else
            ui->TextB->setTextColor(QColor(Qt::GlobalColor::darkGreen));
        ui->TextB->append(p_client->Currmsg);
    }
    );
    connect(p_client,&Client::closeconnect,this,&MainWindow::close);
    connect(ui->SendButton,&QPushButton::clicked,ui->TextE,[=]()
    {
        QString str = ui->TextE->toPlainText();
        if(str.size()!=0)
        {
            ui->TextE->clear();
            p_client->SendMsg(str.toUtf8().constData());
        }
    });
    connect(ui->TextE,&MyTextEdit::ReturnKey,ui->TextE,[=]()
    {
        QString str = ui->TextE->toPlainText();
        str = str.left(str.length()-1);
        if(str.size()!=0)
        {
            ui->TextE->clear();
            p_client->SendMsg(str.toUtf8().constData());
        }
    });
}
