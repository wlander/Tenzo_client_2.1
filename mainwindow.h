#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "port.h"
#include "myudp.h"
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

signals:

    void savesettings(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    void Connect_Port();
    void Disconnect_Port();
    void writeData(QByteArray data);
    void ReadData(QByteArray data);
    void Signal_SetRunRecv(bool Recv_En);
    void Signal_SetRunReg(bool Reg_En);
    void Clear_Buf_Data(void);
    void SayHelloUDP();

private slots:

    void on_Button_Connect_clicked();
    void on_Button_Start_Stop_clicked();
    void on_pushButton_5_clicked();
    void ReadFromPort(QByteArray);

    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_lineEdit_returnPressed();
    void on_lineEdit_3_returnPressed();
    void on_lineEdit_4_returnPressed();
    void on_lineEdit_6_returnPressed();
    void on_lineEdit_7_returnPressed();
    void on_pushButton_3_clicked();
    void on_pushButton_8_clicked();
    void on_lineEdit_8_returnPressed();
    void on_lineEdit_9_returnPressed();
    void on_lineEdit_10_returnPressed();
    void on_lineEdit_11_returnPressed();
    void on_pushButton_9_clicked();
    void on_pushButton_2_clicked();
    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    void Write_byte_to_serial_port(char cb);
    void  Write_byte_to_esp(char cb);
    int write_in_file(char *the_data, unsigned int the_size);
};

#endif // MAINWINDOW_H
