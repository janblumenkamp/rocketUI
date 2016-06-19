#include <QFile>
#include <QXmlStreamReader>
#include <QString>
#include <QDebug>

#include "flightdataxmlreader.h"

FlightDataXMLReader::FlightDataXMLReader(const QString filename):
    _filename(filename)
{
}

void FlightDataXMLReader::read() {
    QFile xmlFile(_filename);
    xmlFile.open(QIODevice::ReadOnly);
    _xml.setDevice(&xmlFile);

    if (_xml.readNextStartElement() && _xml.name() == "commdef") {
       processCommdef();
    }

    // readNextStartElement() leaves the stream in
    // an invalid state at the end. A single readNext()
    // will advance us to EndDocument.
    if (_xml.tokenType() == QXmlStreamReader::Invalid) {
        _xml.readNext();
    }

    if (_xml.hasError()) {
        _xml.raiseError();
        qDebug() << errorString();
    }
}

void FlightDataXMLReader::processCommdef() {
    if (!_xml.isStartElement()) {
        return;
    }
    while (_xml.readNextStartElement()) {
        if (_xml.name() == "data") {
            processData();
        } else {
            _xml.skipCurrentElement();
        }
    }
}

void FlightDataXMLReader::processData() {
    if (!_xml.isStartElement()) {
        return;
    }

    QString entry_id;
    QString entry_specifier;
    QString entry_short;
    QString entry_description;
    while (_xml.readNextStartElement()) {
        if (_xml.name() == "id") {
            entry_id = readNextText();
        } else if (_xml.name() == "specifier") {
            entry_specifier = readNextText();
        } else if (_xml.name() == "short") {
            entry_short = readNextText();
        } else if (_xml.name() == "description") {
            entry_description = readNextText();
        }
        _xml.skipCurrentElement();
    }

    if (!(entry_id.isEmpty() || entry_specifier.isEmpty() || entry_short.isEmpty() || entry_description.isEmpty())) {
        qDebug() << entry_id << entry_specifier << entry_short << entry_description;
    }
}

QString FlightDataXMLReader::readNextText() {
    _xml.readNext();
    return _xml.text().toString();
}

QString FlightDataXMLReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(_xml.errorString())
            .arg(_xml.lineNumber())
            .arg(_xml.columnNumber());
}
