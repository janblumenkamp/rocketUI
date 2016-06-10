#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	Ui::MainWindow *ui;
    Comm *comm;
    double height;

    QVector<double> height_history, height_time;
private slots:
    void refreshPortList();
    void commOpenPort();
    void commClosePort();

    /*void led0_commRefresh(int val); // Änderung der Werte in den LEDs
	void led1_commRefresh(int val);
	void led2_commRefresh(int val);
    void led3_commRefresh(int val);*/

    void receivedByte(int8_t byte);

    void updateTempGraph(void); // Aktualisieren des Temperaturgraphen
};

#endif // MAINWINDOW_H
