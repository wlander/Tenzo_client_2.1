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
    int set_header(char*);
public:

    QByteArray buf_reg;
    data_managing* data_mng;
    cdg_control_sd* cntrl_sd_ptr;

    unsigned num_data_recv;

private:

};

#endif // BEAMUNITLIB_H
