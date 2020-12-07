#include "mytextedit.h"

MyTextEdit::MyTextEdit(QWidget *parent) : QTextEdit(parent)
{

}

MyTextEdit::~MyTextEdit()
{

}


void MyTextEdit::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Return)
    {
        emit this->ReturnKey();
    }
    QWidget::keyReleaseEvent(ev);
}
