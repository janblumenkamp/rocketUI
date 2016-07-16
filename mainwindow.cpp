#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMessageBox>
#include <QLabel>
#include <QSerialPortInfo>
#include <QTime>
#include <algorithm>

#include "flightdatamodel.h"
#include "RocketComm_Defs.h"

extern "C" {
    #include "EMBcomm/HAL_Memory.h"
    #include "EMBcomm/HAL_Serial.h"
    #include "EMBcomm/Memory.h"
    #include "EMBcomm/Comm.h"
    #include "EMBcomm/PackageMemory.h"
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    sinterface = new SerialInterface();

    time_since_start.start();

    time_since_launch.setHMS(0, 0, 0);

    timer_launch = new QTimer();
    connect(timer_launch, SIGNAL(timeout()), this, SLOT(timer_launch_event()));

    HAL_Memory_Init();
    HAL_Serial_InitWriteString((void (*)(void *, int8_t *, uint16_t))SerialInterface::static_writeString, sinterface);
    Memory_Init(&m_in, mementr_in, MEMORY_IN_START, MEMORY_IN_LENGTH, MEMORY_INOUT_ALIGN);
    Memory_Init(&m_out, mementr_out, MEMORY_OUT_START, MEMORY_OUT_LENGTH, MEMORY_INOUT_ALIGN);
    Comm_Init(&comm, &m_in, &m_out, (uint32_t (*)(void *))MainWindow::getTimeMS, this);

    ui->plot_flight->addGraph();
    /*ui->plot_flight->graph(0)->setData(height_time, height_history);
    ui->plot_flight->xAxis->setLabel("t in s");
    ui->plot_flight->yAxis->setLabel("Height in m");
    //ui->plot_flight->setBackground(Qt::transparent);
    //ui->plot_flight->setAttribute(Qt::WA_OpaquePaintEvent, false);*/

    flightdatamodel = new FlightdataModel(0);
    ui->tbl_flightdata->setModel(flightdatamodel);

    msgtblmdl_in = new MSGTableModel(0, comm.m_in, &comm);
    ui->tbl_msgin->setModel(msgtblmdl_in);
    ui->tbl_msgin->verticalHeader()->hide();

    msgtblmdl_out = new MSGTableModel(0, comm.m_out, &comm);
    ui->tbl_msgout->setModel(msgtblmdl_out);
    ui->tbl_msgout->verticalHeader()->hide();

    ui->lcd_timer->display(time_since_launch.toString(TIMER_LAUNCH_DISPLAY_FORMAT));

    ui->lbl_flightstate->setText("Please calibrate");

    // Buttons
    connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
    connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(serialOpenPort())); //Connect Button (Liste der seriellen Ports)
    connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(serialClosePort())); //Disconnect Button

    connect(ui->btn_calibrate, SIGNAL(clicked()), this, SLOT(calibrate()));
    connect(ui->btn_launch, SIGNAL(clicked()), this, SLOT(launch()));

    connect(sinterface, SIGNAL(receivedByte(int8_t)), this, SLOT(receivedByte(int8_t)));
}

MainWindow::~MainWindow() {
    delete sinterface;
    delete ui;
    delete flightdatamodel;
    delete msgtblmdl_in;
    delete msgtblmdl_out;
    delete timer_launch;
}

void MainWindow::refreshPortList() {
    ui->cmb_serialPorts->clear();
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
       ui->cmb_serialPorts->addItem(port.portName());
    }
}

void MainWindow::serialOpenPort() {
    if(ui->cmb_serialPorts->currentText() != "") {
        if(sinterface->openPort("/dev/" + ui->cmb_serialPorts->currentText())) {
            ui->lbl_commstate->setText("Connected");
            ui->btn_connect->setEnabled(false);
            ui->btn_disconnect->setEnabled(true);
        } else {
            ui->lbl_commstate->setText("Failed");
        }
    }
}

void MainWindow::calibrate() {
    Comm_Package_t p;
    p.length = 0;
    p.type = PACKAGE_TYPE_CMD;
    p.reg = PACKAGE_CMD_CALIBRATE;
    msgtblmdl_out->send(&p);
    Comm_SendAllPackages(&comm);

    pack_id_calibrate = p.id;

    ui->lbl_flightstate->setText("Calibration initiated");
    ui->btn_calibrate->setEnabled(false);
    ui->btn_launch->setEnabled(true);
}

void MainWindow::launch() {
    if(1){//PackageMemory_GetMemID(comm.m_out, pack_id_calibrate) < 0) {
        // Message not in memory anymore, that means the ack was received.

        Comm_Package_t p;
        p.length = 0;
        p.type = PACKAGE_TYPE_CMD;
        p.reg = PACKAGE_CMD_START;
        msgtblmdl_out->send(&p);
        Comm_SendAllPackages(&comm);

        ui->lbl_flightstate->setText("Sent launch command");
        timer_launch->start(TIMER_LAUNCH_INCREMENT_MS);
        ui->btn_launch->setEnabled(false);
    } else {
        ui->lbl_flightstate->setText("Waiting for calibration ack");
    }
}

void MainWindow::timer_launch_event() {
    time_since_launch = time_since_launch.addMSecs(timer_launch->interval());
    ui->lcd_timer->display(time_since_launch.toString(TIMER_LAUNCH_DISPLAY_FORMAT));
}

void MainWindow::serialClosePort() {
    sinterface->closePort();
    ui->btn_connect->setEnabled(true);
    ui->btn_disconnect->setEnabled(false);
    ui->lbl_commstate->setText("Not connected");
}

uint32_t MainWindow::getTimeMS(MainWindow *w) {
    return w->time_since_start.elapsed();
}

/*
 * Neues Paket von der Comm Klasse empfangen
 *
 */
void MainWindow::receivedByte(int8_t byte) {
    qDebug() << "rec b " << byte;
    Comm_Parser(&comm, byte);
}

/*
 * Timer für die Aktualisierung des Graphen abgelaufen:
 * Neu zeichnen
 *
 */
void MainWindow::updateTempGraph(void) {
    /*if(sinterface->isConnected())
    {
        using namespace std;
        double height_max = *max_element(height_history.begin(), height_history.end());
        double height_min = *min_element(height_history.begin(), height_history.end());

        height_history.push_front(height);
        height_time.push_front(0);

        ui->plot_flight->graph(0)->setData(height_time, height_history);
        ui->plot_flight->yAxis2->setRange(height_min - 2, height_max + 2); // yAxis 2 ist die rechte Y-Achse. yAxis 1 ist deaktiviert, aber alle Werte bezeiehn sich auf diese Achse, wewegen bei beiden die Range eingestellt werden muss
        ui->plot_flight->yAxis->setRange(height_min - 2, height_max + 2);
        ui->plot_flight->replot();
    }*/
}
