#ifndef WADC_DEFINES_H
#define WADC_DEFINES_H

#include <QVector>

//#define DEBUG_MODE

#define N_RCV_MAX 4000
#define N_RCV_DEF 1024
#define N_FLAG 16
#define POS_CH1 2121
#define POS_CH2 2135
#define POS_CH1_SM 2041
#define POS_CH2_SM 2039
#define POS_VIEW_CH1 5000
#define BOTTOM_AXIS 256
#define ADC_CLK 14
#define NUM_BUF_TAH 32
#define START_ID_DATA  0xFEFE
#define HEADER_LEN  2
#define MAX_ID_BUF  128
#define OVERFLOW_DATA  0xFFFF
#define DELETE_DATA  6000

#define HEADER_LEN2  4
#define START_ID_DATA2  0xFF0000FF

#define HEADER_TYPE2

#define POROG_BAD_TAH   15

#define NUM_MEAN_P  3

volatile bool Mode_UDP = true;      //else MODE = COM_Port

volatile char Mode_Op = 0;

const char Conf_Byte_Start = 0x31;
const char Conf_Byte_Stop = 0x30;
const char Power_SD_On = 0x33;
const char RESET_WR_SD = 0x38;
const char Conf_Byte_data_On = 0x34;
const char Conf_Byte_data_Off = 0x35;
const char Conf_Byte_sig_On = 0x36;
const char Conf_Byte_sig_Off = 0x37;

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


int COM_BaudRate = 230400;
bool Fl_Connect = false;
bool Fl_Start = false;
bool fl_write_to_file = false;
bool Fl_SD_Ready = false;
char cnt_tst1 = 0;
volatile bool flag_id_data = false;
volatile int N_RCV = N_RCV_DEF;
volatile int cnt_recv = 0;
volatile int cnt_obr = 0;
volatile int cnt_byte_recv = 0;
volatile int cnt_cdg_recv = 0;
volatile int cnt_byte_obr = 0;
volatile int cnt_block_recv = 0;
volatile int num_data_recv = 80;
volatile int cnt_data_packet = 0;
volatile int  Num_Ch = 2;
volatile bool fl_overflow_in_buf = false;

volatile bool flag_recv_en = false;
volatile bool Reg_On  = false;
volatile int cnt_reg_recv = 0;

bool View_Fresh_Reg = true;
int cnt_p_view = 1;
bool fl_rd_sd = false;
bool fl_wr_sd = false;
double k_norm = 0.0008056641;  // 3.3/2048
double k_norm_enc = 1.0;
volatile bool f_korr = false;
int except_cnt  = 0;
volatile double  mx_ch[3];
volatile double  min_ch[3];
volatile double Koef_M = 1.0;
volatile unsigned NoNo = 0;
volatile double  obr_buf_f[2][N_RCV_MAX];
volatile double Fd = 5786.667; //3968.254; //4464.28;//├Ў
volatile double Fd_Inv;
volatile unsigned cnt_session_recv = 0;
volatile int Fl_0_Before = 0;
volatile int Fl_0_After = 0;
volatile unsigned n_sample = 0;
volatile unsigned Refresh_Cycle_Tah = 1;
volatile bool Tahometr_On = true;
volatile bool Fl_Porog = true;
volatile double PorogTah = 0.4;
volatile unsigned NumChTah = 1;
volatile unsigned NumChVol = 0;
volatile unsigned CntObTah = 0;
volatile unsigned ObMinTah_Aver = 0;
volatile unsigned TimeObTah = 0;
volatile double Sum_Tah = 0;
volatile unsigned CntAverTah = 0;
volatile double TimeAverTah = 1;
volatile unsigned SumTimeAverTah = 0;
volatile double win_mean_p[NUM_MEAN_P] = {0};
volatile unsigned short cnt_mean = 0;
volatile double TimeAverVol = 1;
volatile double Sum_Vol = 0;
volatile double Aver_Vol = 1;
volatile unsigned CntAverVol = 0;
volatile unsigned Detect_Tah = 0;
double Pgd_sredn = 0;
double Pval_sredn = 0;
unsigned cnt_p_sredn = 0;

volatile unsigned Refresh_Cycle_View = 10;
volatile unsigned cnt_cycle_view = 0;
volatile unsigned num_block_view = N_RCV_DEF;
short before_val = 0;

#define SIZE_DATA_CDG 	32

typedef struct
{
    uint32_t cnt_status_write;
    uint32_t cnt_file_sd;
    uint32_t cnt_status_write_all;
    uint8_t stat[4];

    uint16_t data[ SIZE_DATA_CDG];

}cdg_cntrl_sd;


#define START_ID_DATA_ENC  0x3FFFFFFF
#define SPEED_MARK_ENC  0xFFFFFFFF

#endif // WADC_DEFINES_H
