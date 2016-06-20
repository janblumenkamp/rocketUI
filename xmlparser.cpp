#include <QFile>
#include <QXmlStreamReader>
#include <QString>
#include <QDebug>
#include <QVector>

#include "xmlparser.h"

XMLParser::XMLParser(const QString filename, const QString firstlevel, callback_t callback, void *userdata):
    _filename(filename)
{
    _firstlevel = firstlevel;
    _callback = callback;
    _callback_userdata = userdata;
}

void XMLParser::read() {
    QFile xmlFile(_filename);
    xmlFile.open(QIODevice::ReadOnly);
    _xml.setDevice(&xmlFile);

    if (_xml.readNextStartElement() && _xml.name() == _firstlevel) {
        while (_xml.readNextStartElement()) {
            processContent();
        }
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

void XMLParser::processContent() {
    if (!_xml.isStartElement()) {
        return;
    }

    _currententrys.clear();
    _currententrys.resize(2);
    _currentidentifier = _xml.name().toString();

    while (_xml.readNextStartElement()) {

        _currententrys[0].push_back(_xml.name().toString());

        _xml.readNext();
        _currententrys[1].push_back(_xml.text().toString());

        _xml.skipCurrentElement();
    }

    if(_callback != NULL) {
        _callback(_currentidentifier, _currententrys, _callback_userdata);
    }
}

QString XMLParser::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(_xml.errorString())
            .arg(_xml.lineNumber())
            .arg(_xml.columnNumber());
}
