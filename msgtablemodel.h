#ifndef MSGTABLEMODEL_H
#define MSGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDebug>

#include "RocketComm_Defs.h"

extern "C" {
    #include "EMBcomm/Comm_Package.h"
    #include "EMBcomm/Comm.h"
}

class MSGTableModel : public QAbstractTableModel
{
    Q_OBJECT

    #define MAX_ENTRYS 1000

    enum COLUMNS {
        COL_ID, COL_TYPE, COL_REG, COL_LENGTH, COL_ACK,
        ////
        COL_NUM
    };

    struct Package_t {
        Comm_Package_t p;
        bool ack;
    };
    typedef struct Package_t Package_t;

    QVector<Package_t> packages;

    Memory_t *mem;

public:
    MSGTableModel(QObject *parent, Memory_t *m);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void addData(Comm_Package_t *p);
    int send(Comm_Package_t *p);
};

#endif // MSGTABLEMODEL_H
