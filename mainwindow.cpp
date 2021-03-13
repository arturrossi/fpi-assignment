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

using namespace std;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), src(Image()), copy(Image()), srcImage(QPixmap()), copyImage(QPixmap()), newWindow(new QMainWindow()) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_open_img_btn_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Abrir imagem", "/Desktop", "Images (*.png *.xpm *.jpg)");
    QPixmap image(filename);
    srcImage = image;
    ui->src_img->setPixmap(image);
    src.read(filename.toLatin1().data());
}

void MainWindow::on_copy_img_btn_clicked()
{
    copy = Image(src);
    copyImage = srcImage.copy();
    ui->copy_img->setPixmap(copyImage);
}

void MainWindow::on_rotate_x_btn_clicked()
{
    copy.flipX();
    DrawImage();
}

void MainWindow::on_rotate_y_btn_clicked() {
    copy.flipY();
    DrawImage();
}

void MainWindow::on_luminance_btn_clicked() {
    copy.luminance();
    DrawImage();
}

void MainWindow::on_quantize_btn_clicked() {
    int value = ui->quantize_amount->value();
    copy.luminance();
    copy.quantize(value);
    DrawImage();
}

void MainWindow::on_histogram_btn_clicked(){
    std::map<int,int> frequency;

    copy.luminance();
    frequency = copy.histogram();
    DrawImage();
    DrawBarChart(frequency);
}

void MainWindow::on_brightness_btn_clicked() {
    copy.brightness(50);

    DrawImage();
}

void MainWindow::on_contrast_btn_clicked() {
    copy.contrast(2);

    DrawImage();
}

void MainWindow::on_negative_btn_clicked() {
    copy.negative();

    DrawImage();
}

void MainWindow::DrawImage() {
    QImage img(copy.data, copy.w, copy.h, QImage::Format_RGB888);
    copyImage = copyImage.fromImage(img);
    ui->copy_img->setPixmap(copyImage);
}

void MainWindow::DrawBarChart(std::map<int,int> frequency) {
    QBarSet *set = new QBarSet("0");

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
    axisY->setRange(0,2000);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addSeries(series);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);

    chartView->setRenderHint(QPainter::Antialiasing);

    newWindow.setCentralWidget(chartView);
    newWindow.resize(256, 256);
    newWindow.show();
}

