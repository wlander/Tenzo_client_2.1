#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <unistd.h>
#include <errno.h>
#include <QThread>
#include "beamunitlib.h"
#include "wadc_defines.h"
#include "myudp.h"
#include "users_lang.h"

BeamUnitLib *beamunitlib;
Users_Lang *u_lng;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QVector<double> x(N_RCV_MAX), y(N_RCV_MAX);

    ui->setupUi(this);


    //------------- The class is base of control and processing of data ---------------
    beamunitlib = new BeamUnitLib();//Создаем обьект по классу

    beamunitlib->data_mng->fl_rcv_data_en = ui->checkBox_2->isChecked();
    beamunitlib->data_mng->fl_rcv_control_en = ui->checkBox->isChecked();

    for(int i = 0; i < beamunitlib->data_mng->N_RCV; i++){
       x[i] = i;
       y[i] = -4.0+(double)i*(8.0/(double)beamunitlib->data_mng->N_RCV);
    }

    u_lng = new Users_Lang();
    u_lng->set_lang("ru");

    ui->Button_Start_Stop->setEnabled(false);

    ui->groupBox_7->setVisible(false);
    ui->pushButton_7->setVisible(false);
    ui->pushButton_9->setVisible(false);
    ui->lineEdit_10->setVisible(false);

    ui->checkBox->setEnabled(false);
    ui->checkBox_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
//----------- настройка QCustomPlot -----------------------------------------------
    ui->Plot1->addGraph();
    ui->Plot2->addGraph();
    ui->Plot2_2->addGraph();

    ui->Plot1->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->Plot2->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->Plot2_2->axisRect()->setRangeZoom(Qt::Horizontal);

    ui->Plot1->graph(0)->setData(x,y);
    ui->Plot1->xAxis->setRange(1, beamunitlib->data_mng->N_RCV);
    ui->Plot1->yAxis->setRange(-2.0, 2.0);

    ui->Plot2->graph(0)->setData(x,y);
    ui->Plot2->xAxis->setRange(1, beamunitlib->data_mng->N_RCV);
    ui->Plot2->yAxis->setRange(-2.0, 2.0);

    ui->Plot2_2->graph(0)->setData(x,y);
    ui->Plot2_2->xAxis->setRange(1, beamunitlib->data_mng->N_RCV);
    ui->Plot2_2->yAxis->setRange(-2.0, 2.0);

    ui->Plot1->replot();
    ui->Plot2->replot();
    ui->Plot2_2->replot();

    ui->lineEdit_3->setText("2");
    ui->lineEdit_4->setText("-2");
    ui->lineEdit_6->setText("2");
    ui->lineEdit_7->setText("-2");
    ui->lineEdit_8->setText("2");
    ui->lineEdit_9->setText("-2");
    ui->lineEdit_11->setText("1000");

    if(!beamunitlib->data_mng->Mode_UDP){
        ui->Button_Connect->setText(u_lng->get_p(u_lng->Connect));
        ui->label_19->setText(u_lng->get_p(u_lng->Disconnected));
    }
    else{
        ui->Button_Connect->setText(u_lng->get_p(u_lng->Check_connection));
        ui->label_19->setText(u_lng->get_p(u_lng->Asked_connection));
    }

    ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: black;");
    ui->Button_Start_Stop->setStyleSheet("QPushButton { background-color : lightgray; }");
    ui->label_tah->setStyleSheet("QLabel { background-color : yellow; }");
    ui->label_2->setText(u_lng->get_p(u_lng->SD_not_activated));
    ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Start));

    ui->label_13->setText(u_lng->get_p(u_lng->Files_in_SD) + "-");
    ui->label_9->setText(u_lng->get_p(u_lng->Samples_in_cur_file) + "-");
    ui->label->setText(u_lng->get_p(u_lng->Total_data_in_SD) + "-");
    ui->checkBox_2->setText(u_lng->get_p(u_lng->Signal));
    ui->checkBox->setText(u_lng->get_p(u_lng->Control));
    ui->pushButton_2->setText(u_lng->get_p(u_lng->Clear_SD));
    ui->pushButton_3->setText(u_lng->get_p(u_lng->Auto));
    ui->pushButton_8->setText(u_lng->get_p(u_lng->Auto));
    ui->pushButton_6->setText(u_lng->get_p(u_lng->Auto));
    ui->pushButton_9->setText(u_lng->get_p(u_lng->Auto));
//---------------------------------------------------------------------------------

//------ Creating and start thread for UDP or COM Port in dependense Mod_UDP param (wadc_defines.h) -----------------

    QThread *thread_New = new QThread;//Создаем поток для порта платы

    if(beamunitlib->data_mng->Mode_UDP){
        MyUDP *udpc = new MyUDP();
        udpc->moveToThread(thread_New);

        connect(udpc,SIGNAL(outUDP(QByteArray)),this,SLOT(ReadFromPort(QByteArray)));
        connect(this,SIGNAL(writeData(QByteArray)),udpc,SLOT(UDPWrite(QByteArray)));
        connect(this,SIGNAL(SayHelloUDP()),udpc,SLOT(SayHello()));
    }
    else{ //Mode = COM Port
        Port *PortNew = new Port();//COM port class instance
        PortNew->set_num_data_recv(beamunitlib->num_data_recv); //set count data for calling signal that data complete
        PortNew->moveToThread(thread_New);//put COM port class instance into thread
        PortNew->thisPort.moveToThread(thread_New);

        connect(PortNew, SIGNAL(error_(QString)), this, SLOT(Print(QString)));//error log
        connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));//override run
        connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));//override exit
        connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));//delete thread
        connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));//delete thread
        connect(this,SIGNAL(savesettings(QString,int,int,int,int,int)),PortNew,SLOT(Write_Settings_Port(QString,int,int,int,int,int)));//Слот - ввод настроек!
        connect(this,SIGNAL(Signal_SetRunRecv(bool Recv_En)),PortNew,SLOT(SetRunRecv(bool Recv_En)));
        connect(this,SIGNAL(Signal_SetRunReg(bool Reg_En)),PortNew,SLOT(SetRunReg(bool Reg_En)));
        connect(this, SIGNAL(Connect_Port()),PortNew,SLOT(ConnectPort()));//connect port
        connect(this, SIGNAL(Disconnect_Port()),PortNew,SLOT(DisconnectPort()));//disconnect port
        connect(PortNew, SIGNAL(outPort(QByteArray)), this, SLOT(ReadFromPort(QByteArray)));//handler for recieved data
        connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));//send to port
        connect(this,SIGNAL(Clear_Buf_Data(void)),PortNew,SLOT(ClearBuf1(void)));

    }

    thread_New->start(QThread::LowestPriority);
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Button_Connect_clicked()
{
    QString file1;
    int i;
    QFile file;

if(!beamunitlib->data_mng->Mode_UDP){

    if(beamunitlib->data_mng->Fl_Connect==false){

        beamunitlib->data_mng->Fl_Connect = true;
        ui->Button_Connect->setText(u_lng->get_p(u_lng->Disconnect));
        ui->Button_Start_Stop->setEnabled(true);

        savesettings(ui->PortNameEdit->text(),COM_BaudRate,8,0,1,0);
        Connect_Port();
        ui->Button_Connect->setEnabled(false);
        QThread::msleep(1000);
        ui->Button_Connect->setEnabled(true);

        ui->label_19->setText(u_lng->get_p(u_lng->Stoped));
        ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: blue;");

        ui->checkBox_2->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->pushButton_2->setEnabled(true);

    }else{


        Disconnect_Port();
        beamunitlib->data_mng->Fl_Connect = false;

        ui->Button_Connect->setText(u_lng->get_p(u_lng->Connect));
        ui->Button_Start_Stop->setEnabled(false);
        ui->label_19->setText(u_lng->get_p(u_lng->Disconnected));
        ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: black;");
        ui->Button_Connect->setEnabled(false);
        QThread::msleep(1000);
        ui->Button_Connect->setEnabled(true);
        ui->checkBox_2->setEnabled(false);
        ui->checkBox->setEnabled(false);

        beamunitlib->data_mng->Fl_SD_Ready = false;
    }

    Signal_SetRunRecv(beamunitlib->data_mng->Fl_Connect);

}
else{

    ui->label_19->setText(u_lng->get_p(u_lng->Asked_connection));
    Write_byte_to_esp(Conf_Byte_Req_Data);
    //SayHelloUDP();

    }

}

void MainWindow::on_Button_Start_Stop_clicked()
{
  QString file1;
  unsigned int i;
  QFile file;

//  Fl_SD_Ready = 1; //!!!

 if(beamunitlib->data_mng->Fl_SD_Ready){

    if(beamunitlib->data_mng->Fl_Start==false){

        beamunitlib->data_mng->cnt_recv = 0;
        beamunitlib->data_mng->Fl_Start = true;

        ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Stop));
        ui->Button_Connect->setEnabled(false);
        ui->checkBox->setEnabled(false);
        Write_byte_to_serial_port(Conf_Byte_Start);


    }else{

        beamunitlib->data_mng->Fl_Start = false;
        ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Start));
        //ui->Button_Start_Stop->setStyleSheet("QPushButton { background-color : lightgray; }");
        ui->Button_Connect->setEnabled(true);

        Write_byte_to_serial_port(Conf_Byte_Stop);

        ui->Button_Start_Stop->setEnabled(false);
        QThread::msleep(1000);
        ui->Button_Start_Stop->setEnabled(true);

        ui->checkBox->setEnabled(true);

    }
 }
 else{

    Write_byte_to_esp(Power_SD_On);
    ui->Button_Start_Stop->setEnabled(false);
    QThread::msleep(1000);
    ui->Button_Start_Stop->setEnabled(true);
    beamunitlib->data_mng->Fl_SD_Ready = true;
    ui->pushButton_2->setEnabled(false);
 }

}


void MainWindow::on_pushButton_5_clicked()
{

}

void MainWindow::ReadFromPort(QByteArray data)
{
    unsigned int i,j;
    QString str;
    double mx,mn;
    QVector<double> x(N_RCV_MAX), y(N_RCV_MAX);
    Signal_SetRunRecv(0);

 beamunitlib->set_header(data.data());

 if((beamunitlib->cntrl_sd_ptr->stat[0]==3) && ui->checkBox->isChecked()){

    beamunitlib->Handler_unit(data.data(),data.count());

    ui->label_13->setText(u_lng->get_p(u_lng->Files_in_SD) + QString::number(beamunitlib->cntrl_sd_ptr->cnt_file_sd));
    ui->label_9->setText(u_lng->get_p(u_lng->Samples_in_cur_file) + QString::number(((beamunitlib->cntrl_sd_ptr->cnt_status_write_file*512)/beamunitlib->data_mng->Num_Ch)/2));
    ui->label->setText(u_lng->get_p(u_lng->Total_data_in_SD) + QString::number(((beamunitlib->cntrl_sd_ptr->cnt_status_write*512)/beamunitlib->data_mng->Num_Ch)/2));

 //ЗДЕСЬ ЗАКОНЧИЛ!!!

    if((beamunitlib->data_mng->cnt_block_recv >= beamunitlib->data_mng->N_RCV) && (ui->checkBox_2->isChecked())){

        //view_ch_1
        for(j=0;j<beamunitlib->data_mng->Num_Ch;j++){

            mx = beamunitlib->data_mng->obr_buf_f[j][0];
            mn = beamunitlib->data_mng->obr_buf_f[j][0];
            for(int i = 0; i < beamunitlib->data_mng->cnt_block_recv; i++){
               x[i] = i;
               y[i] = beamunitlib->data_mng->obr_buf_f[j][i];
               if(mx<y[i]) mx = y[i];
               if(mn>y[i]) mn = y[i];
            }

            beamunitlib->data_mng->mx_ch[j]=mx;
            beamunitlib->data_mng->min_ch[j]=mn;

            if(j==0){
                ui->Plot1->graph(0)->setData(x,y);
                ui->Plot1->replot();
            }
            else if(j==1){
                ui->Plot2->graph(0)->setData(x,y);
                ui->Plot2->replot();
            }
            else{
                ui->Plot2_2->graph(0)->setData(x,y);
                ui->Plot2_2->replot();
            }

        }

        beamunitlib->data_mng->cnt_block_recv = 0;
        beamunitlib->data_mng->cnt_recv++;
    }
    else{

    }

    if(beamunitlib->data_mng->Fl_Start){
        Write_byte_to_esp(Conf_Byte_Req_Data);
    }

  }
  else if(beamunitlib->cntrl_sd_ptr->stat[0]==1){
     ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Write));
     ui->label_2->setText(u_lng->get_p(u_lng->SD_not_activated));
     ui->label_19->setText(u_lng->get_p(u_lng->Connection_OK));
     beamunitlib->data_mng->Fl_SD_Ready = true;
  }
  else if(beamunitlib->cntrl_sd_ptr->stat[0]==2){
    ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Stop));
    ui->label_2->setText(u_lng->get_p(u_lng->SD_writing_running));
    ui->label_19->setText(u_lng->get_p(u_lng->Connection_OK));
  }
  else if(beamunitlib->cntrl_sd_ptr->stat[0]==4){
   ui->Button_Start_Stop->setText(u_lng->get_p(u_lng->Write));
   ui->label_2->setText(u_lng->get_p(u_lng->SD_writing_stoped));
   ui->label_19->setText(u_lng->get_p(u_lng->Connection_OK));
  }
  else if(beamunitlib->cntrl_sd_ptr->stat[0]==5){
    ui->label_19->setText(u_lng->get_p(u_lng->Connection_OK));
  }

    Signal_SetRunRecv(1);

}


void MainWindow::on_pushButton_7_clicked()
{
    //Fl_0_Before = 1;
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
}

void MainWindow::on_lineEdit_returnPressed()
{
    //PorogTah = ui->lineEdit->text().toDouble();
}

void MainWindow::Write_byte_to_serial_port(char cb)
{
  QByteArray data_cdg;

    data_cdg.append(cb);
    writeData(data_cdg);
    data_cdg.clear();

}

void MainWindow::Write_byte_to_esp(char cb)
{
    QByteArray data_cdg;

    data_cdg.append(cb);
//    data_cdg.append(0x0A);
    writeData(data_cdg);
    data_cdg.clear();

}

void MainWindow::on_lineEdit_3_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_3->text().toDouble();
    Ym_2 = ui->lineEdit_4->text().toDouble();
    ui->Plot1->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot1->replot();
}

void MainWindow::on_lineEdit_4_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_3->text().toDouble();
    Ym_2 = ui->lineEdit_4->text().toDouble();
    ui->Plot1->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot1->replot();
}

//*******************************************************************************
//Функция выполняет запись в файл
int write_in_file(char *the_data, unsigned int the_size){
    QString str = QFileDialog::getSaveFileName(0, "Файл для записи");

    if(str == ""){
        return 1;
    }

    QFile file;
    file.setFileName(str);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        return 2;
    }

    file.write(the_data, the_size);
    file.close();

    return 0;
}

void MainWindow::on_lineEdit_6_returnPressed()
{

    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_6->text().toDouble();
    Ym_2 = ui->lineEdit_7->text().toDouble();
    ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2->replot();

}

void MainWindow::on_lineEdit_7_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_6->text().toDouble();
    Ym_2 = ui->lineEdit_7->text().toDouble();
    ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2->replot();
}

void MainWindow::on_pushButton_3_clicked()
{
    double Ym_1;
    double Ym_2;

    if(beamunitlib->data_mng->cnt_recv>0){

        Ym_1 = beamunitlib->data_mng->mx_ch[0]+beamunitlib->data_mng->mx_ch[0]*0.3;
        Ym_2 = beamunitlib->data_mng->min_ch[0]-beamunitlib->data_mng->min_ch[0]*0.3;
        ui->Plot1->yAxis->setRange(Ym_2, Ym_1);
        ui->Plot1->replot();

        ui->lineEdit_3->setText(QString::number(Ym_1));
        ui->lineEdit_4->setText(QString::number(Ym_2));

    }
}

void MainWindow::on_pushButton_8_clicked()
{   
    double Ym_1;
    double Ym_2;

    if(beamunitlib->data_mng->cnt_recv>0){

        Ym_1 = beamunitlib->data_mng->mx_ch[1]+beamunitlib->data_mng->mx_ch[1]*0.3;
        Ym_2 = beamunitlib->data_mng->min_ch[1]-beamunitlib->data_mng->min_ch[1]*0.3;
        ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
        ui->Plot2->replot();

        ui->lineEdit_6->setText(QString::number(Ym_1));
        ui->lineEdit_7->setText(QString::number(Ym_2));

    }
}

void MainWindow::on_lineEdit_8_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_8->text().toDouble();
    Ym_2 = ui->lineEdit_9->text().toDouble();
    ui->Plot2_2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2_2->replot();
}

void MainWindow::on_lineEdit_9_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_8->text().toDouble();
    Ym_2 = ui->lineEdit_9->text().toDouble();
    ui->Plot2_2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2_2->replot();
}

void MainWindow::on_lineEdit_10_returnPressed()
{

}

void MainWindow::on_lineEdit_11_returnPressed()
{

        double Xm_1;
        double Xm_2;
        Xm_1 = 1;
        Xm_2 = ui->lineEdit_11->text().toDouble();
        if(Xm_2>N_RCV_MAX){
            Xm_2 = N_RCV_MAX;
            ui->lineEdit_11->setText(QString::number(N_RCV_MAX));
        }
        beamunitlib->data_mng->N_RCV = (int)Xm_2;
        ui->Plot1->xAxis->setRange(Xm_1, Xm_2);
        ui->Plot2->xAxis->setRange(Xm_1, Xm_2);
        ui->Plot2_2->xAxis->setRange(Xm_1, Xm_2);

        ui->Plot1->replot();
        ui->Plot2->replot();
        ui->Plot2_2->replot();
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->Plot2->xAxis->setRange(0, 2000);
    ui->Plot2->replot();
    ui->lineEdit_10->setText("0");
    ui->lineEdit_11->setText("2000");
}

void MainWindow::on_pushButton_6_clicked()
{
    double Ym_1;
    double Ym_2;

    if(beamunitlib->data_mng->cnt_recv>0){

        Ym_1 = beamunitlib->data_mng->mx_ch[2]+beamunitlib->data_mng->mx_ch[2]*0.3;
        Ym_2 = beamunitlib->data_mng->min_ch[2]-beamunitlib->data_mng->min_ch[2]*0.3;
        ui->Plot2_2->yAxis->setRange(Ym_2, Ym_1);
        ui->Plot2_2->replot();

        ui->lineEdit_8->setText(QString::number(Ym_1));
        ui->lineEdit_9->setText(QString::number(Ym_2));

    }
}


void MainWindow::on_pushButton_2_clicked()
{
   QByteArray data_cdg;
   int i;

   data_cdg.append(RESET_WR_SD);
   writeData(data_cdg);
   data_cdg.clear();
   QThread::msleep(1000);



}

void MainWindow::closeEvent(QCloseEvent *event)
{

}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{

}
