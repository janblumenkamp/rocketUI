#include "msgtablemodel.h"

extern "C" {
    #include "EMBcomm/Comm_Package.h"
}

MSGTableModel::MSGTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int MSGTableModel::rowCount(const QModelIndex & /*parent*/) const {
   return packages.length();
}

int MSGTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return COL_NUM;
}

QVariant MSGTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case COL_ID:        return QString("ID");
            case COL_TYPE:      return QString("Type");
            case COL_REG:       return QString("Reg");
            case COL_LENGTH:    return QString("Length");
            case COL_ACK:       return QString("Ack");
            }
        }
    }
    return QVariant();
}

QVariant MSGTableModel::data(const QModelIndex &index, int role) const {
    int row = rowCount() - index.row() - 1; // Inverse the row so that we see the newest massages on the top

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case COL_ID:
            return QString("%1").arg(packages[row].p.id);
        case COL_TYPE:
            switch (packages[row].p.type) {
            case PACKAGE_TYPE_ACK:      return QString("ACK");
            case PACKAGE_TYPE_DATA:     return QString("DATA");
            case PACKAGE_TYPE_CMD:      return QString("CMD");
            case PACKAGE_TYPE_STATUS:   return QString("STATUS");
            case PACKAGE_TYPE_ERROR:    return QString("ERROR");
            }
            break;
        case COL_REG:
            return QString("%1").arg(packages[row].p.reg);
        case COL_LENGTH:
            return QString("%1").arg(packages[row].p.length);
        case COL_ACK:
            break;
        }
    } else if(role == Qt::CheckStateRole) {
        if(index.column() == COL_ACK) {
            return packages[row].ack ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

void MSGTableModel::addData(Comm_Package_t *p) {
    if(rowCount() < MAX_ENTRYS) {
        beginInsertRows(QModelIndex(), 0, 0); // Add one row in the top
        endInsertRows();
    } else {
        packages.pop_front();
    }
    Package_t pck;
    pck.p = *p;
    packages.push_back(pck);
}
