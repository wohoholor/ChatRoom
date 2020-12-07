#include "Popup.h"
void MakeLoginwindow(QWidget* &Loginwindow,QLineEdit* &User_ID_edit,QLineEdit* &User_password_edit,QPushButton* &loginbutton,QCheckBox* &Is_display)
{
    Loginwindow = new QWidget();
    Loginwindow->resize(300,200);
    Loginwindow->setWindowTitle("请登录");
    Loginwindow->setAttribute(Qt::WA_ShowModal, true);

    QLabel *label01=new QLabel("账号(手机号)",Loginwindow);
    label01->move(10,30);
    label01->resize(90,30);
    label01->setAlignment(Qt::AlignCenter);


    User_ID_edit = new QLineEdit(Loginwindow);
    User_ID_edit->move(100,30);
    User_ID_edit->resize(160,30);
    QRegExpValidator *pRevalidotor0 = new QRegExpValidator(QRegExp("[0-9]{25}"), Loginwindow);
    User_ID_edit->setValidator(pRevalidotor0);

    QLabel *label02=new QLabel("密码",Loginwindow);
    label02->move(10,80);
    label02->resize(90,30);
    label02->setAlignment(Qt::AlignCenter);

    User_password_edit = new QLineEdit(Loginwindow);
    User_password_edit->move(100,80);
    User_password_edit->resize(160,30);
    User_password_edit->setEchoMode(QLineEdit::Password);
    QRegExpValidator *pRevalidotor = new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}"), Loginwindow);
    User_password_edit->setValidator(pRevalidotor);
    Is_display = new QCheckBox("显示密码",Loginwindow);
    Is_display->move(40,115);

    loginbutton = new QPushButton("确认登陆",Loginwindow);
    loginbutton->resize(150,40);
    loginbutton->move(80,140);
}

void MakeRegisterwindow(QWidget* &Registerwindow,QLineEdit* &User_ID_edit,QLineEdit* &User_password_edit,QLineEdit* &User_name,QPushButton* &Register_button,QCheckBox* &Is_display)
{
    Registerwindow = new QWidget();
    Registerwindow->resize(300,250);
    Registerwindow->setWindowTitle("请注册");
    Registerwindow->setAttribute(Qt::WA_ShowModal, true);

    QLabel *label01=new QLabel("手机号",Registerwindow);
    label01->move(10,30);
    label01->resize(90,30);
    label01->setAlignment(Qt::AlignCenter);

    User_ID_edit = new QLineEdit(Registerwindow);
    User_ID_edit->move(100,30);
    User_ID_edit->resize(160,30);
    QRegExpValidator *pRevalidotor0 = new QRegExpValidator(QRegExp("[0-9]{25}"), Registerwindow);
    User_ID_edit->setValidator(pRevalidotor0);

    QLabel *label02=new QLabel("密码",Registerwindow);
    label02->move(10,80);
    label02->resize(90,30);
    label02->setAlignment(Qt::AlignCenter);

    User_password_edit = new QLineEdit(Registerwindow);
    User_password_edit->move(100,80);
    User_password_edit->resize(160,30);
    User_password_edit->setEchoMode(QLineEdit::Password);
    QRegExpValidator *pRevalidotor = new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}"), Registerwindow);
    User_password_edit->setValidator(pRevalidotor);
    Is_display = new QCheckBox("显示密码",Registerwindow);
    Is_display->move(40,170);

    QLabel *label03 =new QLabel("昵称",Registerwindow);
    label03->move(10,130);
    label03->resize(90,30);
    label03->setAlignment(Qt::AlignCenter);

    User_name = new QLineEdit(Registerwindow);
    User_name->move(100,130);
    User_name->resize(160,30);

    Register_button = new QPushButton("确认注册",Registerwindow);
    Register_button->resize(150,40);
    Register_button->move(80,200);
}
