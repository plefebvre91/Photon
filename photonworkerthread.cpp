#include "photonworkerthread.h"

PhotonWorkerThread::PhotonWorkerThread(QObject *parent) :
    QThread(parent)
{
    p = (Photon*)parent;
}

void PhotonWorkerThread::run(void) {
    p->analyze();
}


