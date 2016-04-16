#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

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

    /* Connect menu signals/slots */
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
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}


void MainWindow::analyze()
{
    QMessageBox messageBox;
    messageBox.information(0,"Info", "Analyze");

    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes()[0];
    QDirIterator it(model->filePath(index), QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        qDebug() << it.next();

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
