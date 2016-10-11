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
    bool done;
    QSerialPort* port;
    QTimer * timer;
    const static char sigs[3];
    //const static QVector<const int> inputs;

    bool setSerial(QString portName);
    void sendCollectDataSignal();
    unsigned int readBytes(int numBytes);

signals:
    void dataRecieved(QVector<unsigned int> data);
    void error(QString message);

public slots:
    void record();

private slots:
    void handleSerialError(QSerialPort::SerialPortError error);
};

#endif // RECIEVER_H
