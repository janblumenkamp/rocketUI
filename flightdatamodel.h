#ifndef FLIGHTDATAMODEL_H
#define FLIGHTDATAMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <algorithm>
#include <vector>

#include "RocketComm_Defs.h"
#include "xmlparser.h"

class FlightdataModel : public QAbstractTableModel {
    Q_OBJECT

private:
    static void xmlcallback(QString &identfier, QVector< QVector<QString> > &entrys, FlightdataModel *flightdatamodel);

    enum COLUMNS {
        COL_IS, COL_MIN, COL_MAX, COL_UNIT,
        ///
        COL_NUM
    };

    QVector<double> flightdata[PACKAGE_DATA_NUM];
    XMLParser *xmlparser;

    QVector<QString> label_data; // The labels are arsed from the xml file into this vector.
    QVector<QString> label_unit; // The units as string

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
