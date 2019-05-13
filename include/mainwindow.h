#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include <stdio.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Pixel_clicked();

    void on_XML_clicked();

    void on_Customized_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
