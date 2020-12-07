#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QMainWindow>
#include <QTextEdit>
#include <QKeyEvent>
#include <QDebug>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();
protected:
    virtual void keyReleaseEvent(QKeyEvent *ev);

signals:
    void ReturnKey();
};

#endif // MYTEXTEDIT_H
