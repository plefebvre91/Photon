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

#include "photon.h"
#include "ui_mainwindow.h"
#include "libexif/exif-data.h"
#include <iostream>
#include <QtConcurrent/QtConcurrent>

#define PHOTON_STR_APERTURE "Aperture"
#define PHOTON_STR_ISO      "ISO"
#define PHOTON_STR_SHUTTER_SPEED "Shutter speed"
#define PHOTON_STR_FOCAL_LENGTH "Focal lens"

#define PHOTON_MAX_TAG_SIZE 1024
#define PHOTON_Y_AXIS_LABEL        "Times used"
#define PHOTON_ISO_LABEL           PHOTON_STR_ISO " speed"
#define PHOTON_SHUTTER_SPEED_LABEL PHOTON_STR_SHUTTER_SPEED " (s)"
#define PHOTON_APERTURE_LABEL      PHOTON_STR_APERTURE " (f)"
#define PHOTON_FOCAL_LENGTH_LABEL  PHOTON_STR_FOCAL_LENGTH " (mm)"

#define PHOTON_APERTURE_TITLE      PHOTON_STR_APERTURE " usage"
#define PHOTON_SHUTTER_SPEED_TITLE PHOTON_STR_SHUTTER_SPEED " usage"
#define PHOTON_ISO_TITLE           PHOTON_STR_ISO " usage"
#define PHOTON_FOCAL_LENGTH_TITLE  PHOTON_STR_FOCAL_LENGTH " usage"

#define PHOTON_X_LABEL_ROTATION "-60"


Photon::Photon(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow),
    mapAperture(), mapISO(), mapShutterSpeed(),
    mapFocalLength(), layoutOverview()
{
    ui->setupUi(this);

    /* Connect buttons signals/slots */
    QObject::connect(ui->pushButtonQuit,      SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->pushButtonAnalyze,   SIGNAL(clicked()), this, SLOT(analyze()));
    QObject::connect(ui->pushButtonSave,      SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(ui->pushButtonAddFilter, SIGNAL(clicked()), this, SLOT(addFilter()));
    QObject::connect(ui->pushButtonRefresh,   SIGNAL(clicked()), this, SLOT(refresh()));

    /* Connect menu signal= QString::s/slots */
    QObject::connect(ui->actionQuit ,     SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionAnalyze,   SIGNAL(triggered()), this, SLOT(analyze()));
    QObject::connect(ui->actionSave ,     SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(ui->actionAddFilter, SIGNAL(triggered()), this, SLOT(addFilter()));
    QObject::connect(ui->actionRefresh ,  SIGNAL(triggered()), this, SLOT(refresh()));

    /* Create and populate our model */
    model = new QDirModel(this);
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    model->setReadOnly(true);

    /* Enable modifying file system */
    ui->treeView->setModel(model);

    /* Select a default*/
    //ui->treeView->selectionModel()->setCurrentIndex(QModelIndex(0),);
    QModelIndex first = model->index(0, 0, QModelIndex());
    ui->treeView->setCurrentIndex(first);


    ui->plotAperture->yAxis->setLabel(PHOTON_Y_AXIS_LABEL);
    ui->plotISO->yAxis->setLabel(PHOTON_Y_AXIS_LABEL);
    ui->plotShutterSpeed->yAxis->setLabel(PHOTON_Y_AXIS_LABEL);
    ui->plotFocalLength->yAxis->setLabel(PHOTON_Y_AXIS_LABEL);

    ui->plotAperture->xAxis->setLabel(PHOTON_APERTURE_LABEL);
    ui->plotISO->xAxis->setLabel(PHOTON_ISO_LABEL);
    ui->plotShutterSpeed->xAxis->setLabel(PHOTON_SHUTTER_SPEED_LABEL);
    ui->plotFocalLength->xAxis->setLabel(PHOTON_FOCAL_LENGTH_LABEL);

    ui->plotAperture->plotLayout()->insertRow(0);
    ui->plotAperture->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plotAperture, PHOTON_APERTURE_TITLE));

    ui->plotShutterSpeed->plotLayout()->insertRow(0);
    ui->plotShutterSpeed->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plotShutterSpeed, PHOTON_SHUTTER_SPEED_TITLE));

    ui->plotISO->plotLayout()->insertRow(0);
    ui->plotISO->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plotISO, PHOTON_ISO_TITLE));

    ui->plotFocalLength->plotLayout()->insertRow(0);
    ui->plotFocalLength->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plotFocalLength, PHOTON_FOCAL_LENGTH_TITLE));

    ui->plotAperture->setVisible(false);
    ui->plotISO->setVisible(false);
    ui->plotFocalLength->setVisible(false);
    ui->plotShutterSpeed->setVisible(false);

    this->showMaximized();

}

Photon::~Photon()
{
    delete model;
    delete ui;
}

void Photon::updateStatistics(ExifData *d, ExifIfd ifd, ExifTag tag, QMap<QString, int>& map)
{
    /* See if this tag exists */
    ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);

    if(!entry)
    {
        return;
    }

    char buf[PHOTON_MAX_TAG_SIZE];

    /* Get the contents of the tag in human-readable form */
    exif_entry_get_value(entry, buf, sizeof(buf));

    QString b(buf);

    /* Don't bother printing it if it's entirely blank */
    b = b.trimmed();

    if (!b.isEmpty()) {
        map[b]++;
    }
}

void Photon::updatePlot(QCustomPlot* plot, QMap<QString, int>& map)
{    
    QVector<QString> labels;
    QVector<double> y;
    QVector<double> x;
    int i=1;
labels << "0";
    for(QMap<QString, int>::iterator it = map.begin(); it!=map.end(); ++it){
        if(plot == ui->plotAperture || plot == ui->plotShutterSpeed)
        {
            int begin = it.key().indexOf('(');
            int end = it.key().indexOf(')');
            QString l = it.key().mid(begin+1, end-begin-1);
            labels.push_back(l);

        }
        else
            labels.push_back(it.key());

        y.push_back(it.value());
        x.push_back(++i);
    }

    plot->clearGraphs();
    plot->clearPlottables();
    plot->clearItems();
    plot->xAxis->setAutoTickStep(true);
    plot->xAxis->setAutoTickCount(map.size());
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVectorLabels(labels);

    plot->xAxis->setTickLabelRotation(PHOTON_X_LABEL_ROTATION);

    plot->xAxis->setRange(1, (double)i+1.5);
    plot->yAxis->setRange(0, *(std::max_element(y.constBegin(),y.constEnd())));

    QCPBars* myBars = new QCPBars(plot->xAxis, plot->yAxis);
    plot->addPlottable(myBars);
    myBars->setData(x, y);


    plot->replot();

/*TODO: Libere le new */

}


void Photon::analyze()
{

    mapAperture.clear();
    mapISO.clear();
    mapShutterSpeed.clear();
    mapFocalLength.clear();

    ui->plotAperture->setVisible(false);
    ui->plotISO->setVisible(false);
    ui->plotFocalLength->setVisible(false);
    ui->plotShutterSpeed->setVisible(false);

    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes()[0];
    QDirIterator it(model->filePath(index), QStringList() << "*.JPG", QDir::Files, QDirIterator::Subdirectories);
    ExifData *ed;
   // ExifEntry *entry;
    nbPics = 0;
    while (it.hasNext()) {
        /* Load an ExifData object from an EXIF file */
        ed = exif_data_new_from_file(it.next().toStdString().c_str());

        if (!ed)
        {
            continue;
        }

        /* Show all the tags that might contain information about the
         * photographer
         */
        //show_tag(ed, EXIF_IFD_0, EXIF_TAG_MODEL);
        /* These are much less likely to be useful */
       // updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH, mapFocalLength);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, mapFocalLength);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS, mapISO);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_APERTURE_VALUE, mapAperture);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_SHUTTER_SPEED_VALUE, mapShutterSpeed);

        /* Free the EXIF data */
        exif_data_unref(ed);
        nbPics++;
    }

    updatePlot(ui->plotAperture, mapAperture);
    updatePlot(ui->plotFocalLength, mapFocalLength);
    updatePlot(ui->plotShutterSpeed, mapShutterSpeed);
    updatePlot(ui->plotISO, mapISO);

    this->statusBar()->showMessage(QString::number(nbPics) + " pictures have been analyzed", 10000);

    ui->plotAperture->setVisible(true);
    ui->plotISO->setVisible(true);
    ui->plotFocalLength->setVisible(true);
    ui->plotShutterSpeed->setVisible(true);

}


void Photon::save()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Save");
}

void Photon::addFilter()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Add a filter");
}

void Photon::refresh()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Refresh");
}
