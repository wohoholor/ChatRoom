#ifndef POPUP_H
#define POPUP_H
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRegExpValidator>
#include <QCheckBox>

void MakeLoginwindow(QWidget* &Loginwindow,QLineEdit* &User_ID_edit,QLineEdit* &User_password_edit,QPushButton* &loginbutton,QCheckBox* &Is_display);
void MakeRegisterwindow(QWidget* &Registerwindow,QLineEdit* &User_ID_edit,QLineEdit* &User_password_edit,QLineEdit* &User_name,QPushButton* &Register_button,QCheckBox* &Is_display);

#endif // POPUP_H
