#ifndef RECIEVER_H
#define RECIEVER_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QHash>
#include <QVector>
#include <QString>


class Reciever : public QObject
{
    Q_OBJECT
public:
    //const static qint sigs[3];

    explicit Reciever(QObject *parent = 0);
    ~Reciever();

   QVector< QVector <QString> > updateSerialPorts();

    void startRecording(QString portName, int interval);
    void stopRecording(bool error);

private:
    bool done; //used to help control the state of the program
    const static char sigs[3]; //stores signals used to communicate with the arduino
    const static QVector<int> inputs; //stores the number bytes in input: see readme.txt
    const static int bufferSize;

    QSerialPort* port;
    QTimer * timer;


    bool setSerial(QString portName);
    void sendCollectDataSignal();
    unsigned int readBytes(int numBytes);
    unsigned int getNumber(unsigned char buff[], int size);

signals:
    void dataRecieved(QVector<unsigned int> data);
    void error(QString message);

public slots:
    void record();

private slots:
    void handleSerialError(QSerialPort::SerialPortError error);
};

#endif // RECIEVER_H
