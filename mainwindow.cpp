#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>
#include <QDesktopWidget>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QFileDialog>
#include <QMetaEnum>
#include <unistd.h>
#include <errno.h>
#include <QThread>
#include "wadc_defines.h"
#include <QSettings>
#include "myudp.h"

QByteArray reg_buf;
QString strr;
QString filereg;
QString Fd_str;
QString Volts_inv_str;
cdg_cntrl_sd* cntrl_sd_ptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QVector<double> x(N_RCV_MAX), y(N_RCV_MAX);

    ui->setupUi(this);

    QString fileName = QCoreApplication::applicationDirPath()+"/settings.ini";

    if (QFile::exists(fileName)) {
         QSettings settings(fileName, QSettings::IniFormat);

         ui->PortNameEdit->setText(settings.value("SETTINGS/COM_PORT", "COM5").toString());
         ui->lineEdit->setText(settings.value("SETTINGS/porog", "0.4").toString());
         ui->spinBox_2->setValue(settings.value("SETTINGS/time_mean", "1000").toInt());
         ui->lineEdit_14->setText(settings.value("SETTINGS/koef_usil", "0.0008056641").toString());
         ui->lineEdit_15->setText(settings.value("SETTINGS/moment_sopr", "0.0008056641").toString());
         ui->lineEdit_12->setText(settings.value("SETTINGS/kpd_reduktora", "700000").toString());
         ui->lineEdit_13->setText(settings.value("SETTINGS/pered_otnoshenie", "0.0008056641").toString());
         Fd_str = settings.value("SETTINGS/Fd", "3500").toString();
         Volts_inv_str = settings.value("SETTINGS/1_div_Volts", "0.0008056641").toString();

//         ui->lineEdit_2->setText(settings.value("SETTINGS/Fd", "3500").toString()); //DEL
//         ui->lineEdit_5->setText(settings.value("SETTINGS/1_div_Volts", "0.0008056641").toString());

    }

    N_RCV = ui->lineEdit_7->text().toInt();

    for(int i = 0; i < N_RCV; i++)
    {
       x[i] = i;
       y[i] = -4.0+(double)i*(8.0/(double)N_RCV);
    }




/*
    ui->PortNameBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->PortNameBox->addItem(info.portName());
    }
*/

    ui->Button_Start_Stop->setEnabled(false);

    ui->groupBox_7->setVisible(false);
    ui->groupBox_2->setVisible(false);
    ui->pushButton_7->setVisible(false);
   // ui->pushButton_2->setVisible(false);
    ui->groupBox_11->setVisible(false);


    ui->checkBox->setEnabled(false);
    ui->checkBox_2->setEnabled(false);
//----------- настройка QCustomPlot -----------------------------------------------
    ui->Plot1->addGraph();
    ui->Plot2->addGraph();

    ui->Plot1->axisRect()->setRangeZoom(Qt::Horizontal);

    ui->Plot1->graph(0)->setData(x,y);
    ui->Plot2->graph(0)->setData(x,y);
    ui->Plot1->xAxis->setRange(0, N_RCV);
    ui->Plot1->yAxis->setRange(-2.0, 2.0);
    ui->Plot2->xAxis->setRange(0, N_RCV);
    ui->Plot2->yAxis->setRange(-2.0, 2.0);
    ui->Plot1->replot();
    ui->Plot2->replot();

    ui->lineEdit_3->setText("2");
    ui->lineEdit_4->setText("-2");
    ui->lineEdit_8->setText("2");
    ui->lineEdit_9->setText("-2");

    ui->label_19->setText("Отключено");
    ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: black;");
    ui->Button_Start_Stop->setStyleSheet("QPushButton { background-color : lightgray; }");

    ui->label_tah->setStyleSheet("QLabel { background-color : yellow; }");

    k_norm = Volts_inv_str.toDouble(); //ui->lineEdit_5->text().toDouble();

    Fd_Inv = 1.0/Fd_str.toDouble();  //ui->lineEdit_2->text().toDouble();

    ui->label_2->setText("SD не активирована!");

//---------------------------------------------------------------------------------

//------ Creating and start thread for UDP or COM Port in dependense Mod_UDP param (wadc_defines.h) -----------------

    QThread *thread_New = new QThread;//Создаем поток для порта платы

    if(Mode_UDP){
        MyUDP *udpc = new MyUDP();
        udpc->moveToThread(thread_New);
        \
        connect(udpc,SIGNAL(outUDP(QByteArray)),this,SLOT(ReadFromPort(QByteArray)));
        connect(this,SIGNAL(writeData(QByteArray)),udpc,SLOT(UDPWrite(QByteArray)));
        connect(this,SIGNAL(SayHelloUDP()),udpc,SLOT(SayHello()));
    }
    else{ //Mode = COM Port
        Port *PortNew = new Port();//Создаем обьект по классу
        PortNew->moveToThread(thread_New);//помешаем класс  в поток
        PortNew->thisPort.moveToThread(thread_New);//Помещаем сам порт в поток

        connect(PortNew, SIGNAL(error_(QString)), this, SLOT(Print(QString)));//Лог ошибок
        connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));//Переназначения метода run
        connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));//Переназначение метода выход
        connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));//Удалить к чертям поток
        connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));//Удалить к чертям поток
        connect(this,SIGNAL(savesettings(QString,int,int,int,int,int)),PortNew,SLOT(Write_Settings_Port(QString,int,int,int,int,int)));//Слот - ввод настроек!
        connect(this,SIGNAL(Signal_SetRunRecv(bool Recv_En)),PortNew,SLOT(SetRunRecv(bool Recv_En)));
        connect(this,SIGNAL(Signal_SetRunReg(bool Reg_En)),PortNew,SLOT(SetRunReg(bool Reg_En)));
        connect(this, SIGNAL(Connect_Port()),PortNew,SLOT(ConnectPort()));//по нажатию кнопки подключить порт
        connect(this, SIGNAL(Disconnect_Port()),PortNew,SLOT(DisconnectPort()));//по нажатию кнопки отключить порт
        connect(PortNew, SIGNAL(outPort(QByteArray)), this, SLOT(ReadFromPort(QByteArray)));//вывод в текстовое поле считанных данных
        connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));//отправка в порт данных
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
    QByteArray data_cdg;
    QString file1;
    int i;
    short* ptr_reg_data;
    short sample = 0;
    double fsample = 0.0;
    QFile file;

    if(Fl_Connect==false){

        Fl_Connect = true;
        ui->Button_Connect->setText("Отключить");
        ui->Button_Start_Stop->setEnabled(true);

        if(!Mode_UDP){
            savesettings(ui->PortNameEdit->text(),COM_BaudRate,8,0,1,0);
            Connect_Port();
        }
        else{
            data_cdg.append(Conf_Byte_Start);
            SayHelloUDP();
            data_cdg.clear();
        }
//        ui->listWidget->addItem("Подключение к порту");

        ui->label_19->setText("Остановлено");
        ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: blue;");

        for(i=0;i<1000000;i++){i=i;}

        ui->checkBox_2->setEnabled(true);
        ui->checkBox->setEnabled(true);

    }else{
        Fl_Connect = false;
        ui->Button_Connect->setText("Подключить");
        ui->Button_Start_Stop->setEnabled(false);

        if(!Mode_UDP){
            Disconnect_Port();
        }

//        ui->listWidget->addItem("Отключение от порта");
        ui->label_19->setText("Отключено");
        ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: black;");

        for(i=0;i<1000000;i++){i=i;}

        ui->checkBox_2->setEnabled(false);
        ui->checkBox->setEnabled(false);
    }

    if(!Mode_UDP){Signal_SetRunRecv(Fl_Connect);}
}

void MainWindow::on_Button_Start_Stop_clicked()
{
  QByteArray data_cdg;
  QString file1;
  unsigned int i;
  char* ptr_reg_data;
  unsigned short sample = 0;
  double fsample = 0.0;
  unsigned start_pos_data=0;
  unsigned int kk=0;
  QFile file;

  char* ptr_sample = (char*)&sample;

  unsigned int sample_u;
  char* ptr_sample_u = (char*)&sample_u;

if(Fl_SD_Ready){
    if(Fl_Start==false){

        cnt_recv = 0;
        Fl_Start = true;
        CntObTah = 0;
        Pgd_sredn = 0;
        Pval_sredn = 0;
        cnt_p_sredn = 0;

        ui->Button_Start_Stop->setText("Стоп");
        ui->Button_Connect->setEnabled(false);
        ui->checkBox->setEnabled(false);

        data_cdg.append(Conf_Byte_Start);
        writeData(data_cdg);
        data_cdg.clear();

    }else{

        //ui->label_19->setText("Остановлено");
        //ui->label_19->setStyleSheet("font-weight: bold; font-size: 10pt; color: blue;");
        Fl_Start = false;
        ui->Button_Start_Stop->setText("Старт");
        //ui->Button_Start_Stop->setStyleSheet("QPushButton { background-color : lightgray; }");
        ui->Button_Connect->setEnabled(true);

        data_cdg.append(Conf_Byte_Stop);
        writeData(data_cdg);
        data_cdg.clear();
        for(i=0;i<100000;i++){i=i;}

        ui->checkBox->setEnabled(true);

    }
 }
 else{
 //    ui->label_19->setText("Готов к записи");
     data_cdg.append(Power_SD_On);
     writeData(data_cdg);
     data_cdg.clear();
     for(i=0;i<100000;i++){i=i;}
     //Fl_SD_Ready = true;
 }

}


void MainWindow::on_pushButton_5_clicked()
{

}

void MainWindow::ReadFromPort(QByteArray data)
{
    unsigned int i,j;
    QString str;
    char* tmc;
    double mx,mn;
    QVector<double> x(N_RCV_MAX), y(N_RCV_MAX);
    unsigned kk = 0;
    unsigned* rcv_struct;
    short* rcv_data;
    rcv_struct = (unsigned*)data.data();
    rcv_data = (short*)(rcv_struct+4);
    cntrl_sd_ptr = (cdg_cntrl_sd*)rcv_struct;

 if((cntrl_sd_ptr->stat[0]==3) && ui->checkBox->isChecked()){

    ui->label_13->setText("Файлов на SD: " + QString::number(cntrl_sd_ptr->cnt_file_sd));
    ui->label_9->setText("Сэмплов в тек. файле: " + QString::number(cntrl_sd_ptr->cnt_status_write));
    ui->label->setText("Всего сэмплов на SD: " + QString::number(cntrl_sd_ptr->cnt_status_write_all));

    //Cycle unpack samples ch1 and ch2
    if(ui->checkBox_2->isChecked()){
        kk = 0;
        for(i=0;i<SIZE_DATA_CDG/2;i++){
            obr_buf_f[0][cnt_block_recv] = ((double)cntrl_sd_ptr->data[i*2])*k_norm; //rcv_data[i*Num_Ch]*k_norm);
            obr_buf_f[1][cnt_block_recv] = ((double)cntrl_sd_ptr->data[i*2+1])*k_norm;
            cnt_block_recv++;
        }//end unpack
    }
    else{
       cnt_block_recv = 0;
    }

    if((cnt_block_recv >= N_RCV) && (ui->checkBox_2->isChecked())){

        //view_ch_1
        mx = obr_buf_f[0][i];
        mn = obr_buf_f[0][i];
        for(int i = 0; i < cnt_block_recv; i++){
           x[i] = i;
           y[i] = obr_buf_f[0][i];
           if(mx<y[i]) mx = y[i];
           if(mn>y[i]) mn = y[i];
        }

        mx_ch[0]=mx;
        min_ch[0]=mn;

        ui->Plot1->graph(0)->setData(x,y);
        ui->Plot1->replot();

        //view_ch_2
        mx = obr_buf_f[1][i];
        mn = obr_buf_f[1][i];
        for(int i = 0; i < cnt_block_recv; i++){
           x[i] = i;
           y[i] = obr_buf_f[1][i];
           if(mx<y[i]) mx = y[i];
           if(mn>y[i]) mn = y[i];
        }

        mx_ch[1]=mx;
        min_ch[1]=mn;

        ui->Plot2->graph(0)->setData(x,y);
        ui->Plot2->replot();

        cnt_block_recv = 0;
    }
    else{

    }
  }
  else if(cntrl_sd_ptr->stat[0]==1){
     ui->Button_Start_Stop->setText("Запись");
     ui->label_2->setText("SD активирована!");
     Fl_SD_Ready = true;
  }
  else if(cntrl_sd_ptr->stat[0]==2){
    ui->Button_Start_Stop->setText("Стоп");
    ui->label_2->setText("Запись на SD запущена");
  }
  else if(cntrl_sd_ptr->stat[0]==4){
   ui->Button_Start_Stop->setText("Запись");
   ui->label_2->setText("Запись на SD остановлена");
  }

}



void MainWindow::on_pushButton_4_clicked()
{
 //   ui->listWidget->clear();
}


void MainWindow::on_pushButton_7_clicked()
{
    Fl_0_Before = 1;
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
}

void MainWindow::on_lineEdit_returnPressed()
{
    PorogTah = ui->lineEdit->text().toDouble();
}

void MainWindow::Write_byte_to_serial_port(char cb)
{
  QByteArray data_cdg;

    data_cdg.append(cb);
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

/* !!! DEL
void MainWindow::on_lineEdit_5_returnPressed()
{
    k_norm = ui->lineEdit_5->text().toDouble();
}
*/

void MainWindow::on_lineEdit_6_returnPressed()
{
    double Xm_1;
    double Xm_2;
    Xm_1 = 0;
    Xm_2 = ui->lineEdit_7->text().toDouble();
    N_RCV = (int)Xm_2;
    ui->Plot1->xAxis->setRange(Xm_1, Xm_2);
    ui->Plot1->replot();

}

void MainWindow::on_lineEdit_7_returnPressed()
{

}

void MainWindow::on_pushButton_3_clicked()
{
    double Ym_1;
    double Ym_2;

    if(cnt_recv>0){

        Ym_1 = mx_ch[0]+mx_ch[0]*0.3;
        Ym_2 = min_ch[0]-min_ch[0]*0.3;
        ui->Plot1->yAxis->setRange(Ym_2, Ym_1);
        ui->Plot1->replot();

        ui->lineEdit_3->setText(QString::number(Ym_1));
        ui->lineEdit_4->setText(QString::number(Ym_2));

    }
}

void MainWindow::on_pushButton_8_clicked()
{
        ui->Plot1->xAxis->setRange(0, 2000);
        ui->Plot1->replot();
        ui->lineEdit_6->setText("0");
        ui->lineEdit_7->setText("2000");
}

void MainWindow::on_lineEdit_8_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_8->text().toDouble();
    Ym_2 = ui->lineEdit_9->text().toDouble();
    ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2->replot();
}

void MainWindow::on_lineEdit_9_returnPressed()
{
    double Ym_1;
    double Ym_2;
    Ym_1 = ui->lineEdit_8->text().toDouble();
    Ym_2 = ui->lineEdit_9->text().toDouble();
    ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
    ui->Plot2->replot();
}

void MainWindow::on_lineEdit_10_returnPressed()
{

}

void MainWindow::on_lineEdit_11_returnPressed()
{

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

    if(cnt_recv>0){

        Ym_1 = mx_ch[1]+mx_ch[1]*0.3;
        Ym_2 = min_ch[1]-min_ch[1]*0.3;
        ui->Plot2->yAxis->setRange(Ym_2, Ym_1);
        ui->Plot2->replot();

        ui->lineEdit_8->setText(QString::number(Ym_1));
        ui->lineEdit_9->setText(QString::number(Ym_2));

    }
}

void MainWindow::on_checkBox_clicked()
{

    if(ui->checkBox->isChecked()==true){
        ui->label_2->setText("Запись на SD остановлена!");
        ui->label_2->setStyleSheet("font-weight: bold; font-size: 10pt; color: green;");
        fl_write_to_file = true;
    }else{
        ui->label_2->setText("Зап в файл Откл!");
        ui->label_2->setStyleSheet("font-weight: bold; font-size: 10pt; color: red");
        fl_write_to_file = false;
    }

}

/*!!! DEl
void MainWindow::on_lineEdit_2_returnPressed()
{
    Fd_Inv = 1.0/ui->lineEdit_2->text().toDouble();
}
*/

void MainWindow::on_spinBox_2_valueChanged(const QString &arg1)
{
    TimeAverTah = ui->spinBox_2->text().toDouble();
}

void MainWindow::on_pushButton_2_clicked()
{
   QByteArray data_cdg;
   int i;

   data_cdg.append(RESET_WR_SD);
   writeData(data_cdg);
   data_cdg.clear();
   for(i=0;i<100000;i++){i=i;}

}

void MainWindow::on_lineEdit_14_returnPressed()
{
//    Kusil = ui->lineEdit_14->text().toDouble();
}

void MainWindow::on_lineEdit_15_returnPressed()
{
 //   Msopr = ui->lineEdit_15->text().toDouble();
}

void MainWindow::on_lineEdit_12_returnPressed()
{
 //   KPD_Red = ui->lineEdit_12->text().toDouble()/100.0;
}

void MainWindow::on_lineEdit_13_returnPressed()
{
 //   Per_Otnosh = ui->lineEdit_13->text().toDouble()/100.0;
}

void MainWindow::on_lineEdit_13_editingFinished()
{
 //   Per_Otnosh = ui->lineEdit_13->text().toDouble()/100.0;
}

void MainWindow::on_lineEdit_12_editingFinished()
{
 //   KPD_Red = ui->lineEdit_12->text().toDouble()/100.0;
}

void MainWindow::on_lineEdit_15_editingFinished()
{
//   Msopr = ui->lineEdit_15->text().toDouble();
}

void MainWindow::on_lineEdit_14_editingFinished()
{
//  Kusil = ui->lineEdit_14->text().toDouble();
}

void MainWindow::on_lineEdit_editingFinished()
{
  // PorogTah = ui->lineEdit->text().toDouble();
}

/*!!! DEL
void MainWindow::on_lineEdit_2_editingFinished()
{
   Fd_Inv = 1.0/ui->lineEdit_2->text().toDouble();
}
*/

/* !!! DEL
void MainWindow::on_lineEdit_5_editingFinished()
{
   k_norm = ui->lineEdit_5->text().toDouble();
}
*/

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{

}


void MainWindow::closeEvent(QCloseEvent *event)
{
QString fileName = QCoreApplication::applicationDirPath()+"/settings.ini";

    QSettings settings(fileName,QSettings::IniFormat);

    settings.setValue("SETTINGS/COM_PORT",ui->PortNameEdit->text());
    settings.setValue("SETTINGS/porog",ui->lineEdit->text());
    settings.setValue("SETTINGS/time_mean",ui->spinBox_2->value());
    settings.setValue("SETTINGS/koef_usil",ui->lineEdit_14->text());
    settings.setValue("SETTINGS/moment_sopr",ui->lineEdit_15->text());
    settings.setValue("SETTINGS/kpd_reduktora",ui->lineEdit_12->text());
    settings.setValue("SETTINGS/pered_otnoshenie",ui->lineEdit_13->text());
//    settings.setValue("SETTINGS/Fd",ui->lineEdit_2->text()); !!! DEL
//    settings.setValue("SETTINGS/1_div_Volts",ui->lineEdit_5->text());  !!! DEL

    settings.sync(); //записываем настройки

}


void MainWindow::on_groupBox_clicked()
{

}
