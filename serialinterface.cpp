#include <QLabel>
#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include "serialinterface.h"

SerialInterface::SerialInterface()
{
	// Serieller Port
	serial = new QSerialPort();

	// Wenn neue Daten zur Verfügung stehen
    connect(serial, SIGNAL(readyRead()), this, SLOT(readPort()));
}

SerialInterface::~SerialInterface() // Destruktor
{
	closePort();
}

bool SerialInterface::openPort(const QString port)
{
	serial->setPortName(port);
	serial->setBaudRate(QSerialPort::Baud19200);
	serial->setDataBits(QSerialPort::Data8);
	serial->setParity(QSerialPort::NoParity);
	serial->setStopBits(QSerialPort::OneStop);
	serial->setFlowControl(QSerialPort::NoFlowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		qDebug() << "Opened sucessfully";

        return true;
	} else {
		qDebug() << "Opening port failed!";
		return false;
	}
}

void SerialInterface::writeByte(int8_t b)
{
    if(serial->isOpen())
    {
        unsigned char data[1];
        data[0] = b;
        serial->write((const char *)data, 1);
    }
}

void SerialInterface::writeString(int8_t *s, int length)
{
    if(serial->isOpen())
    {
        serial->write((const char *)s, length);
    }
}

void SerialInterface::static_writeString(SerialInterface *si, int8_t *s, uint16_t length)
{
    si->writeString(s, length);
}

bool SerialInterface::isConnected(void)
{
    return serial->isOpen();
}

void SerialInterface::closePort(void)
{
	if(serial->isOpen())
	{
		serial->close();
		qDebug() << "Serial port closed!";
	}
}

/*
 * Slot - wird aufgerufen, wenn neue Daten im seriellen Port zur Verfügung stehen
 *
 */

void SerialInterface::readPort(void)
{
	QByteArray ser_raw = serial->readAll(); // Speichere Daten in QBytearray

	std::vector<unsigned char> data( // Übertrage Daten in unsigned char vektor
		ser_raw.begin(), ser_raw.end());

	for(std::vector<unsigned char>::iterator i = data.begin(); i != data.end(); ++i)
	{
        emit receivedByte(*i);
    }
}
