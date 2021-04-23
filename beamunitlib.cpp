#include "beamunitlib.h"
#include <qdebug.h>
#include <QString>
#include "wadc_defines.h"


BeamUnitLib::BeamUnitLib(QObject *parent) : QObject(parent)
{
    cntrl_sd_ptr = new cdg_control_sd;
    data_mng = new data_managing;

    data_mng->Fl_Connect = false;
    data_mng->Fl_Start = false;
    data_mng->Fl_SD_Ready = false;
    data_mng->Reg_On = false;
    data_mng->Mode_UDP = false;      //else MODE = COM_Port
    data_mng->fl_rcv_data_en = false;
    data_mng->fl_rcv_control_en = false;

    data_mng->N_RCV = N_RCV_DEF;
    data_mng->cnt_recv = 0;
    data_mng->cnt_obr = 0;
    data_mng->cnt_byte_rec = 0;
    data_mng->cnt_cdg_recv = 0;
    data_mng->cnt_block_recv = 0;
    data_mng->Num_Ch = 3;
    data_mng->cnt_reg_recv = 0;

    data_mng->Fd = 7000.0;
    data_mng->Fd_Inv = 1.0/data_mng->Fd;

    data_mng->k_norm[0] = 1.0;
    data_mng->k_norm[1] = 1.0;
    data_mng->k_norm[2] = 1.0;

    num_data_recv = ((SIZE_DATA_CDG*2)+16);

    qDebug("Hello from BeamUnitLib!");
}

BeamUnitLib::~BeamUnitLib()
{
    qDebug("By BeamUnitLib!");
    delete [] cntrl_sd_ptr;
    delete [] data_mng;
}

int BeamUnitLib::Handler_unit(char* data_buf, int num)
{
    unsigned int i,j;
    QString str;
    double mx,mn;
    QVector<double> x(N_RCV_MAX), y(N_RCV_MAX);
    unsigned kk = 0;
    unsigned* rcv_struct;
    short* rcv_data;
    rcv_struct = (unsigned*)data_buf;
    rcv_data = (short*)(rcv_struct+4);

    //Cycle unpack samples ch1 and ch2
    if(data_mng->fl_rcv_data_en){
        kk = 0;
        for(i=0;i<SIZE_DATA_CDG/data_mng->Num_Ch;i++){
            for(j=0;j<data_mng->Num_Ch;j++){
                data_mng->obr_buf_f[j][data_mng->cnt_block_recv] = ((double)cntrl_sd_ptr->data[i*data_mng->Num_Ch+j])*data_mng->k_norm[j];
            }
           data_mng->cnt_block_recv++;
        }//end unpack
    }
    else{
       data_mng->cnt_block_recv = 0;
    }

    if(data_mng->Fl_Start){
        //!!! Write_byte_to_esp(Conf_Byte_Req_Data);
    }

    return 0;
}

int BeamUnitLib::set_header(char* data){
    char* struct_ptr = (char*)this->cntrl_sd_ptr;

    for(int i=0;i<sizeof(cdg_control_sd);i++) struct_ptr[i] = data[i];

    return 0;
}
