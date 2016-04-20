#ifndef PHOTONPLOT_H
#define PHOTONPLOT_H
#include <qcustomplot.h>
class PhotonPlot
{
public:
    PhotonPlot();
    void setUiPlot(QCustomPlot* uiPlot);

private:
    QCustomPlot* plot;
    QMap<QString, int> data;
};

#endif // PHOTONPLOT_H
