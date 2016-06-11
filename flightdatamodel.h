#ifndef FLIGHTDATAMODEL_H
#define FLIGHTDATAMODEL_H

#include <QAbstractTableModel>

class FlightdataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FlightdataModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // FLIGHTDATAMODEL_H
