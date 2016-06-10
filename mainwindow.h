#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

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

    QTime timer_start; // Stores time since system start

    Comm_t comm;
    Memory_t m_in, m_out;
    MemoryEntry_t mementr_in[MEMORY_IN_LENGTH/MEMORY_INOUT_ALIGN];
    MemoryEntry_t mementr_out[MEMORY_OUT_LENGTH/MEMORY_INOUT_ALIGN];

    double height;

    QVector<double> height_history, height_time;
private slots:
    void refreshPortList();
    void serialOpenPort();
    void serialClosePort();

    /*void led0_commRefresh(int val); // Änderung der Werte in den LEDs
	void led1_commRefresh(int val);
	void led2_commRefresh(int val);
    void led3_commRefresh(int val);*/

    void receivedByte(int8_t byte);

    void updateTempGraph(void); // Aktualisieren des Temperaturgraphen
};

#endif // MAINWINDOW_H
