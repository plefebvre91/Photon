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
#include "defs.h"
static const char* xLabels[] =
{
    PHOTON_APERTURE_LABEL,
    PHOTON_ISO_LABEL,
    PHOTON_SHUTTER_SPEED_LABEL,
    PHOTON_FOCAL_LENGTH_LABEL
};

static const char* strPlots[] =
{
    PHOTON_STR_APERTURE,
    PHOTON_STR_ISO,
    PHOTON_STR_SHUTTER_SPEED,
    PHOTON_STR_FOCAL_LENGTH
};

static const char* plotTitles[] =
{
    PHOTON_APERTURE_TITLE,
    PHOTON_ISO_TITLE,
    PHOTON_SHUTTER_SPEED_TITLE,
    PHOTON_FOCAL_LENGTH_TITLE
};

Photon::Photon(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow),
    //mapAperture(), mapISO(), mapShutterSpeed(),
    /*mapFocalLength(), */layoutOverview(), plots(PHOTON_NB_PLOT), maps(PHOTON_NB_PLOT)
{
    ui->setupUi(this);

    /* Connect buttons signals/slots */
    QObject::connect(ui->pushButtonQuit,      SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->pushButtonAnalyze,   SIGNAL(clicked()), this, SLOT(analyze()));
    QObject::connect(ui->pushButtonSave,      SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(ui->pushButtonAddFilter, SIGNAL(clicked()), this, SLOT(addFilter()));

    /* Connect menu signal= QString::s/slots */
    QObject::connect(ui->actionQuit ,     SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionAnalyze,   SIGNAL(triggered()), this, SLOT(analyze()));
    QObject::connect(ui->actionSave ,     SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(ui->actionAddFilter, SIGNAL(triggered()), this, SLOT(addFilter()));

    /* Create and populate our model */
    model = new QDirModel(this);
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    model->setReadOnly(true);

    /* Enable modifying file system */
    ui->treeView->setModel(model);

    /* Select a default selected line */
    QModelIndex first = model->index(0, 0, QModelIndex());
    ui->treeView->setCurrentIndex(first);

    /* Hide useless columns */
    for(int columnIndex=1; columnIndex<3; columnIndex++)
    {
        ui->treeView->hideColumn(columnIndex);
    }

    plots[PHOTON_ID_APERTURE] = ui->plotAperture;
    plots[PHOTON_ID_ISO] = ui->plotISO;
    plots[PHOTON_ID_SHUTTER_SPEED] = ui->plotShutterSpeed;
    plots[PHOTON_ID_FOCAL_LENGTH] = ui->plotFocalLength;

    /* Set plot Y labels */
    int i = 0;
    for(auto p : plots)
    {
        p->yAxis->setLabel(PHOTON_Y_AXIS_LABEL);
        p->setVisible(false);
        p->xAxis->setLabel(xLabels[i]);
        p->plotLayout()->insertRow(0);
        p->plotLayout()->addElement(0, 0, new QCPPlotTitle(p, plotTitles[i]));
        i++;
    }

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
    labels << " ";
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

    plot->clearPlottables();

    QCPBars* myBars = new QCPBars(plot->xAxis, plot->yAxis);

    myBars->setWidthType(QCPBars::wtPlotCoords);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setAutoTickStep(true);
    plot->xAxis->setAutoTickCount(map.size());
    plot->xAxis->setTickVectorLabels(labels);
    plot->xAxis->setTickLabelRotation(PHOTON_X_LABEL_ROTATION);
    plot->xAxis->setRange(1, (double)i+1.5);
    plot->yAxis->setRange(0, *(std::max_element(y.constBegin(),y.constEnd())));

    myBars->setData(x, y);
    plot->addPlottable(myBars);

    plot->replot();

    /*TODO: Libere le new */
}


void Photon::analyze()
{

    for(auto map : maps)
    {
        map.clear();
    }

    for(auto p : plots)
    {
        p->setVisible(false);
    }

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
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, maps[PHOTON_ID_FOCAL_LENGTH]);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS, maps[PHOTON_ID_ISO]);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_APERTURE_VALUE, maps[PHOTON_ID_APERTURE]);
        updateStatistics(ed, EXIF_IFD_EXIF, EXIF_TAG_SHUTTER_SPEED_VALUE, maps[PHOTON_ID_SHUTTER_SPEED]);

        /* Free the EXIF data */
        exif_data_unref(ed);
        nbPics++;
    }

    /* Update plots and make them visible */
    for(auto plot : plots)
    {
        plot->setVisible(true);
    }

    for(int i=0; i<PHOTON_NB_PLOT; i++)
    {
        updatePlot(plots[i], maps[i]);

    }
    this->statusBar()->showMessage(QString::number(nbPics) + " pictures have been analyzed", 10000);
}


void Photon::save()
{

    QDate date = QDate::currentDate();
    QString path = QFileDialog::getExistingDirectory(this,"Choose a directory");

    int i=0;
    for(auto p : plots)
    {
        p->resize(PHOTON_PLOT_DIMENSION_X, PHOTON_PLOT_DIMENSION_Y);
        p->savePdf(path+"/"+strPlots[i++]+"-"+date.toString()+".pdf");
    }

    ui->tabFocalLength->resize(ui->tabWidget->size());

    this->statusBar()->showMessage("Plots have been saved", 10000);
}

void Photon::addFilter()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Add a filter");
}
