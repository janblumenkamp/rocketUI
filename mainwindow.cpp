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
	timer_tempGraph = new QTimer(this);
	connect(timer_tempGraph, SIGNAL(timeout()), this, SLOT(updateTempGraph()));
	timer_tempGraph->start(50); // 20Hz
	temperature_history = QVector<double>(TEMP_VALUES); // N Werte in Vektor speichern können
	temperature_time = QVector<double>(TEMP_VALUES); // N Werte für Zeitachse
	for(int i = 0; i < TEMP_VALUES; i++)
	{
		temperature_time[i] = -i;
	}
	ui->temp_plot->addGraph();
	ui->temp_plot->graph(0)->setData(temperature_time, temperature_history);
	ui->temp_plot->xAxis->setLabel("t");
	ui->temp_plot->xAxis->setRange(-TEMP_VALUES, 0);
	ui->temp_plot->yAxis->setVisible(false); // Die linke Y-Achse soll deaktiviert werden und die recht aktiviert werden (damit die aktuelle Temperatur direkt abgelesen werden kann)
	ui->temp_plot->yAxis2->setVisible(true);
	ui->temp_plot->yAxis2->setLabel("Temperatur in" + QString::fromUtf8("°") + "C");
    //ui->temp_plot->setBackground(Qt::transparent);
    //ui->temp_plot->setAttribute(Qt::WA_OpaquePaintEvent, false);

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
		double temperature_max = *max_element(temperature_history.begin(), temperature_history.end());
		double temperature_min = *min_element(temperature_history.begin(), temperature_history.end());

		temperature_history.pop_back();
		temperature_history.push_front(temperature);

		ui->temp_plot->graph(0)->setData(temperature_time, temperature_history);
		ui->temp_plot->yAxis2->setRange(temperature_min - 2, temperature_max + 2); // yAxis 2 ist die rechte Y-Achse. yAxis 1 ist deaktiviert, aber alle Werte bezeiehn sich auf diese Achse, wewegen bei beiden die Range eingestellt werden muss
		ui->temp_plot->yAxis->setRange(temperature_min - 2, temperature_max + 2);
		ui->temp_plot->replot();
	}
}
