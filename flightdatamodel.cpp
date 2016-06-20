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

void FlightdataModel::xmlcallback(QString &identfier, QVector< QVector<QString> > &entrys, FlightdataModel *flightdatamodel) {
    qDebug() << identfier;
    qDebug() << entrys[0][0] << entrys[0][1] << entrys[0][2] << entrys[0][3];
    qDebug() << entrys[1][0] << entrys[1][1] << entrys[1][2] << entrys[1][3];
}

int FlightdataModel::rowCount(const QModelIndex & /*parent*/) const {
   return PACKAGE_DATA_NUM;
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
            }
        }
        if (orientation == Qt::Vertical) {
            switch (section) {
            case PACKAGE_DATA_ALTITUDE:     return QString("Altitude");
            case PACKAGE_DATA_SPEED_VERT:   return QString("Vert. Speed");
            case PACKAGE_DATA_ACC_X:        return QString("Acc X");
            case PACKAGE_DATA_ACC_Y:        return QString("Acc Y");
            case PACKAGE_DATA_ACC_Z:        return QString("Acc Z");
            case PACKAGE_DATA_GYR_X:        return QString("Gyr X");
            case PACKAGE_DATA_GYR_Y:        return QString("Gyr Y");
            case PACKAGE_DATA_GYR_Z:        return QString("Gyr Z");
            case PACKAGE_DATA_IMU_X:        return QString("IMU X");
            case PACKAGE_DATA_IMU_Y:        return QString("IMU Y");
            case PACKAGE_DATA_IMU_Z:        return QString("IMU Z");
            case PACKAGE_DATA_TEMPERATURE:  return QString("Temp");
            }
        }
    }
    return QVariant();
}

QVariant FlightdataModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if(flightdata[index.row()].isEmpty()) {
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
