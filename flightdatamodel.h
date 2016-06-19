#ifndef FLIGHTDATAMODEL_H
#define FLIGHTDATAMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <algorithm>
#include <vector>

#include "RocketComm_Defs.h"
#include "flightdataxmlreader.h"

class FlightdataModel : public QAbstractTableModel {
    Q_OBJECT

    enum COLUMNS {
        COL_IS, COL_MIN, COL_MAX,
        ///
        COL_NUM
    };

    QVector<double> flightdata[PACKAGE_DATA_NUM];
    FlightDataXMLReader *xmlparser;

public:
    FlightdataModel(QObject *parent);
    ~FlightdataModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addData(package_type_data_t i, double value);
};

#endif // FLIGHTDATAMODEL_H
