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
    ui(new Ui::MainWindow)
{
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
    timer_start.start();

    HAL_Memory_Init();
    HAL_Serial_InitWriteString((void (*)(void *, int8_t *, uint16_t))SerialInterface::static_writeString, this);
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

    msgtblmdl_in = new MSGTableModel(0);
    ui->tbl_msgin->setModel(msgtblmdl_in);
    msgtblmdl_out = new MSGTableModel(0);
    ui->tbl_msgout->setModel(msgtblmdl_out);

    flightdatamodel->addData(PACKAGE_DATA_ACC_X, 1.5);
    flightdatamodel->addData(PACKAGE_DATA_ACC_X, 0.5);

    Comm_Package_t pack;
    pack.id = 1234;
    pack.length = 12;
    pack.type = PACKAGE_TYPE_CMD;
    pack.reg = PACKAGE_CMD_CALIBRATE;
    msgtblmdl_in->addData(&pack);

    pack.id = 2345;
    pack.length = 33;
    pack.type = PACKAGE_TYPE_STATUS;
    pack.reg = PACKAGE_STATUS_IDLE;
    msgtblmdl_in->addData(&pack);

    ui->lcd_timer->display(QString("T-00:00:000"));
   /* QStringList tableHeader_data;
    tableHeader_data << "Item" << "Is" << "Max";
    ui->tbl_data->setColumnCount(tableHeader_data.count());
    ui->tbl_data->setHorizontalHeaderLabels(tableHeader_data);

    QStringList tableHeader_msg;
    tableHeader_msg << "ID" << "Type" << "Reg" << "Length";
    ui->tbl_msgin->setColumnCount(tableHeader_msg.count());
    ui->tbl_msgin->setHorizontalHeaderLabels(tableHeader_msg);
    ui->tbl_msgout->setColumnCount(tableHeader_msg.count());
    ui->tbl_msgout->setHorizontalHeaderLabels(tableHeader_msg);*/

	// Buttons
	connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
    connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(serialOpenPort())); //Connect Button (Liste der seriellen Ports)
    connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(serialClosePort())); //Disconnect Button

    connect(ui->btn_calibrate, SIGNAL(clicked()), this, SLOT(calibrate()));

    connect(sinterface, SIGNAL(receivedByte(int8_t)), this, SLOT(receivedByte(int8_t)));
}

MainWindow::~MainWindow()
{
    delete sinterface;
	delete ui;
    delete flightdatamodel;
    delete msgtblmdl_in;
    delete msgtblmdl_out;
}

void MainWindow::refreshPortList()
{
	ui->cmb_serialPorts->clear();
	Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
	{
	   ui->cmb_serialPorts->addItem(port.portName());
	}

    /////// ONLY FOR DEBUGGING PURPOSES - VIRTUAL SERIAL PORT!
    ui->cmb_serialPorts->addItem("ttypts0");
}

void MainWindow::serialOpenPort()
{
	if(ui->cmb_serialPorts->currentText() != "")
	{
        if(sinterface->openPort("/dev/" + ui->cmb_serialPorts->currentText())) // Vebindung konnte erfolgeich hergestellt werden
		{
            ui->lbl_current_state->setText("Connected");
			ui->btn_connect->setEnabled(false);
			ui->btn_disconnect->setEnabled(true);
		}
		else
		{
			ui->lbl_current_state->setText("Failed");
		}
	}
}

void MainWindow::calibrate()
{
    Comm_Package_t p;
    p.id = 1111;
    p.length = 0;
    p.type = PACKAGE_TYPE_CMD;
    p.reg = PACKAGE_CMD_CALIBRATE;
    PackageMemory_ToMemory(comm.m_out, &p);
    Comm_SendAllPackages(&comm);
}

void MainWindow::serialClosePort()
{
    sinterface->closePort();
	ui->btn_connect->setEnabled(true);
	ui->btn_disconnect->setEnabled(false);
	ui->lbl_current_state->setText("Not connected");
}

uint32_t MainWindow::getTimeMS(MainWindow *w) {
    return w->timer_start.elapsed();
}

/*
 * Neues Paket von der Comm Klasse empfangen
 *
 */
void MainWindow::receivedByte(int8_t byte)
{
    qDebug() << "rec b " << byte;
    Comm_Parser(&comm, byte);
}

/*
 * Timer für die Aktualisierung des Graphen abgelaufen:
 * Neu zeichnen
 *
 */
void MainWindow::updateTempGraph(void)
{
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
