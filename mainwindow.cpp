#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMessageBox>
#include <QLabel>
#include <QSerialPortInfo>
#include <qmath.h>
#include <QTimer>
#include <vector>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    comm = new Comm();

	// Timer zum periodischen aktualisieren des Graphen
    height_history = QVector<double>(100);
    height_time = QVector<double>(100);
    for(int i = 0; i < 100; i++)
	{
        height_time[i] = -i;
	}
    ui->plot_flight->addGraph();
    ui->plot_flight->graph(0)->setData(height_time, height_history);
    ui->plot_flight->xAxis->setLabel("t");
    ui->plot_flight->xAxis->setRange(-100, 0);
    ui->plot_flight->yAxis->setVisible(false); // Die linke Y-Achse soll deaktiviert werden und die recht aktiviert werden (damit die aktuelle Temperatur direkt abgelesen werden kann)
    ui->plot_flight->yAxis2->setVisible(true);
    ui->plot_flight->yAxis2->setLabel("Temperatur in" + QString::fromUtf8("°") + "C");
    //ui->plot_flight->setBackground(Qt::transparent);
    //ui->plot_flight->setAttribute(Qt::WA_OpaquePaintEvent, false);

    QStringList tableHeader_data;
    tableHeader_data << "Item" << "Is" << "Max";
    ui->tbl_data->setColumnCount(tableHeader_data.count());
    ui->tbl_data->setHorizontalHeaderLabels(tableHeader_data);

    QStringList tableHeader_msg;
    tableHeader_msg << "ID" << "Type" << "Reg" << "Length";
    ui->tbl_msgin->setColumnCount(tableHeader_msg.count());
    ui->tbl_msgin->setHorizontalHeaderLabels(tableHeader_msg);
    ui->tbl_msgout->setColumnCount(tableHeader_msg.count());
    ui->tbl_msgout->setHorizontalHeaderLabels(tableHeader_msg);


  /*  // Verbinde Spinfelder mit Slider:
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), ui->sld_led0, SLOT(setValue(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), ui->sld_led1, SLOT(setValue(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), ui->sld_led2, SLOT(setValue(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), ui->sld_led3, SLOT(setValue(int)));

	 // Verbinde Slider mit Spinfelder:
	connect(ui->sld_led0, SIGNAL(valueChanged(int)), ui->spn_led0, SLOT(setValue(int)));
	connect(ui->sld_led1, SIGNAL(valueChanged(int)), ui->spn_led1, SLOT(setValue(int)));
	connect(ui->sld_led2, SIGNAL(valueChanged(int)), ui->spn_led2, SLOT(setValue(int)));
	connect(ui->sld_led3, SIGNAL(valueChanged(int)), ui->spn_led3, SLOT(setValue(int)));
*/
	// Buttons
	connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
	connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(commOpenPort())); //Connect Button (Liste der seriellen Ports)
	connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(commClosePort())); //Disconnect Button

/*	// Spinfeld change event
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), this, SLOT(led0_commRefresh(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), this, SLOT(led1_commRefresh(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), this, SLOT(led2_commRefresh(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), this, SLOT(led3_commRefresh(int)));
*/
    connect(comm, SIGNAL(receivedByte(int8_t)), this, SLOT(receivedByte(int8_t)));
}

/*
 *
 * Destruktor
 */
MainWindow::~MainWindow()
{
	delete comm;
	delete ui;
}

/*
 * Verbindung zum Refresh Button, lädt die Portliste der verfügbaren Comports
 *
 */
void MainWindow::refreshPortList()
{
	ui->cmb_serialPorts->clear();
	Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
	{
	   ui->cmb_serialPorts->addItem(port.portName());
	}

    /////// ONLY FOR DEBUGGING PURPOSES - VIRTUAL SERIAL PORT!
    ui->cmb_serialPorts->addItem("pts/5");
}

/*
 * Connection zum Open Button, verbindet mit dem seriellen Port
 *
 */
void MainWindow::commOpenPort()
{
	if(ui->cmb_serialPorts->currentText() != "")
	{
		if(comm->openPort("/dev/" + ui->cmb_serialPorts->currentText())) // Vebindung konnte erfolgeich hergestellt werden
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

/*
 * Connection zum Close Button, schließt den seriellen Port
 *
 */
void MainWindow::commClosePort()
{
	comm->closePort();
	ui->btn_connect->setEnabled(true);
	ui->btn_disconnect->setEnabled(false);
	ui->lbl_current_state->setText("Not connected");
}

/*
 * Neues Paket von der Comm Klasse empfangen
 *
 */
void MainWindow::receivedByte(int8_t byte)
{

}

/*
 * Timer für die Aktualisierung des Graphen abgelaufen:
 * Neu zeichnen
 *
 */
void MainWindow::updateTempGraph(void)
{
	if(comm->isConnected())
	{
		using namespace std;
        double height_max = *max_element(height_history.begin(), height_history.end());
        double height_min = *min_element(height_history.begin(), height_history.end());

        height_history.pop_back();
        height_history.push_front(height);

        ui->plot_flight->graph(0)->setData(height_time, height_history);
        ui->plot_flight->yAxis2->setRange(height_min - 2, height_max + 2); // yAxis 2 ist die rechte Y-Achse. yAxis 1 ist deaktiviert, aber alle Werte bezeiehn sich auf diese Achse, wewegen bei beiden die Range eingestellt werden muss
        ui->plot_flight->yAxis->setRange(height_min - 2, height_max + 2);
        ui->plot_flight->replot();
	}
}
