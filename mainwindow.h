#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirModel>
#include <qcustomplot.h>
#include <QMap>
#include <QString>
#include <libexif/exif-data.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void analyze();
    void save();
    void addFilter();
    void refresh();

private:
    Ui::MainWindow *ui;
    QDirModel *model;
    QMap<QString, int> mapAperture;
    void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag);

    void initPlot(QCustomPlot* plot);
};

#endif // MAINWINDOW_H
