#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <cstring>
#include <qt_windows.h>
#include <qtextbrowser.h>
#include <QMessageBox>
#include <QLabel>
#include <QColor>
#include <thread>
#include <QPainter>

#include "include/dataReader.h"
#include "include/strTools.h"

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
    //原始图片
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toLocal8Bit().toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //像素数据
    std::string pixDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含像素点数据的文件夹", "/").toLocal8Bit().toStdString();
    if(pixDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pix_names = listFiles(pixDatadirectory, "png;jpg");

    auto data_pair = getCommonStrPair(pic_names, pix_names);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toLocal8Bit().toStdString();
        int errorCount = 0;
        //多线程处理
        auto processPic = [&](int process_num, int process_id){
            for(int it=process_id; it<int(data_pair.size()); it+=process_num) {
                auto &i = data_pair[size_t(it)];
                QImage pic, pix;
                pic.load(i.first.c_str());
                pix.load(i.second.c_str());
                if(pic.size().width()!=pix.size().width() || pic.size().height()!=pix.size().height()){
                    errorCount ++;
                    continue;
                }
                for(int i=0; i<pic.size().width(); ++i) {
                    for(int j=0; j<pic.size().height(); ++j) {
                        int r = qRed(pix.pixel(i, j)), b = qBlue(pix.pixel(i, j)), g = qGreen(pix.pixel(i, j));
                        if(r!=0 || g!=0 || b!=0) {
                            QColor color(r, g, b);
                            pic.setPixelColor(i,j,color);
                        }
                    }
                }
                std::string outPutPath = outPutDirectory + std::string("\\") +
                        i.first.substr(i.first.rfind("/")+1, i.first.rfind(".")-i.first.rfind("/")-1)
                        + std::string(".pix.jpg");
                pic.save(outPutPath.c_str());
            }
        };
        const int process_num = 6;
        std::thread process[process_num];
        for(int i=0; i<process_num; ++i)
            process[i] = std::thread(processPic, process_num, i);
        for(int i=0; i<process_num; ++i)
            process[i].join();

        msg.disShow();
        char s[100];
        if(errorCount != 0)
            sprintf(s, "处理完成，期中%d对发生错误(分辨率不匹配)!", errorCount);
        else
            sprintf(s, "处理完成!");
        QMessageBox::information(this,"OK",s);
    }
    else {
        QMessageBox::critical(this,"错误","未发现相匹配的文件!");
    }

}

//XML数据格式
void MainWindow::on_XML_clicked()
{
    //原始图片
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toLocal8Bit().toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //xml数据
    std::string xmlDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含XML数据的文件夹", "/").toLocal8Bit().toStdString();
    if(xmlDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto xml_names = listFiles(xmlDatadirectory, "xml");

    auto data_pair = getCommonStrPair(pic_names, xml_names);
    std::atomic_int errorXML(0);
    std::atomic_int errorPic(0);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toLocal8Bit().toStdString();
        int errorCount = 0;
        //多线程处理
        auto processPic = [&](int process_num, int process_id){
            for(int it=process_id; it<int(data_pair.size()); it+=process_num) {
                auto &i = data_pair[size_t(it)];
                QImage pic;
                if(!pic.load(i.first.c_str())) {
                    errorPic ++;
                    continue;
                }
                std::vector<BoxSize> lines;
                if(!ReadParaXml(i.second.c_str(), lines)) {
                    errorXML ++;
                    continue;
                }
                QPainter painter(&pic);
                QPen pen = painter.pen();
                pen.setWidth(3);
                pen.setColor(QColor(250,0,0));
                painter.setPen(pen);
                for(auto &p: lines)
                    painter.drawRect(p.xMin,p.yMin,p.xMax-p.xMin,p.yMax-p.yMin);
                painter.end();
                std::string outPutPath = outPutDirectory + std::string("\\") +
                        i.first.substr(i.first.rfind("/")+1, i.first.rfind(".")-i.first.rfind("/")-1)
                        + std::string(".xml.jpg");
                pic.save(outPutPath.c_str());
            }
        };
        const int process_num = 6;
        std::thread process[process_num];
        for(int i=0; i<process_num; ++i)
            process[i] = std::thread(processPic, process_num, i);
        for(int i=0; i<process_num; ++i)
            process[i].join();

        msg.disShow();
        char s[100];
        if(errorCount != 0)
            sprintf(s, "处理完成，期中%d对发生错误(分辨率不匹配)!", errorCount);
        else
            sprintf(s, "处理完成!");
        QMessageBox::information(this,"OK",s);
    }
    else {
        QMessageBox::critical(this,"错误","未发现相匹配的文件!");
    }

}

//自定义数据格式
void MainWindow::on_Customized_clicked()
{
    //原始图片
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toLocal8Bit().toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //xml数据
    std::string txtDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含TXT数据的文件夹", "/").toLocal8Bit().toStdString();
    if(txtDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto txt_names = listFiles(txtDatadirectory, "txt");

    auto data_pair = getCommonStrPair(pic_names, txt_names);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toLocal8Bit().toStdString();
        int errorCount = 0;
        //多线程处理
        std::atomic_int errorTXT(0);
        std::atomic_int errorPic(0);
        auto processPic = [&](int process_num, int process_id){
            for(int it=process_id; it<int(data_pair.size()); it+=process_num) {
                auto &i = data_pair[size_t(it)];
                QImage pic;
                if(!pic.load(i.first.c_str())){
                    errorPic ++;
                    continue;
                }
                std::vector<BoxSize> lines;
                if(!ReadFromTxt(i.second.c_str(), lines)){
                    errorTXT ++;
                    continue;
                }
                std::sort(lines.begin(), lines.end());
                QPainter painter(&pic);
                QPen pen = painter.pen();
                pen.setWidth(3);
                pen.setColor(QColor(250,0,0));
                painter.setPen(pen);
                for(int i=0; i<15&&i<int(lines.size()); ++i) {
                    auto &p = lines[size_t(i)];
                    std::cout<<p.Score<<std::endl;
                    painter.drawRect(p.xMin,p.yMin,p.xMax-p.xMin,p.yMax-p.yMin);
                }
                painter.end();
                std::string outPutPath = outPutDirectory + std::string("\\") +
                        i.first.substr(i.first.rfind("/")+1, i.first.rfind(".")-i.first.rfind("/")-1)
                        + std::string(".txt.jpg");
                pic.save(outPutPath.c_str());
            }
        };
        const int process_num = 6;
        std::thread process[process_num];
        for(int i=0; i<process_num; ++i)
            process[i] = std::thread(processPic, process_num, i);
        for(int i=0; i<process_num; ++i)
            process[i].join();

        msg.disShow();
        char s[100];
        if(errorCount != 0)
            sprintf(s, "处理完成，期中%d对发生错误(分辨率不匹配)!", errorCount);
        else
            sprintf(s, "处理完成!");
        QMessageBox::information(this,"OK",s);
    }
    else {
        QMessageBox::critical(this,"错误","未发现相匹配的文件!");
    }
}
