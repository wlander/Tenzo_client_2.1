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
    unsigned* struct_d = (unsigned*)cntrl_sd_ptr;
    short* rcv_data;
    rcv_struct = (unsigned*)data_buf;
    rcv_data = (short*)(rcv_struct+4);

    for(i=0;i<sizeof (cdg_control_sd)/4;i++) struct_d[i] = rcv_struct[i]; //get header from data

    //Cycle unpack samples ch1 and ch2
    if(data_mng->fl_rcv_data_en/*ui->checkBox_2->isChecked()*/){
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

    if((data_mng->cnt_block_recv >= data_mng->N_RCV) && (data_mng->fl_rcv_data_en/*ui->checkBox_2->isChecked()*/)){

        //view_ch_1
        for(j=0;j<data_mng->Num_Ch;j++){

            mx = data_mng->obr_buf_f[j][i];
            mn = data_mng->obr_buf_f[j][i];
            for(int i = 0; i < data_mng->cnt_block_recv; i++){
               x[i] = i;
               y[i] = data_mng->obr_buf_f[j][i];
               if(mx<y[i]) mx = y[i];
               if(mn>y[i]) mn = y[i];
            }

            data_mng->mx_ch[j]=mx;
            data_mng->min_ch[j]=mn;

            if(j==0){
                //!!! ui->Plot1->graph(0)->setData(x,y);
                //!!! ui->Plot1->replot();
            }
            else if(j==1){
                //!!! ui->Plot2->graph(0)->setData(x,y);
                //!!! ui->Plot2->replot();
            }
            else{
                //!!! ui->Plot2_2->graph(0)->setData(x,y);
                //!!! ui->Plot2_2->replot();
            }

        }

        data_mng->cnt_block_recv = 0;
        data_mng->cnt_recv++;
    }
    else{

    }

    if(data_mng->Fl_Start){
        //!!! Write_byte_to_esp(Conf_Byte_Req_Data);
    }

}
