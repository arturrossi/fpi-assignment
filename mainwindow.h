#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <image.h>
#include <QtCharts>

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

    void on_save_img_btn_clicked();

    void on_copy_img_btn_clicked();

    void on_rotate_x_btn_clicked();

    void on_rotate_y_btn_clicked();

    void on_luminance_btn_clicked();

    void on_quantize_btn_clicked();

    void on_histogram_btn_clicked();

    void on_brightness_btn_clicked();

    void on_contrast_btn_clicked();

    void on_negative_btn_clicked();

    void on_eq_histogram_btn_clicked();

    void on_histogram_matching_btn_clicked();

    void on_go_back_btn_clicked();

    void on_open_original_btn_clicked();

    void on_open_target_btn_clicked();

    void DrawImage(Image img, QLabel* dest);

    void DrawOneChartOnWindow(std::map<int,float> histogram, std::string barChartName, int x, int h);

    void DrawTwoChartsOnWindow(std::map<int,float> histogramOne, std::map<int, float> histogramTwo, std::string leftBarChartName, std::string rightBarChartName, int x , int h);

    void ShowWindow(int x, int h);

    void DrawBarChartOnLayout(QHBoxLayout* layout, QWidget* chart);

    QWidget* DrawBarChart(std::map<int,float> frequency, std::string barChartName, float yRange);

    void DrawLayoutOnFrame(QFrame* frame, QLayout* layout);

    void DrawBarChartOnWindow(QFrame * chartView);

    QStackedWidget* returnStackedWidget();

    void openFile(Image* img, QLabel* dest);

    void on_histogram_matching_btn_2_clicked();

    void on_gauss_clicked();

    void on_laplace_clicked();

    void on_passaaltas_clicked();

    void on_prewitthx_clicked();

    void on_prewitthy_clicked();

    void on_sobelhx_clicked();

    void on_sobelhy_clicked();

    void on_ninety_plus_btn_clicked();

    QList<float> splitString();

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
