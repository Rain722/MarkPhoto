#include <QHBoxLayout>

#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include "include/dataProcess.h"
#include "include/searchfilter.h"
#include "include/imageviewer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setMinimumSize(608,648);
    this->setMaximumSize(608,648);

    setWindowTitle(tr("数据可视化工具"));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showPicBorse(std::string path) {
    path += std::string("\\");
    picBorse = new QMainWindow();
    picBorse->setMinimumSize(1308,948);
    picBorse->setMaximumSize(1308,948);
    dir_str = path.c_str();
    QStringList filters;
    filters << "*.jpg";
    m_widget_search_filter = new SearchFilter();  //文件浏览
    m_widget_image_viewr = new ImageViewer();  //图片浏览
    m_widget_search_filter->setMinimumWidth(281);
    m_widget_search_filter->setMaximumWidth(251);
    QHBoxLayout* main_layout = new QHBoxLayout();
    main_layout->addWidget(m_widget_search_filter);
    main_layout->addWidget(m_widget_image_viewr);
    QWidget* main_widget = new QWidget(this);
    main_widget->setLayout(main_layout);
    picBorse->setCentralWidget(main_widget);
    m_widget_search_filter->Init(dir_str, filters);
    connect(m_widget_search_filter, SIGNAL(signal_current_select_file(QString)), this, SLOT(on_signal_show_image(QString)));
    connect(m_widget_image_viewr, SIGNAL(signal_next()), this, SLOT(on_signal_next()));
    connect(m_widget_image_viewr, SIGNAL(signal_prev()), this, SLOT(on_signal_prev()));
    picBorse->show();
}

//像素点数据格式
void MainWindow::on_Pixel_clicked() {
    std::string outPutPath;
    if(pixDataProcess(outPutPath)) {
        showPicBorse(outPutPath);
    }
}

//XML数据格式
void MainWindow::on_XML_clicked() {
    std::string outPutPath;
    if(xmlDataProcess(outPutPath)){
        showPicBorse(outPutPath);
    }
}

//自定义数据格式
void MainWindow::on_Customized_clicked() {
    std::string outPutPath;
    if(customizDataProcess(outPutPath)){
        showPicBorse(outPutPath);
    }
}


void MainWindow::on_signal_next()
{
    m_widget_search_filter->SetCurrentSelectFile_Next();
}

void MainWindow::on_signal_prev()
{
    m_widget_search_filter->SetCurrentSelectFile_Prev();
}

void MainWindow::on_signal_show_image(const QString &picture_path)
{
    m_widget_image_viewr->loadFile(picture_path);
}
