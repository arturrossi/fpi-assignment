#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image.h"
#include <QPixmap>
#include <QFileDialog>
#include <QtCharts>
#include <QBarSet>
#include <QBarSeries>
#include <QChart>
#include <QBarCategoryAxis>
#include <sstream>
#include <stdlib.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), src(Image()), copy(Image()), srcImage(QPixmap()), copyImage(QPixmap()), newWindow(new QMainWindow()) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_save_img_btn_clicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Salvar imagem", "imagem", "Images (*.png *.xpm *.jpg)");
    if (filename.isEmpty())
            return;
    else {
        QByteArray ba = filename.toLocal8Bit();
        const char *str_filename = ba.data();
        copy.write(str_filename);
    }
}


void MainWindow::on_open_img_btn_clicked() {
    openFile(&src, ui->src_img);
}

void MainWindow::on_copy_img_btn_clicked()
{
    copy = Image(src);
    copyImage = srcImage.copy();
    ui->copy_img->setPixmap(copyImage);
    ui->copy_img->setScaledContents(true);
}

void MainWindow::on_rotate_x_btn_clicked()
{
    copy.flipX();
    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_rotate_y_btn_clicked() {
    copy.flipY();
    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_luminance_btn_clicked() {
    copy.luminance();
    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_quantize_btn_clicked() {
    int value = ui->quantize_amount->value();
    copy.luminance();
    copy.quantize(value);
    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_histogram_btn_clicked(){
    std::map<int,float> histogram;

    copy.luminance();
    histogram = copy.histogram();
    DrawImage(copy, ui->copy_img);

    DrawOneChartOnWindow(histogram, "Histograma", 256, 256);
}

void MainWindow::on_brightness_btn_clicked() {
    copy.brightness(50);

    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_contrast_btn_clicked() {
    copy.contrast(1.5);

    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_negative_btn_clicked() {
    copy.negative();

    DrawImage(copy, ui->copy_img);
}

void MainWindow::on_eq_histogram_btn_clicked() {
    map<int,float> default_histogram;
    map<int, float> eq_histogram;

    default_histogram = copy.histogram();

    Image *new_img = new Image(copy);

    DrawImage(copy, ui->orig_img);

    *new_img = copy.histogram_equalization(new_img);

    eq_histogram = new_img->histogram();

    DrawImage(*new_img, ui->copy_img);

    DrawTwoChartsOnWindow(default_histogram, eq_histogram, "Histograma padrão", "Histograma equalizado", 1024, 512);
}


void MainWindow::on_histogram_matching_btn_2_clicked() {
    src.histogram_matching(copy);

//    for (int i = 0; i < src.h*src.w*src.channels; i+=3) {
//        qDebug("%d %d %d", src.data[i], src.data[i+1], src.data[i+2]);
//    }

    DrawImage(src, ui->result_img);
    ui->result_img->setScaledContents(true);
}


void MainWindow::on_histogram_matching_btn_clicked() {
    returnStackedWidget()->setCurrentIndex(1);
}

void MainWindow::on_go_back_btn_clicked() {
    returnStackedWidget()->setCurrentIndex(0);
}

void MainWindow::on_open_original_btn_clicked() {
    openFile(&src, ui->orig_img);
}

void MainWindow::on_open_target_btn_clicked() {
    openFile(&copy, ui->target_img);
}

void MainWindow::DrawImage(Image img, QLabel* dest) {
    QImage qimage(img.data, img.w, img.h, QImage::Format_RGB888);
    copyImage = copyImage.fromImage(qimage);
    dest->setPixmap(copyImage);
}

void MainWindow::DrawOneChartOnWindow(map<int,float> histogram, string barChartName, int x, int h) {
    QFrame* centralFrame = new QFrame();

    QHBoxLayout* bar_charts_layout = new QHBoxLayout();

    QWidget* default_histogramChart = DrawBarChart(histogram, barChartName, 1);

    DrawBarChartOnLayout(bar_charts_layout, default_histogramChart);

    DrawLayoutOnFrame(centralFrame, bar_charts_layout);

    DrawBarChartOnWindow(centralFrame);

    ShowWindow(x, h);
}

void MainWindow::DrawTwoChartsOnWindow(map<int,float> histogramOne, map<int, float> histogramTwo, string leftBarChartName, string rightBarChartName, int x, int h) {
    QFrame* centralFrame = new QFrame();

    QHBoxLayout* bar_charts_layout = new QHBoxLayout();

    QWidget* default_histogramChart = DrawBarChart(histogramOne, leftBarChartName, 1);

    QWidget* eq_histogramChart = DrawBarChart(histogramTwo, rightBarChartName, 1.0);

    DrawBarChartOnLayout(bar_charts_layout, default_histogramChart);
    DrawBarChartOnLayout(bar_charts_layout, eq_histogramChart);

    DrawLayoutOnFrame(centralFrame, bar_charts_layout);

    DrawBarChartOnWindow(centralFrame);

    ShowWindow(x, h);
}

QWidget* MainWindow::DrawBarChart(std::map<int,float> frequency, string barChartName, float yRange) {
    QBarSet *set = new QBarSet("Ocorrencias de intensidade");

    QBarSeries *series = new QBarSeries();

    QChart *chart = new QChart();

    QStringList categories;

    for (auto const& [key, val]: frequency) {
        set->append(val);
    }

    for (int i = 0; i < 256; i ++) {
        categories.append(QString::number(i));
    }

    series->append(set);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, yRange);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QString qstr = QString::fromStdString(barChartName);

    chart->addSeries(series);
    chart->setTitle(qstr);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);

    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void MainWindow::DrawBarChartOnLayout(QHBoxLayout* layout, QWidget* chart) {
    layout->addWidget(chart);
}

void MainWindow::DrawBarChartOnWindow(QFrame *chartView) {
    newWindow.setCentralWidget(chartView);
}

void MainWindow::DrawLayoutOnFrame(QFrame* frame, QLayout* layout) {
    frame->setLayout(layout);
}

void MainWindow::ShowWindow(int x, int h) {
    newWindow.resize(x, h);
    newWindow.show();
}

QStackedWidget* MainWindow::returnStackedWidget() {
    return ui->centralwidget->findChild<QStackedWidget*>("stackedWidget");
}

void MainWindow::openFile(Image* img, QLabel* dest) {
    QString filename = QFileDialog::getOpenFileName(this, "Abrir imagem", "/Desktop", "Images (*.png *.xpm *.jpg)");
    QPixmap image(filename);
    srcImage = image;
    img->read(filename.toLatin1().data());
    dest->setPixmap(srcImage);
    dest->setScaledContents(true);
}

void MainWindow::on_gauss_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 0);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_laplace_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 0);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_passaaltas_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 0);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_prewitthx_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 127);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_prewitthy_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    for (int i = 0; i < 9; i++){
        qDebug("%f", list[i]);
    }

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };



    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 127);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_sobelhx_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 127);

    DrawImage(*new_img, ui->copy_img);
}

void MainWindow::on_sobelhy_clicked() {
    QList<float> list;

    list = splitString();

    vector<vector<float>> kernel;

    kernel = {
        {list[0], list[1], list[2]},
        {list[3], list[4], list[5]},
        {list[6], list[7], list[8]},
    };

    Image *new_img = new Image(copy);

    *new_img = copy.convolution_no_borders(kernel, new_img, 127);

    DrawImage(*new_img, ui->copy_img);
}

QList<float> MainWindow::splitString() {
    QList<float> list;

    string s = ui->weight_input->displayText().toStdString();

    istringstream iss(s);

    while(getline(iss, s, ' ')) {
        list.append(atof(s.c_str()));
    }

    return list;
}


void MainWindow::on_ninety_plus_btn_clicked() {
    Image* new_img = new Image(copy);

    *new_img = copy.rotate90(new_img);

    DrawImage(*new_img, ui->copy_img);
}
