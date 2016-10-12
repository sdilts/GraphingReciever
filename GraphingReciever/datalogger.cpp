#include "datalogger.h"
#include "ui_datalogger.h"
#include "reciever.h"
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <QMessageBox>


//range of display on the graph:
#define UPDATE_INTERVAL 50

DataLogger::DataLogger(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataLogger)
{
    ui->setupUi(this);
    rec = new Reciever(this);
    QString fileLocation = QDir::homePath();
    qDebug() << fileLocation;

    //connect(rec, SIGNAL(dataRecieved(QVector<uint>)), this, SLOT(graphData(QVector<uint>)));
    connect(rec, SIGNAL(dataRecieved(QVector <unsigned int>)), this, SLOT(processData(QVector <unsigned int>)));
    connect(rec, &Reciever::error, this, &DataLogger::appError);
    setupGraphs();
    updateSerialPortData();

    first = true;
}

DataLogger::~DataLogger()
{
    delete ui;
    delete rec;
}

//slots:
void DataLogger::processData(QVector<unsigned int> data) {
    unsigned int time = data.at(0) / 100;
    //convert to voltages:
    double salinity = (int) data.at(1) * (5.0 / 1023.0);
    double pressure = (int) data.at(2) * (5.0 / 1032.0);
    graphSalinity(time, salinity);
    graphPressure(time, pressure);
    emit saveData(time, salinity, pressure);
}

void DataLogger::graphSalinity(unsigned int time, double data) {
    QCustomPlot* plot = ui->salinityGraph;
    plot->graph(0)->addData(time, data);
    plot->xAxis->setRange(time, UPDATE_INTERVAL, Qt::AlignCenter);
    plot->replot();
}

void DataLogger::graphPressure(unsigned int time, double data) {
    QCustomPlot* plot = ui->pressureGraph;
    plot->graph(0)->addData(time, data);
    plot->xAxis->setRange(time, UPDATE_INTERVAL, Qt::AlignCenter);
    plot->replot();
}

//void DataLogger::recordData(QVector<unsigned int> data) {

//}

void DataLogger::on_recordButton_toggled(bool checked)
{
     if(checked) {
        disableSerialConfig();
         if(!first) {
             ui->pressureGraph->clearGraphs();
             ui->salinityGraph->clearGraphs();
             configGraphs();
         } else first = false;

         rec->startRecording(ui->serialSelect->currentText(), ui->intervalSelect->value());

     } else {
         rec->stopRecording(false);
         enableSerialConfig();
         ui->pressureGraph->rescaleAxes();
         ui->pressureGraph->replot();
         ui->salinityGraph->rescaleAxes();
         ui->salinityGraph->replot();
     }
}

void DataLogger::on_actionUpdate_Serial_Ports_triggered()
{
   updateSerialPortData();
}

void DataLogger::appError(QString message) {
    //reset the GUI:
    ui->recordButton->blockSignals(true);
    ui->recordButton->setChecked(false);
    ui->recordButton->blockSignals(false);
    enableSerialConfig();
    //QMessageBox::StandardButton rply;
    QMessageBox::critical(this, tr("ERROR:"),
                                    message,
                                    QMessageBox::Abort);

}

void DataLogger::disableSerialConfig() {
    ui->serialFrame->setDisabled(true);
    ui->menuMenu->setDisabled(true);
}

void DataLogger::enableSerialConfig() {
    ui->serialFrame->setDisabled(false);
    ui->menuMenu->setDisabled(false);

}

//setup functions
void DataLogger::setupGraphs() {
    QCustomPlot* plot = ui->pressureGraph;
    plot->legend->setVisible(true);
    plot->yAxis->setRange(0, 5);
    plot->xAxis->setRange(0, UPDATE_INTERVAL, Qt::AlignCenter);

    plot = ui->salinityGraph;
    plot->legend->setVisible(true);
    plot->yAxis->setRange(0, 5);
    plot->xAxis->setRange(0, UPDATE_INTERVAL, Qt::AlignCenter);

    configGraphs();

}

void DataLogger::configGraphs() {
    QCPScatterStyle myScatter;
    myScatter.setShape(QCPScatterStyle::ssCircle);
    myScatter.setPen(QPen(Qt::blue));
    myScatter.setBrush(Qt::white);
    myScatter.setSize(5);

    QCustomPlot * plot = ui->pressureGraph;
    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setName("pressure");
    plot->graph(0)->setScatterStyle(myScatter);
    plot->yAxis->setRange(0, 5);

    plot = ui->salinityGraph;
    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::red));
    plot->graph(0)->setName("Salinity");
    plot->graph(0)->setScatterStyle(myScatter);
    plot->yAxis->setRange(0, 5);
}

void DataLogger::updateSerialPortData() {
    ui->serialSelect->clear();
    QVector< QVector <QString> > newList = rec->updateSerialPorts();
    for(int i = 0; i < newList.length(); i++) {
        ui->serialSelect->insertItem(i, newList[i][0]);
        ui ->serialSelect->setItemData(i, newList[i][1]);
    }
    qInfo() << "Serial ports updated";
}

//void DataLogger::on_actionChoose_Save_Directory_triggered()
//{
//    fileLocation = QFileDialog::getSaveFileName(this, tr("SaveLocation"),
//                                                    fileLocation,
//                                                    tr("*.txt;;*.csv"));
//}
