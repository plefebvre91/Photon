#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Create and populate our model */
    model = new QDirModel(this);
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    /* Enable modifying file system */
    model->setReadOnly(true);

    // QTreeView::setModel(QAbstractItemModel * model)
    // Reimplemented from QAbstractItemView::setModel().
    ui->treeView->setModel(model);


//    QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
//    while (it.hasNext())
//        qDebug() << it.next();

}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}
