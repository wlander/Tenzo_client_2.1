#ifndef MYUDP_H
#define MYUDP_H

#include <QUdpSocket>
 
class MyUDP : public QObject
{
  Q_OBJECT
 
  public:

      explicit MyUDP(QObject *parent = 0);
      ~MyUDP();

      QUdpSocket *socket;
 
  signals:
      void outUDP(QByteArray data);
  public slots:
      void UDPRead();
      void UDPWrite(QByteArray data);
      void SayHello();
 
};

#endif
