#include "myudp.h"
#include <QMessageBox>
#include <QString>

//If you want to broadcast something you send it to your broadcast address
//ex. 192.2.1.255

QString Set_IP_write = "192.168.4.1";
quint16 Set_Port_read = 8888;
quint16 Set_Port_write = 1112;

MyUDP::MyUDP(QObject *parent) : 
    QObject(parent)
{
  socket = new QUdpSocket(this);
  
  //We need to bind the UDP socket to an address and a port
  socket->bind( QHostAddress::Any, Set_Port_read);         //
  connect(socket,SIGNAL(readyRead()),this,SLOT(UDPRead()));
  qDebug()<<"Hello MyUDP! " << "Any_IP" << ", " << Set_Port_read;

}

MyUDP::~MyUDP()
{
    qDebug("By in Thread_UDP!");
}

void MyUDP::SayHello()      //Just spit out some data
{
  QByteArray Data;
  Data.append("Hello\r\n");
  
  socket->writeDatagram(Data,QHostAddress(Set_IP_write), Set_Port_write);
  qDebug("SayHello");
}


void MyUDP::UDPWrite(QByteArray Data)      //Just spit out some data
{
  socket->writeDatagram(Data,QHostAddress(Set_IP_write), Set_Port_write);
  qDebug("UDPWrite");
}

void MyUDP::UDPRead()     //Read something
{
  QByteArray Buffer;
  QString str;
  Buffer.resize(socket->pendingDatagramSize());
  QHostAddress sender = QHostAddress::Any;
  quint16 senderPort = Set_Port_read;
  socket->readDatagram(Buffer.data(),Buffer.size(),&sender,&senderPort);
  
  outUDP(Buffer);

  qDebug("data recieve!");
  //qDebug()<<"data: "<<Buffer.data();

}
