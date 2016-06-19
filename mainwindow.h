#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>

#include "flightdatamodel.h"
#include "msgtablemodel.h"
#include "serialinterface.h"

extern "C" {
    #include "EMBcomm/Comm.h"
    #include "EMBcomm/HAL_Memory.h"
}

#define MEMORY_IN_START     0
#define MEMORY_IN_LENGTH    MEM_LENGTH/2 // Seperate the physical memory 50/50 between ingoing and outgoing package memory
#define MEMORY_OUT_START    MEMORY_IN_START+MEMORY_IN_LENGTH
#define MEMORY_OUT_LENGTH   MEM_LENGTH/2
#define MEMORY_INOUT_ALIGN  64

#define TIMER_LAUNCH_DISPLAY_FORMAT "mm:ss.zzz"
#define TIMER_LAUNCH_INCREMENT_MS 20

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static uint32_t getTimeMS(MainWindow *w);

private:
	Ui::MainWindow *ui;
    SerialInterface *sinterface;

    FlightdataModel *flightdatamodel;
    MSGTableModel *msgtblmdl_out;
    MSGTableModel *msgtblmdl_in;

    QTime time_since_start; // Stores time since system start
    QTime time_since_launch;
    QTimer *timer_launch; // Time since launch

    Comm_t comm;
    Memory_t m_in, m_out;
    MemoryEntry_t mementr_in[MEMORY_IN_LENGTH/MEMORY_INOUT_ALIGN];
    MemoryEntry_t mementr_out[MEMORY_OUT_LENGTH/MEMORY_INOUT_ALIGN];

    int16_t pack_id_calibrate; //Store the message ID of the calibration command to verify calibration (ack) and only then activate the launch button

private slots:
    void refreshPortList();
    void serialOpenPort();
    void serialClosePort();

    void calibrate();
    void launch();

    void timer_launch_event(); //Timer to refresh the time stop

    void receivedByte(int8_t byte);

    void updateTempGraph(void); // Aktualisieren des Temperaturgraphen
};

#endif // MAINWINDOW_H
