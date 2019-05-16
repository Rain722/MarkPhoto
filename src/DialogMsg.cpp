
#include "include/DialogMsg.h"

DialogMsg::DialogMsg(){
    lb = new QLabel();
    dlg = new QDialog();
    ft.setPointSize(15);
    lb->setFont(ft);
    lb->setGeometry(0,0,400,100);//设置位置和大小
    lb->setAlignment(Qt::AlignCenter);//对齐方式
    dlg->setMinimumSize(400,100);
    dlg->setMaximumSize(400,100);
    lb->setParent(dlg);
    dlg->setModal(true);
    Qt::WindowFlags flags= dlg->windowFlags();
    dlg->setWindowFlags(flags&~Qt::WindowContextHelpButtonHint&~Qt::WindowCloseButtonHint);
}
void DialogMsg::showMsg(int msgNum){
    char s[100] = {0};
    sprintf(s, "共发现%d对匹配，处理中...", msgNum);
    lb->setText(s);
    dlg->setVisible(true);
}
void DialogMsg::disShow(){
    dlg->setVisible(false);
}
