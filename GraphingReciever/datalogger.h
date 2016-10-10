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
    void setupGraphs();
    void configGraphs();
    void updateSerialPortData();
    void enableSerialConfig();
    void disableSerialConfig();

public:
    explicit DataLogger(QWidget *parent = 0);
    ~DataLogger();

public slots:
    void graphData(QVector<unsigned int> data);
    void appError(QString message);

private slots:
    void on_recordButton_toggled(bool checked);

    void on_actionUpdate_Serial_Ports_triggered();

signals:
    void startRecording();
    void stopRecording();
    
private:
    Ui::DataLogger *ui;
};

#endif // DATALOGGER_H
