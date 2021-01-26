#ifndef BEAMUNITLIB_H
#define BEAMUNITLIB_H

#include <QObject>
#include "wadc_defines.h"

class BeamUnitLib : public QObject
{
    Q_OBJECT
public:
    explicit BeamUnitLib(QObject *parent = nullptr);
    ~BeamUnitLib();
signals:

public slots:
    int Handler_unit(char*, int);

public:

    QByteArray buf_reg;
    data_managing* data_mng;
    cdg_control_sd* cntrl_sd_ptr;

    const int num_data_recv = ((SIZE_DATA_CDG*2)+16);

private:

};

#endif // BEAMUNITLIB_H
