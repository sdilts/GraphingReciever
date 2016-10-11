#include "reciever.h"
#include <QDebug>
#include <QTimer>
#include <QVector>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/*
 * sigs are the characters that control the board:
 * 0. Send more data
 * 1. Reset ticker
 * 2. Undefined
 */
//TODO: change to individual variables:
const char Reciever::sigs[] = {17, 18, 19};
const QVector<int> Reciever::inputs = {4, 2, 2};
const int Reciever::bufferSize = 8;

Reciever::Reciever(QObject *parent) : QObject(parent)
{
    done = true;
    //init objects
    port = new QSerialPort(this);
    timer=  new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(record()));
    connect(port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
}

Reciever::~Reciever() {
    //free up data
    delete timer;
    delete port;
}

QVector< QVector <QString> > Reciever::updateSerialPorts() {
    const auto infos = QSerialPortInfo::availablePorts();

    QVector< QVector <QString> > ports(infos.length(), QVector<QString>(2));

    for (int i = 0; i < infos.length(); i++) {
        QSerialPortInfo info = infos[i];
        ports[i][0] = info.portName();
        ports[i][1] = info.description();
    }
    return ports;
}

bool Reciever::setSerial(QString newPort) {
    if(done) {
        port->setPortName(newPort);
        qInfo() << "Using port" << newPort;
        return true;
    } else return false;
}

void Reciever::sendCollectDataSignal() {
     port->write(&sigs[0],1);
}

void Reciever::record() {
    //qDebug() << port->bytesAvailable();
    if(port->bytesAvailable() >= bufferSize) {
        char buff[bufferSize];
        port->read(buff, bufferSize);
        qDebug() << "New Data";
        QVector<unsigned int > data(inputs.length());
        for(int i = 0, j = 0; i < inputs.length(); i += 1) {
            data[i] = getNumber((unsigned char*) (buff +j), inputs[i]);
            qDebug("%x", data[i]);
            j += inputs[i];
        }
        emit dataRecieved(data);
    }
    if(!done) {
        sendCollectDataSignal();
    }
}

unsigned int Reciever::getNumber(unsigned char buff[], int size) {
    unsigned int result = 0;
    //qDebug("%x", result);
    for(int i = 0; i < size; i++) {
        int number = (int) buff[i];
        result += number << (i *8);
    }
    return result;
}

unsigned int Reciever::readBytes(int numBytes) {
    if(port->isOpen()) {
        char* buff = new char[numBytes];
        port->read(buff, numBytes);
        unsigned int result = 0;
        for(int i = 0; i < numBytes; i++) {
            result = result | buff[i] << (i * 8);
        }
        delete buff;
        return result;
    } else {
        emit error("Error: port is not open");
        return -1;
    }
}

void Reciever::startRecording(QString portName, int interval) {
    //will not run if done = false (side effect?)
    if(setSerial(portName)) {
        done = false;
        //open serial port:
        if(port->open(QIODevice::ReadWrite)) {
            //reset the ticker:
            port->write(&sigs[1],1);
            //start signal chain
            this->sendCollectDataSignal();
            timer->start(interval);
        }
    }
}

void Reciever::stopRecording(bool error) {
    timer->stop();
    done = true;
    //collect anything that might be in the buffer:
    if(!error) {
        this->record();
    }
    if(port->isOpen()) {
        qInfo() << "Closing Serial Port";
        port->close();
    }
}

void Reciever::handleSerialError(QSerialPort::SerialPortError error) {
    if(error != QSerialPort::NoError) {
        //stop using the serial port:
        stopRecording(true);
        //emit the correct error message:
        switch(error) {
        case QSerialPort::NoError:
            break;
        case QSerialPort::PermissionError:
            emit Reciever::error("Don't have permission to open this serial port");
            break;
        case QSerialPort::OpenError:
            emit Reciever::error("The serial port is already open");
                break;
        case QSerialPort::NotOpenError:
            emit Reciever::error("The serial port is not open");
            break;
        case QSerialPort::WriteError:
            emit Reciever::error("An I/O read error occured on the serial port");
            break;
        case QSerialPort::ReadError:
            emit Reciever::error("An I/O write error occured on the serial port");
            break;
        case QSerialPort::ResourceError:
            emit Reciever::error("Resource has become unavailable on the serial port");
            break;
        case QSerialPort::TimeoutError:
            emit Reciever::error("The Serial port has timed out.");
            break;
        default:
            emit Reciever::error("An unknown error occured in the serial port.");
        }
    }
}
