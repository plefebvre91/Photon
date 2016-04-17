#ifndef PHOTON_H
#define PHOTON_H

#include <QMainWindow>
#include <QDirModel>
#include <qcustomplot.h>
#include <QMap>
#include <QString>
#include <libexif/exif-data.h>
namespace Ui {
class MainWindow;
}

class Photon : public QMainWindow
{
    Q_OBJECT

public:
    explicit Photon(QWidget *parent = 0);
    ~Photon();

public slots:
    void analyze();
    void save();
    void addFilter();
    void refresh();

private:
    Ui::MainWindow *ui;
    QDirModel *model;
    QMap<QString, int> mapAperture;
    QMap<QString, int> mapISO;
    QMap<QString, int> mapShutterSpeed;
    QMap<QString, int> mapFocalLength;
    QGridLayout layoutOverview;
int nbPics;
    void updateStatistics(ExifData *d, ExifIfd ifd, ExifTag tag, QMap<QString, int>& map);
    void updatePlot(QCustomPlot* plot, QMap<QString, int>& map);
};

#endif // PHOTON_H
