#include "datalogger.h"
#include "ui_datalogger.h"
#include "reciever.h"
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <QMessageBox>


//how far apart each data point is
#define LISTEN_INTERVAL 250
//range of display on the graph:
#define UPDATE_INTERVAL 10

DataLogger::DataLogger(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataLogger)
{
    ui->setupUi(this);
    rec = new Reciever(this);

    connect(rec, SIGNAL(dataRecieved(QVector <unsigned int>)), this, SLOT(graphData(QVector <unsigned int>)));
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
void DataLogger::graphData(QVector<unsigned int> data) {
    QCustomPlot* plot = ui->graphWidget;
    plot->graph(0)->addData(data[0], (int) data[1]);
    plot->graph(1)->addData(data[0], (int) data[2]);
    plot->xAxis->setRange(data[0], UPDATE_INTERVAL, Qt::AlignCenter);
    //delete data;
    plot->replot();
}

void DataLogger::on_recordButton_toggled(bool checked)
{
     if(checked) {
        disableSerialConfig();
         if(!first) {
             ui->graphWidget->clearGraphs();
             configGraphs();
         } else first = false;

         rec->startRecording(ui->serialSelect->currentText(), ui->intervalSelect->value());

     } else {
         rec->stopRecording(false);
         enableSerialConfig();
         ui->graphWidget->rescaleAxes();
         ui->graphWidget->replot();
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
    QCustomPlot* plot = ui->graphWidget;
    plot->legend->setVisible(true);
    plot->yAxis->setRange(0, 100);
    plot->xAxis->setRange(0, UPDATE_INTERVAL, Qt::AlignCenter);
    configGraphs();

}

void DataLogger::configGraphs() {
    QCPScatterStyle myScatter;
    myScatter.setShape(QCPScatterStyle::ssCircle);
    myScatter.setPen(QPen(Qt::blue));
    myScatter.setBrush(Qt::white);
    myScatter.setSize(5);

    QCustomPlot* plot = ui->graphWidget;
    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setName("pressure");
    plot->graph(0)->setScatterStyle(myScatter);

    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(1)->setName("Salinity");
    plot->graph(1)->setScatterStyle(myScatter);
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
