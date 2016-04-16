#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libexif/exif-data.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    QVector<double> x(10);
    QVector<double> y(10);
    QVector<QString> labels;

    for(int i=0;i<10;i++){
        x[i] = i;
        y[i] = 3*i;
        labels.push_back("= QString::lol"+QString::number(i));
    }


//    ui->plotAperture->addGraph();
//    ui->plotAperture->graph(0)->setData(x, y);

//    ui->plotAperture->xAxis->setAutoTicks(true);
//    ui->plotAperture->xAxis->setAutoTickLabels(false);
//    ui->plotAperture->xAxis->setTickVectorLabels(labels);

//    ui->plotAperture->xAxis->setRange(0, 13);
//    ui->plotAperture->yAxis->setRange(0, 33);
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}


void MainWindow::show_tag(ExifData *d, ExifIfd ifd, ExifTag tag)
{
  /* See if this tag exists */
  ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
  if (entry) {
    char buf[1024];

    /* Get the contents of the tag in human-readable form */
    exif_entry_get_value(entry, buf, sizeof(buf));
    QString b(buf);
    /* Don't bother printing it if it's entirely blank */
    b = b.trimmed();
    mapAperture[b]++;

    if (!b.isEmpty()) {
        mapAperture[b]++;
        //std::cout << "<" << exif_tag_get_name_in_ifd(tag,ifd) << ">";
      //      printf("<%s> : ", exif_tag_get_name_in_ifd(tag,ifd));
    }


    if (!b.isEmpty()) {
      //std::cout << b << std::endl;
      //      printf("%s\n", buf);//;exif_tag_get_name_in_ifd(tag,ifd), buf);
    }
  }
}


void MainWindow::analyze()
{



    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes()[0];
    QDirIterator it(model->filePath(index), QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        std::cout << it.next().toStdString()<< std::endl;
        ExifData *ed;
        ExifEntry *entry;
        /* Load an ExifData object from an EXIF file */
        ed = exif_data_new_from_file(it.next().toStdString().c_str());
//it.path()
        if (!ed)
        {
            continue;
        }

        /* Show all the tags that might contain information about the
         * photographer
         */
       // show_tag(ed, EXIF_IFD_0, EXIF_TAG_MODEL);
        /* These are much less likely to be useful */
        //show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH);
        //show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM);
        //show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS);
        show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_APERTURE_VALUE);

        //show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_SHUTTER_SPEED_VALUE);

        /* Free the EXIF data */
        exif_data_unref(ed);
    }
QVector<QString> labels;
QVector<double> y;
QVector<double> x;
int i=1;
for(QMap<QString, int>::iterator it = mapAperture.begin(); it!=mapAperture.end(); ++it){
   // std::cout << it.key().toStdString()  << ":" << it.value() << std::endl;
    labels.push_back(it.key());
    y.push_back(it.value());
    x.push_back(i++);
}
QCPBars *myBars = new QCPBars(ui->plotAperture->xAxis, ui->plotAperture->yAxis);
ui->plotAperture->addPlottable(myBars);
ui->plotAperture->addGraph();
myBars->setData(x, y);

//ui->plotAperture->graph(0)->setData(x, y);

ui->plotAperture->xAxis->setAutoTicks(true);
ui->plotAperture->xAxis->setAutoTickLabels(false);
ui->plotAperture->xAxis->setTickVectorLabels(labels);

ui->plotAperture->xAxis->setRange(0, mapAperture.size());
ui->plotAperture->yAxis->setRange(0, mapAperture.);
ui->plotAperture->replot();

}

void MainWindow::save()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Save");
}

void MainWindow::addFilter()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Add a filter");
}

void MainWindow::refresh()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Refresh");
}
