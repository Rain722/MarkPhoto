#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include "include/dataProcess.h"

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

//像素点数据格式
void MainWindow::on_Pixel_clicked() {
    pixDataProcess();
}

//XML数据格式
void MainWindow::on_XML_clicked() {
    xmlDataProcess();
}

//自定义数据格式
void MainWindow::on_Customized_clicked() {
    customizDataProcess();
}
