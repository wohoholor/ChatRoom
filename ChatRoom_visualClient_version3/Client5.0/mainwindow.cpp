#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    QToolBar* Tbar = new QToolBar();
//    addToolBar(Qt::RightToolBarArea,Tbar);
//    Tbar->setMovable(false);

    QWidget* Loginwindow,*Registerwindow = nullptr;
    QLineEdit* User_ID_edit,*Register_ID = nullptr;
    QLineEdit* User_password_edit,*Register_password,*Register_name = nullptr;
    QPushButton* loginbutton,*Registerbutton = nullptr;
    QCheckBox* Is_display01,* Is_display02 = nullptr;
    MakeLoginwindow(Loginwindow,User_ID_edit,User_password_edit,loginbutton,Is_display01);
    MakeRegisterwindow(Registerwindow,Register_ID,Register_password,Register_name,Registerbutton,Is_display02);

    connect(ui->login_action,&QAction::triggered,Loginwindow,&QWidget::show);
    connect(loginbutton,&QPushButton::released,this,[=]()
    {
        Loginwindow->close();
        ui->login_action->setEnabled(false);
        ui->register_action->setEnabled(false);
        ConnectInit(User_ID_edit->text(),User_password_edit->text());
    });
    connect(Is_display01,&QCheckBox::stateChanged,User_password_edit,[=]()
    {
        if (Is_display01->isChecked())
            User_password_edit->setEchoMode(QLineEdit::Normal);
        else
            User_password_edit->setEchoMode(QLineEdit::Password);
    });


    connect(ui->register_action,&QAction::triggered,Registerwindow,&QWidget::show);
    connect(Registerbutton,&QPushButton::released,this,[=]()
    {
        Registerwindow->close();
        ui->login_action->setEnabled(false);
        ui->register_action->setEnabled(false);
        ConnectInit(Register_ID->text(),Register_password->text(),Register_name->text());
    });
    connect(Is_display02,&QCheckBox::stateChanged,Register_password,[=]()
    {
        if (Is_display02->isChecked())
            Register_password->setEchoMode(QLineEdit::Normal);
        else
            Register_password->setEchoMode(QLineEdit::Password);
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

void add_comma(QString &str)
{
    for (int i =0;i<str.size();++i)
    {
        if (str[i]==',')
        {
           str.push_back(str.front());
           for(int j = str.size()-2;j>i;--j)
               str[j]=str[j-1];
           ++i;
        }
    }
}

void prehandle_str(QString &User_ID,QString &User_password,QString &User_name,QString &result)
{
    add_comma(User_ID);
    add_comma(User_password);
    add_comma(User_name);
    if (User_name.isEmpty())
    {
        result+="#SIGNIN,";
        result+=User_ID;
        result+=",";
        result+=User_password;
    }
    else
    {
        result+="#SIGNUP,";
        result+=User_ID;
        result+=",";
        result+=User_password;
        result+=",";
        result+=User_name;
    }

}

void MainWindow::ConnectInit(QString User_ID,QString User_password,QString User_name)
{
    QString VerifyMsg("");
    prehandle_str(User_ID,User_password,User_name,VerifyMsg);
    p_client = new Client(VerifyMsg);
    qDebug() << VerifyMsg << Qt::endl;
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
