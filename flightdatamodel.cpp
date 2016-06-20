#include <QVector>
#include <QDebug>
#include <algorithm>

#include "flightdatamodel.h"
#include "xmlparser.h"

FlightdataModel::FlightdataModel(QObject *parent) :
    QAbstractTableModel(parent) {
    xmlparser = new XMLParser("commdef.xml",
                              "commdef",
                              (XMLParser::callback_t) FlightdataModel::xmlcallback,
                              this);
    xmlparser->read();
}

FlightdataModel::~FlightdataModel() {
    delete xmlparser;
}

void FlightdataModel::xmlcallback(QString &identifier, QVector< QVector<QString> > &entrys, FlightdataModel *flightdatamodel) {
    if(identifier == "data") {
        flightdatamodel->label_data.append(entrys[1][3]);
        flightdatamodel->label_unit.append(entrys[1][4]);
    }
}

int FlightdataModel::rowCount(const QModelIndex & /*parent*/) const {
   return label_data.length();
}

int FlightdataModel::columnCount(const QModelIndex & /*parent*/) const {
    return COL_NUM;
}

QVariant FlightdataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case COL_IS:    return QString("Is");
            case COL_MIN:   return QString("Min");
            case COL_MAX:   return QString("Max");
            case COL_UNIT:  return QString("Unit");
            }
        }
        if (orientation == Qt::Vertical) {
            return label_data[section];
        }
    }
    return QVariant();
}

QVariant FlightdataModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if(index.column() == COL_UNIT) {
            return label_unit[index.row()];
        } else if(flightdata[index.row()].isEmpty()) {
            return QString("None");
        } else {
            double val;

            switch (index.column()) {
            case COL_IS:
                val = flightdata[index.row()][flightdata[index.row()].length() - 1];
                break;
            case COL_MIN:
                val = *std::min_element(flightdata[index.row()].begin(), flightdata[index.row()].end());
                break;
            case COL_MAX:
                val = *std::max_element(flightdata[index.row()].begin(), flightdata[index.row()].end());
                break;
            }
            return QString("%1").arg(val, 0, 'f', 2);
        }
    }
    return QVariant();
}

void FlightdataModel::addData(package_type_data_t i, double value) {
    flightdata[i].push_back(value);
}
