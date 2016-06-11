#ifndef MSGTABLEMODEL_H
#define MSGTABLEMODEL_H

#include <QAbstractTableModel>

class msgtablemodel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit msgtablemodel(QObject *parent = 0);

signals:

public slots:

};

#endif // MSGTABLEMODEL_H
