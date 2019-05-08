#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cstring>
#include <io.h>
#include <qt_windows.h>
#include <vector>
#include <qtextbrowser.h>
#include <QMessageBox>
#include <QLabel>

bool startWith(std::string s, std::string sub) {
    transform(s.begin(),s.end(),s.begin(),::tolower);
    transform(sub.begin(),sub.end(),sub.begin(),::tolower);
    return s.find(sub)==0?1:0;
}

bool endWith(std::string s,std::string sub) {
    transform(s.begin(),s.end(),s.begin(),::tolower);
    transform(sub.begin(),sub.end(),sub.begin(),::tolower);
    return s.rfind(sub)==(s.length()-sub.length())?1:0;
}

//字符串分隔
std::vector<std::string> split(const std::string &s, const std::string &sep)
{
    std::vector<std::string> v;
    std::string::size_type pos1 = 0, pos2 = s.find(sep);
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + sep.size();
        pos2 = s.find(sep, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v;
}

//获取当前文件夹下指定后缀的文件列表
std::vector<std::string> listFiles(const std::string &string_dir, const std::string &end_str)
{
    char dirNew[200];
    strcpy(dirNew, string_dir.c_str());
    strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索
    std::vector<std::string> res;
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dirNew, &findData);
    if (handle == -1)        // 检查是否成功
        return res;
    auto end_str_split = split(end_str, ";");
    do {
        std::string dName = std::string(findData.name);
        if (findData.attrib & _A_SUBDIR)
            continue;
        else {
            for(auto &i : end_str_split) {
                if(endWith(dName, i)) {
                    res.push_back(string_dir + std::string("/")+ dName);
                    break;
                }
            }
        }
    } while (_findnext(handle, &findData) == 0);
    _findclose(handle);    // 关闭搜索句柄
    return res;
}

//获取前缀相同的文件对
std::vector<std::pair<std::string, std::string>>
            getCommonStrPair(std::vector<std::string> &pic_names, std::vector<std::string> &data_names) {
    std::vector<std::pair<std::string, std::string> > pair_arr;
    for(auto &i : pic_names){
        for(auto &j : data_names) {
            auto istart = i.rfind("/"), iend = i.rfind(".");
            auto jstart = j.rfind("/"), jend = j.rfind(".");
            if(iend-istart != jend-jstart)
                continue;
            std::string one = i.substr(istart+1, iend-istart-1);
            std::string two = j.substr(jstart+1, jend-jstart-1);
            if(one == two)
                pair_arr.push_back(std::make_pair(i, j));
        }
    }
    return pair_arr;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(608,648);
    this->setMaximumSize(608,648);
    setWindowTitle(tr("数据可视化工具"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//像素点数据格式
void MainWindow::on_Pixel_clicked()
{
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");


}

//XML数据格式
void MainWindow::on_XML_clicked()
{
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    std::string xmlDatadirectory = QFileDialog::getExistingDirectory(this, "选择保存XML文件的文件夹", "").toStdString();
    auto xml_names = listFiles(xmlDatadirectory, "xml");

    auto data_pair = getCommonStrPair(pic_names, xml_names);

    for(auto &i : data_pair){
        std::cout<<i.first<<" "<<i.second<<std::endl;
    }

}

class DialogMsg{
public:
    DialogMsg(){
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
    void showMsg(int msgNum){
        char s[100] = {0};
        sprintf(s, "共发现%d对匹配，处理中...", msgNum);
        lb->setText(s);
        dlg->setVisible(true);
    }
    void disShow(){
        dlg->setVisible(false);
    }
private:
    QLabel* lb = nullptr;
    QFont ft;
    QDialog* dlg = nullptr;
};

//自定义数据格式
void MainWindow::on_Customized_clicked()
{

    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");


    std::string txtDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含自定义数据的文件夹", "").toStdString();
    if(txtDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto txt_names = listFiles(txtDatadirectory, "txt");

    auto data_pair = getCommonStrPair(pic_names, txt_names);
    if(data_pair.size() > 0) {

        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        Sleep(10000);
        for(int i=0; i<800000; ++i){
            std::cout<<"hh"<<std::endl;
        }
        msg.disShow();
        QMessageBox::information(this,"OK","处理完成!");
    }
    else {
        QMessageBox::critical(this,"错误","未发现相匹配的文件!");
    }

}
