#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "reciever.h"

namespace Ui {
class DataLogger;
}

class DataLogger : public QMainWindow
{
    Q_OBJECT

private:
    Reciever* rec;
    bool first;
    QString fileLocation;
    void setupGraphs();
    void configGraphs();
    void updateSerialPortData();
    void enableSerialConfig();
    void disableSerialConfig();
    void graphSalinity(unsigned int time, double data);
    void graphPressure(unsigned int time, double data);

public:
    explicit DataLogger(QWidget *parent = 0);
    ~DataLogger();

public slots:
    void processData(QVector<unsigned int> data);
    void appError(QString message);

private slots:
    void on_recordButton_toggled(bool checked);

    void on_actionUpdate_Serial_Ports_triggered();

    //void on_actionChoose_Save_Directory_triggered();

signals:
    void startRecording();
    void stopRecording();
    void saveData(unsigned int time, int data1, int data2);
    
private:
    Ui::DataLogger *ui;
};

#endif // DATALOGGER_H
