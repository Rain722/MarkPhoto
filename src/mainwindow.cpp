#include "include/mainwindow.h"
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
#include <QColor>
#include <thread>
#include <regex>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <QPainter>

#include "include/tiny/tinystr.h"
#include "include/tiny/tinyxml.h"

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
//            std::cout<<i<<" "<<j<<std::endl;
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
    //原始图片
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //像素数据
    std::string pixDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含像素点数据的文件夹", "/").toStdString();
    if(pixDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pix_names = listFiles(pixDatadirectory, "png;jpg");

    auto data_pair = getCommonStrPair(pic_names, pix_names);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toStdString();
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


struct BoxSize
{
    std::string name;
    int xMin, yMin;
    int xMax, yMax;
    double Score;
    bool friend operator < (BoxSize a, BoxSize b){
        return a.Score > b.Score;
    }
};

bool ReadParaXml(std::string m_strXmlPath, std::vector<BoxSize>& vecNode)
{
    BoxSize *pNode = new BoxSize;

    //读取xml文件中的参数值
    TiXmlDocument* Document = new TiXmlDocument();
    if(!Document->LoadFile(m_strXmlPath.c_str()))
    {
        std::cout << "无法加载xml文件！" << std::endl;
        std::cin.get();
        return false;
    }
    TiXmlElement* RootElement = Document->RootElement();		//根目录

    TiXmlElement* NextElement = RootElement->FirstChildElement();		//根目录下的第一个节点层
    //for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
    while(NextElement!=nullptr)		//判断有没有读完
    {
        if(NextElement->ValueTStr() == "object")		//读到object节点
        {
            //NextElement = NextElement->NextSiblingElement();

            TiXmlElement* BoxElement = NextElement->FirstChildElement();
            while(BoxElement->ValueTStr() != "bndbox")		//读到box节点
            {
                if (BoxElement->ValueTStr() == "name") {
                    pNode->name = BoxElement->GetText();
                }
                BoxElement = BoxElement->NextSiblingElement();
            }
            //索引到xmin节点
            TiXmlElement* xminElemeng = BoxElement->FirstChildElement();
            {
                //分别读取四个数值
                pNode->xMin = atoi(xminElemeng->GetText());
                TiXmlElement* yminElemeng = xminElemeng->NextSiblingElement();
                pNode->yMin = atoi(yminElemeng->GetText());
                TiXmlElement* xmaxElemeng = yminElemeng->NextSiblingElement();
                pNode->xMax = atoi(xmaxElemeng->GetText());
                TiXmlElement* ymaxElemeng = xmaxElemeng->NextSiblingElement();
                pNode->yMax = atoi(ymaxElemeng->GetText());

                //加入到向量中
                vecNode.push_back(*pNode);
            }
        }
        NextElement = NextElement->NextSiblingElement();
    }

    //释放内存
    delete pNode;
    delete Document;
    std::cout << "完成xml的读取" << std::endl;
    return true;
}
bool ReadFromTxt(const char* m_strXmlPath, std::vector<BoxSize>& vecNode) {
    std::fstream f(m_strXmlPath);
    std::string line;
    while (getline(f, line))
    {
        std::regex rgx("\\s+");
        std::sregex_token_iterator iter(line.begin(), line.end(), rgx, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> v;
        for ( ; iter != end; ++iter)
            v.push_back(*iter);
        BoxSize node;
        node.xMin = stoi(v[0]);
        node.yMin = stoi(v[1]);
        node.xMax = stoi(v[0]) + stoi(v[2]);
        node.yMax = stoi(v[2]) + stoi(v[3]);
        node.Score = stod(v[4]);
        vecNode.push_back(node);
    }
    return true;
}


//XML数据格式
void MainWindow::on_XML_clicked()
{
    //原始图片
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //xml数据
    std::string xmlDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含XML数据的文件夹", "/").toStdString();
    if(xmlDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto xml_names = listFiles(xmlDatadirectory, "xml");

    auto data_pair = getCommonStrPair(pic_names, xml_names);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toStdString();
        int errorCount = 0;
        //多线程处理
        auto processPic = [&](int process_num, int process_id){
            for(int it=process_id; it<int(data_pair.size()); it+=process_num) {
                auto &i = data_pair[size_t(it)];
                QImage pic;
                pic.load(i.first.c_str());
                std::vector<BoxSize> lines;
                ReadParaXml(i.second.c_str(), lines);
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
    std::string sourcePicdirectory = QFileDialog::getExistingDirectory(this, "选择包含原始jpg文件的文件夹", "/").toStdString();
    if(sourcePicdirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto pic_names = listFiles(sourcePicdirectory, "jpg;png");

    //xml数据
    std::string txtDatadirectory = QFileDialog::getExistingDirectory(this, "选择包含TXT数据的文件夹", "/").toStdString();
    if(txtDatadirectory == "") {
        QMessageBox::critical(this,"错误","未选择文件夹!");
        return;
    }
    auto txt_names = listFiles(txtDatadirectory, "txt");

    auto data_pair = getCommonStrPair(pic_names, txt_names);
    if(data_pair.size() > 0) {
        DialogMsg msg;
        msg.showMsg(int(data_pair.size()));
        std::string outPutDirectory = QFileDialog::getExistingDirectory(this, "选择输出文件夹", "/").toStdString();
        int errorCount = 0;
        //多线程处理
        auto processPic = [&](int process_num, int process_id){
            for(int it=process_id; it<int(data_pair.size()); it+=process_num) {
                auto &i = data_pair[size_t(it)];
                QImage pic;
                pic.load(i.first.c_str());
                std::vector<BoxSize> lines;
                ReadFromTxt(i.second.c_str(), lines);
                std::sort(lines.begin(), lines.end());
                QPainter painter(&pic);
                QPen pen = painter.pen();
                pen.setWidth(3);
                pen.setColor(QColor(250,0,0));
                painter.setPen(pen);
                for(int i=0; i<15&&i<int(lines.size()); ++i) {
                    auto &p = lines[i];
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
