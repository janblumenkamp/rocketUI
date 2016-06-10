#ifndef FPGACOMM_H
#define FPGACOMM_H

#include <QMainWindow>

#include <QSerialPort>

class Comm: public QObject
{
	Q_OBJECT

public:
	Comm();
	~Comm();
	bool openPort(const QString port);
	bool isConnected(void);
    void closePort(void);
    void writeByte(int8_t b);

private:
	QSerialPort *serial; // Serielle Schnittstelle

private slots:
	void readPort(void); // Wird von der seriellen Schnittstelle aufgerufen, wenn neue Daten zur Verfügung stehen

signals:
    void receivedByte(int8_t byte);
};

#endif // FPGACOMM_H
