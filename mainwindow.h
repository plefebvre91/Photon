#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirModel>

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
};

#endif // MAINWINDOW_H
