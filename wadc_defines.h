#ifndef WADC_DEFINES_H
#define WADC_DEFINES_H
#include <QString>

//#define DEBUG_MODE

#define N_RCV_MAX 4000
#define N_RCV_DEF 640
#define START_ID_DATA_ENC  0x3FFFFFFF
#define SPEED_MARK_ENC  0xFFFFFFFF
#define SIZE_DATA_CDG 	30

const int COM_BaudRate = 115200;
const char Conf_Byte_Start = 0x31;
const char Conf_Byte_Stop = 0x30;
const char Power_SD_On = 0x33;
const char RESET_WR_SD = 0x38;
const char Conf_Byte_data_On = 0x34;
const char Conf_Byte_data_Off = 0x35;
const char Conf_Byte_sig_On = 0x36;
const char Conf_Byte_sig_Off = 0x37;
const char Conf_Byte_Ask_Ok = 0x39;
const char SD_Rec_Off = 0x41;
const char SD_Rec_On = 0x42;
const char Conf_Byte_Req_Data = 94;
const char Conf_Byte_Pack_Mode_En = 28;
const char Conf_Byte_Pack_Mode_Dis = 29;
const char Conf_Byte_Pack_Mode_Ch1 = 30;
const char Conf_Byte_Pack_Mode_Ch2 = 31;
const char Conf_Byte_Recv_ad8555_Par = 35;
const char Conf_Byte_On_ad8555 = 36;
const char Conf_Byte_Off_ad8555 = 37;
const char Conf_Byte_Enc_Porog = 38;
const char Conf_Byte_Enc_On = 39;
const char Conf_Byte_Enc_Off = 40;
const char Conf_Byte_Enc_View_On = 41;
const char Conf_Byte_Enc_View_Off = 42;

#define MAX_STAT    8

const QString SD_STAT[8] = {"ST_SD_OK", "ST_SD_ERR_GINF", "ST_SD_ERR_WR", "ST_SD_ERR_FULL", "_", "ST_SD_ERR_INIT", "ST_SD_ERR_RD0", "ST_SD_ERR_WR0"};


typedef volatile struct
{
    bool Fl_Connect;
    bool Fl_Start;
    bool Fl_SD_Ready;
    bool Reg_On;
    bool Mode_UDP;      //else MODE = COM_Port
    bool fl_rcv_data_en;
    bool fl_rcv_control_en;

    unsigned int N_RCV;
    unsigned int cnt_recv;
    unsigned int cnt_obr;
    unsigned int cnt_byte_rec;
    unsigned int cnt_cdg_recv;
    unsigned int cnt_block_recv;
    unsigned int  Num_Ch;
    unsigned int cnt_reg_recv;

    double Fd;
    double Fd_Inv;

    double  mx_ch[3];
    double  min_ch[3];
    double  k_norm[3];
    double  obr_buf_f[3][N_RCV_MAX];

}data_managing;


typedef volatile struct
{
    unsigned cnt_status_write;
    unsigned cnt_file_sd;
    unsigned cnt_status_write_file;
    unsigned char stat[4];
    unsigned short data[SIZE_DATA_CDG];

}cdg_control_sd;


#endif // WADC_DEFINES_H
