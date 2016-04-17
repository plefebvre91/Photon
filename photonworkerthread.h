#ifndef PHOTOWORKERTHREAD_H
#define PHOTOWORKERTHREAD_H
#include "photon.h"
#include <QThread>

class Photon;

class PhotonWorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit PhotonWorkerThread(QObject *parent = 0);
    virtual void run(void);

private:
    Photon* p;

};

#endif // PHOTOWORKERTHREAD_H
