
#ifndef DIALOGMSG_H
#define DIALOGMSG_H

#include <QMessageBox>
#include <QLabel>
#include <QColor>
#include <thread>
#include <QPainter>
class DialogMsg{
public:
    DialogMsg();
    void showMsg(int msgNum);
    void disShow();
private:
    QLabel* lb = nullptr;
    QFont ft;
    QDialog* dlg = nullptr;
};

#endif 