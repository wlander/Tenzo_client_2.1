#include "port.h"
#include <qdebug.h>
#include <QThread>
//-----------------------------------------------------------------------------------------

Port::Port(QObject *parent) :
    QObject(parent)
{
    RecvEn = true;
}

//-----------------------------------------------------------------------------------------

Port::~Port()
{
    qDebug("By in Serial Port Thread !");
    emit finished_Port();
}

//-----------------------------------------------------------------------------------------

void Port :: process_Port(void){
    qDebug("Hello World in Serial Port Thread!");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

//-----------------------------------------------------------------------------------------

void Port :: Write_Settings_Port(QString name, int baudrate,int DataBits,
                         int Parity,int StopBits, int FlowControl){
    SettingsPort.name = name;
    SettingsPort.baudRate = (QSerialPort::BaudRate) baudrate;
    SettingsPort.dataBits = (QSerialPort::DataBits) DataBits;
    SettingsPort.parity = (QSerialPort::Parity) Parity;
    SettingsPort.stopBits = (QSerialPort::StopBits) StopBits;
    SettingsPort.flowControl = (QSerialPort::FlowControl) FlowControl;

}

//-----------------------------------------------------------------------------------------

void Port :: ConnectPort(void){//
    thisPort.setPortName(SettingsPort.name);
    if (thisPort.open(QIODevice::ReadWrite)) {
        if (thisPort.setBaudRate(SettingsPort.baudRate)
                && thisPort.setDataBits(SettingsPort.dataBits)//DataBits
                && thisPort.setParity(SettingsPort.parity)
                && thisPort.setStopBits(SettingsPort.stopBits)
                && thisPort.setFlowControl(SettingsPort.flowControl))
        {
            if (thisPort.isOpen()){
                error_((SettingsPort.name+ " >> Opened!\r").toLocal8Bit());
            }
        } else {
            thisPort.close();
            error_(thisPort.errorString().toLocal8Bit());
          }
        thisPort.setReadBufferSize(8000); //!!! *2
    } else {
        thisPort.close();
        error_(thisPort.errorString().toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------------------

void Port::handleError(QSerialPort::SerialPortError error)//
{
    if ( (thisPort.isOpen()) && (error == QSerialPort::ResourceError)) {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}//

//-----------------------------------------------------------------------------------------

void  Port::DisconnectPort(void){
    if(thisPort.isOpen()){
        thisPort.close();
        error_(SettingsPort.name.toLocal8Bit() + " >> Closed!\r");
    }
}

//-----------------------------------------------------------------------------------------

void Port :: WriteToPort(QByteArray data){
    if(thisPort.isOpen()){
        thisPort.write(data);
    }
}

//-----------------------------------------------------------------------------------------

void Port :: ReadInPort(void){
   unsigned Cnt_Byte = 0;
   unsigned Cnt_Block = 0;

   buf1.append(thisPort.readAll());
   Cnt_Byte = buf1.count();
   Cnt_Block = Cnt_Byte/(num_data_recv);

   if(cnt_read_block<Cnt_Block){
       cnt_read_block++;
       cnt_loop_block++;
   }

   if(cnt_loop_block==1){
       if(RecvEn){
         buf2.clear();
         buf2.append(buf1.data()+(cnt_read_block-1)*num_data_recv, num_data_recv);
         outPort(buf2);
       }
       else{
          qDebug("Serial port buffer is busy!");
       }
       cnt_loop_block = 0;
   }

}

//----------------------------------------------------------------------------------------

void Port :: SetRunRecv(bool Recv_En){
   RecvEn = Recv_En;
}
//----------------------------------------------------------------------------------------
void Port :: SetRunReg(bool Reg_En){
   RegEn = Reg_En;
}
//----------------------------------------------------------------------------------------
void Port :: ClearBuf1(void){
   //reg_buf.clear();
}
//----------------------------------------------------------------------------------------
