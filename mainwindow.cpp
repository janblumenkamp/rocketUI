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
    comm = new Comm(); // Neues Objekt der FPGA Kommunikationsklasse

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
	ui->temp_plot->setBackground(Qt::transparent);
	ui->temp_plot->setAttribute(Qt::WA_OpaquePaintEvent, false);

    // Verbinde Spinfelder mit Slider:
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), ui->sld_led0, SLOT(setValue(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), ui->sld_led1, SLOT(setValue(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), ui->sld_led2, SLOT(setValue(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), ui->sld_led3, SLOT(setValue(int)));

	 // Verbinde Slider mit Spinfelder:
	connect(ui->sld_led0, SIGNAL(valueChanged(int)), ui->spn_led0, SLOT(setValue(int)));
	connect(ui->sld_led1, SIGNAL(valueChanged(int)), ui->spn_led1, SLOT(setValue(int)));
	connect(ui->sld_led2, SIGNAL(valueChanged(int)), ui->spn_led2, SLOT(setValue(int)));
	connect(ui->sld_led3, SIGNAL(valueChanged(int)), ui->spn_led3, SLOT(setValue(int)));

	// Buttons
	connect(ui->btn_refreshPortList, SIGNAL(clicked()), this, SLOT(refreshPortList())); //Refresh Button (Liste der seriellen Ports)
	connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(commOpenPort())); //Connect Button (Liste der seriellen Ports)
	connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(commClosePort())); //Disconnect Button

	// Spinfeld change event
	connect(ui->spn_led0, SIGNAL(valueChanged(int)), this, SLOT(led0_commRefresh(int)));
	connect(ui->spn_led1, SIGNAL(valueChanged(int)), this, SLOT(led1_commRefresh(int)));
	connect(ui->spn_led2, SIGNAL(valueChanged(int)), this, SLOT(led2_commRefresh(int)));
	connect(ui->spn_led3, SIGNAL(valueChanged(int)), this, SLOT(led3_commRefresh(int)));

	// Neues Paket von der FPGA Klasse
    connect(comm, SIGNAL(receivedPackage(Comm::Package*)), this, SLOT(newPackage(Comm::Package*)));
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

			// TODO: Manchmal wird korrekt gesendet, manchmal nicht...

			// Lese nun die Werte ua sdem FPGA Board für die LEDs
            comm->queryReg(Comm::LED0);
            comm->queryReg(Comm::LED1);
            comm->queryReg(Comm::LED2);
            comm->queryReg(Comm::LED3);
            comm->queryReg(Comm::BUTTONS);
            comm->queryReg(Comm::TEMP_INT);
            comm->queryReg(Comm::TEMP_DEZI);
            comm->queryReg(Comm::RGB_R);
            comm->queryReg(Comm::RGB_G);
            comm->queryReg(Comm::RGB_B);
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
 * LED 1 Änderung - aktualisiere Register
 *
 */
void MainWindow::led0_commRefresh(int val)
{
	comm->setReg(0, (unsigned char)val);
}

/*
 * LED 2 Änderung - aktualisiere Register
 *
 */
void MainWindow::led1_commRefresh(int val)
{
	comm->setReg(1, (unsigned char)val);
}

/*
 * LED 3 Änderung - aktualisiere Register
 *
 */
void MainWindow::led2_commRefresh(int val)
{
	comm->setReg(2, (unsigned char)val);
}

/*
 * LED 4 Änderung - aktualisiere Register
 *
 */
void MainWindow::led3_commRefresh(int val)
{
	comm->setReg(3, (unsigned char)val);
}

/*
 * Neues Paket von der Comm Klasse empfangen
 *
 */
void MainWindow::newPackage(Comm::Package *p)
{
	if(p->rw == true) // Schreibzugriff
	{
		QColor c;
		switch (p->reg) {
        case Comm::LED0:
			ui->spn_led0->setValue(p->data);
			break;
        case Comm::LED1:
			ui->spn_led1->setValue(p->data);
			break;
        case Comm::LED2:
			ui->spn_led2->setValue(p->data);
			break;
        case Comm::LED3:
			ui->spn_led3->setValue(p->data);
			break;
        case Comm::BUTTONS:
			ui->chk_btnA->setChecked(p->data & (1<<0));
			ui->chk_btnB->setChecked(p->data & (1<<1));
			ui->chk_btnC->setChecked(p->data & (1<<2));
			break;
        case Comm::TEMP_INT:
			temperature = (temperature - qFloor(temperature)) + p->data; // Ändere nur die Ganzzahl vor dem Komma
			break;
        case Comm::TEMP_DEZI:
			temperature = qFloor(temperature) + ((double)p->data / 255);
			break;
		default:
			break;
		}
	}
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

/*
 * Slot - der Benutzer hat die Farbe geändert in der Farbauswahl des Farbdreiecks
 */
void MainWindow::rgb1ColorChanged(const QColor &col)
{
    comm->setReg(Comm::RGB_R, col.red());
    comm->setReg(Comm::RGB_G, col.green());
    comm->setReg(Comm::RGB_B, col.blue());
}
