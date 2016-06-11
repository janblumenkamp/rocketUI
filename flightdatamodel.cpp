#include "flightdatamodel.h"

FlightdataModel::FlightdataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int FlightdataModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 2;
}

int FlightdataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant FlightdataModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
       return QString("Row%1, Column%2")
                   .arg(index.row() + 1)
                   .arg(index.column() +1);
    }
    return QVariant();
}
