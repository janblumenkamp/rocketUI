/*
 * http://3gfp.com/wp/2014/07/three-ways-to-parse-xml-in-qt/
 */

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QFile>
#include <QXmlStreamReader>
#include <QString>
#include <QDebug>
#include <QVector>

class XMLParser
{
public:
    typedef void (*callback_t)(QString &identfier, QVector< QVector<QString> > &entrys, void *userdata);

    XMLParser(const QString filename, const QString firstlevel, callback_t callback, void *userdata);

    void read();

private:
    callback_t _callback;

    void *_callback_userdata;
    QVector< QVector<QString> > _currententrys;
    QString _currentidentifier;

    void processContent();
    QString errorString();

    QString _filename;
    QString _firstlevel;
    QXmlStreamReader _xml;
};

#endif // XMLPARSER_H
