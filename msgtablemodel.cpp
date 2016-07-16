#include "msgtablemodel.h"

extern "C" {
    #include "EMBcomm/Comm_Package.h"
    #include "EMBcomm/PackageMemory.h"
    #include "EMBcomm/Comm.h"
}

MSGTableModel::MSGTableModel(QObject *parent, Memory_t *m, Comm_t *c) :
    QAbstractTableModel(parent)
{
    mem = m;
    comm = c;

    xmlparser = new XMLParser("commdef.xml",
                              "commdef",
                              (XMLParser::callback_t) MSGTableModel::xmlcallback,
                              this);
    xmlparser->read();
}

MSGTableModel::~MSGTableModel() {
    delete xmlparser;
}

void MSGTableModel::xmlcallback(QString &identifier, QVector< QVector<QString> > &entrys, MSGTableModel *msgtable) {
    if(identifier == "data") {
        if(entrys[0][CALLBACK_ENTRY_OFFSET_DATA_SHORT] == "short") {
            msgtable->label_data.append(entrys[1][CALLBACK_ENTRY_OFFSET_DATA_SHORT]);
        }
    } else if(identifier == "cmd") {
        if(entrys[0][CALLBACK_ENTRY_OFFSET_CMD_SHORT] == "short") {
            msgtable->label_cmd.append(entrys[1][CALLBACK_ENTRY_OFFSET_CMD_SHORT]);
        }
    } else if(identifier == "error") {
        if(entrys[0][CALLBACK_ENTRY_OFFSET_ERROR_SHORT] == "short") {
            msgtable->label_err.append(entrys[1][CALLBACK_ENTRY_OFFSET_ERROR_SHORT]);
        }
    } else if(identifier == "status") {
        if(entrys[0][CALLBACK_ENTRY_OFFSET_STATUS_SHORT] == "short") {
            msgtable->label_status.append(entrys[1][CALLBACK_ENTRY_OFFSET_STATUS_SHORT]);
        }
    }
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
            case PACKAGE_TYPE_ACK:      return QString("ACK");
            case PACKAGE_TYPE_DATA:     return label_data[packages[row].p.reg];
            case PACKAGE_TYPE_CMD:      return label_cmd[packages[row].p.reg];
            case PACKAGE_TYPE_STATUS:   return label_status[packages[row].p.reg];
            case PACKAGE_TYPE_ERROR:    return label_err[packages[row].p.reg];
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
                    packages[row].ack = true;
                }
            }
            return packages[row].ack == true ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

void MSGTableModel::addData(Comm_Package_t *p) {
    // The automatic incrementation of the id is also in the Comm_Sendallpackages function, but we need it now because otherwise the actual id will not be displayed.
    if(p->id == 0) {
        p->id = comm->id_transmit_cnt++;
    }

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
