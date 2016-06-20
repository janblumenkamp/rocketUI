/*
 * http://3gfp.com/wp/2014/07/three-ways-to-parse-xml-in-qt/
 */

#ifndef FLIGHTDATAXMLREADER_H
#define FLIGHTDATAXMLREADER_H

#include <QFile>
#include <QXmlStreamReader>
#include <QString>
#include <QDebug>

class FlightDataXMLReader
{
public:
    FlightDataXMLReader(const QString filename);

    void read();

private:
    void processCommdef();
    void processData();
    QString readNextText();
    QString errorString();

    QString _filename;
    QXmlStreamReader _xml;
};

#endif // FLIGHTDATAXMLREADER_H
