#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct Settings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class Port : public QObject
{
    Q_OBJECT

public:

    explicit Port(QObject *parent = 0);

    ~Port();

    QSerialPort thisPort;

    Settings SettingsPort;

    void set_num_data_recv(unsigned n){ num_data_recv = n; }


signals:

    void finished_Port(); //

    void error_(QString err);

    void outPort(QByteArray data);

public slots:

    void  DisconnectPort(void);

    void ConnectPort(void);

    void Write_Settings_Port(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);

    void process_Port(void);

    void WriteToPort(QByteArray data);

    void ReadInPort(void);

    void SetRunRecv(bool Recv_En);
    void SetRunReg(bool Reg_En);

    void ClearBuf1(void);

private slots:

    void handleError(QSerialPort::SerialPortError error);//

public:

private:

    bool RecvEn = false;
    bool RegEn = false;
    bool num_buf = false;
    unsigned cnt_read_block = 0;
    unsigned cnt_loop_block = 0;
    unsigned num_data_recv = 1000;

    QByteArray buf1;
    QByteArray buf2;

};

#endif // PORT_H
