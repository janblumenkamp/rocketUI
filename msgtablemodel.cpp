#include "msgtablemodel.h"

extern "C" {
    #include "EMBcomm/Comm_Package.h"
    #include "EMBcomm/PackageMemory.h"
    #include "EMBcomm/Comm.h"
}

MSGTableModel::MSGTableModel(QObject *parent, Memory_t *m) :
    QAbstractTableModel(parent)
{
    mem = m;
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
            default:                    return QString("%i").arg(packages[row].p.type);
            }
            break;
        case COL_REG:
            switch (packages[row].p.type) {
            case PACKAGE_TYPE_ACK:
                return QString("ACK");
            case PACKAGE_TYPE_DATA:
                switch (packages[row].p.reg) {
                case PACKAGE_DATA_ALTITUDE:     return QString("ALT");
                case PACKAGE_DATA_SPEED_VERT:   return QString("SPEED");
                case PACKAGE_DATA_ACC_X:        return QString("ACC_X");
                case PACKAGE_DATA_ACC_Y:        return QString("ACC_Y");
                case PACKAGE_DATA_ACC_Z:        return QString("ACC_Z");
                case PACKAGE_DATA_GYR_X:        return QString("GYR_X");
                case PACKAGE_DATA_GYR_Y:        return QString("GYR_Y");
                case PACKAGE_DATA_GYR_Z:        return QString("GYR_Z");
                case PACKAGE_DATA_IMU_X:        return QString("IMU_X");
                case PACKAGE_DATA_IMU_Y:        return QString("IMU_Y");
                case PACKAGE_DATA_IMU_Z:        return QString("IMU_Z");
                case PACKAGE_DATA_TEMPERATURE:  return QString("TEMP");
                default:                        return QString("%1").arg(packages[row].p.reg);
                }
                break;
            case PACKAGE_TYPE_CMD:
                switch (packages[row].p.reg) {
                case PACKAGE_CMD_CALIBRATE:         return QString("CALIBR");
                case PACKAGE_CMD_DEPLOY_PARACHUTE:  return QString("PARACH");
                case PACKAGE_CMD_SET_STREAM:        return QString("STREAM");
                case PACKAGE_CMD_START:             return QString("START");
                default:                            return QString("%1").arg(packages[row].p.reg);
                }
                break;
            case PACKAGE_TYPE_STATUS:
                switch (packages[row].p.reg) {
                case PACKAGE_STATUS_IDLE:           return QString("IDLE");
                case PACKAGE_STATUS_ACCEL_UP:       return QString("ACC_UP");
                case PACKAGE_STATUS_DECEL_UP:       return QString("DEC_UP");
                case PACKAGE_STATUS_EPO:            return QString("EPO");
                case PACKAGE_STATUS_PARACHUTE:      return QString("PARACH");
                case PACKAGE_STATUS_ACCEL_DOWN:     return QString("ACC_DOWN");
                case PACKAGE_STATUS_LANDED:         return QString("LANDED");
                default:                            return QString("%1").arg(packages[row].p.reg);
                }
                break;
            case PACKAGE_TYPE_ERROR:
                switch (packages[row].p.reg) {
                case PACKAGE_ERROR_CALIBRATE:   return QString("CALIBR");
                case PACKAGE_ERROR_DEPLOY:      return QString("PARACH");
                case PACKAGE_ERROR_MEMORY:      return QString("MEM");
                case PACKAGE_ERROR_START:       return QString("START");
                default:                        return QString("%1").arg(packages[row].p.reg);
                }
                break;
            }
        case COL_LENGTH:
            return QString("%1").arg(packages[row].p.length);
        case COL_ACK: //Handled in CheckStateRole
            break;
        }
    } else if(role == Qt::CheckStateRole) {
        if(index.column() == COL_ACK) {
            if(!packages[row].ack) {
                if(PackageMemory_GetMemID(mem, packages[row].p.id) < 0) {
                    // The package ID was not found in the memory, that means the entry is not available anymore and therefore an ack received.
                    //packages[row].ack = true;
                }
            }
            return packages[row].ack == true ? Qt::Checked : Qt::Unchecked;
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
    pck.ack = false;
    packages.push_back(pck);
}

int MSGTableModel::send(Comm_Package_t *p) {
    addData(p);
    return PackageMemory_ToMemory(mem, p);
}
