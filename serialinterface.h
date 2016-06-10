#ifndef FPGASerialInterface_H
#define FPGASerialInterface_H

#include <QMainWindow>

#include <QSerialPort>
#include <inttypes.h>

class SerialInterface: public QObject
{
    Q_OBJECT

public:
    SerialInterface();
    ~SerialInterface();
    bool openPort(const QString port);
    bool isConnected(void);
    void closePort(void);
    void writeByte(int8_t b);
    void writeString(int8_t *s, int length);

    static void static_writeString(SerialInterface *si, int8_t *s, uint16_t length);

private:
    QSerialPort *serial; // Serielle Schnittstelle

private slots:
    void readPort(void); // Wird von der seriellen Schnittstelle aufgerufen, wenn neue Daten zur Verfügung stehen

signals:
    void receivedByte(int8_t byte);
};

#endif // FPGASerialInterface_H
