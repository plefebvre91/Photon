#ifndef PHOTON_H
#define PHOTON_H

#include <QMainWindow>
#include <QDirModel>
#include <qcustomplot.h>
#include "photonworkerthread.h"
#include <QMap>
#include <QString>
#include <libexif/exif-data.h>
namespace Ui {
class MainWindow;
class Photon;
}
class PhotonWorkerThread;

class Photon : public QMainWindow
{
    Q_OBJECT

public:
    explicit Photon(QWidget *parent = 0);
    ~Photon();


    void analyze();
public slots:
    void start_thread(void);
    void save();
    void addFilter();

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

    PhotonWorkerThread *p;
};

#endif // PHOTON_H
