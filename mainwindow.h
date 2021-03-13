#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <image.h>
#include <QBarSet>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_open_img_btn_clicked();

    void on_copy_img_btn_clicked();

    void on_rotate_x_btn_clicked();

    void on_rotate_y_btn_clicked();

    void on_luminance_btn_clicked();

    void on_quantize_btn_clicked();

    void on_histogram_btn_clicked();

    void on_brightness_btn_clicked();

    void on_contrast_btn_clicked();

    void on_negative_btn_clicked();

    void DrawImage();

    void DrawBarChart(std::map<int,int> frequency);

private:
    Ui::MainWindow *ui;

public:
    Image src;

public:
    Image copy;

public:
    QPixmap srcImage;

public:
    QPixmap copyImage;

public:
    QMainWindow newWindow;
};
#endif // MAINWINDOW_H
