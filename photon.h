/* Photon reads your pictures EXIF data and plot the the
 * statistical information.

 * Photon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Photon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Photon.  If not, see <http://www.gnu.org/licenses/>
 */


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
class Photon;
}
class PhotonWorkerThread;

class Photon : public QMainWindow
{
    Q_OBJECT

public:
    explicit Photon(QWidget *parent = 0);
    ~Photon();

public slots:
    void analyze(void);
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
