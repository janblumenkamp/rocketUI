#ifndef MSGTABLEMODEL_H
#define MSGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDebug>

#include "RocketComm_Defs.h"
#include "xmlparser.h"

extern "C" {
    #include "EMBcomm/Comm_Package.h"
    #include "EMBcomm/Comm.h"
}

class MSGTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    #define MAX_ENTRYS 1000

#define CALLBACK_ENTRY_OFFSET_DATA_SHORT 3
#define CALLBACK_ENTRY_OFFSET_STATUS_SHORT 2
#define CALLBACK_ENTRY_OFFSET_ERROR_SHORT 2
#define CALLBACK_ENTRY_OFFSET_CMD_SHORT 2

    static void xmlcallback(QString &identfier, QVector< QVector<QString> > &entrys, MSGTableModel *msgtable);

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

    mutable QVector<Package_t> packages;
    XMLParser *xmlparser;

    Memory_t *mem;
    Comm_t *comm;

    QVector<QString> label_data, label_cmd, label_status, label_err;

public:
    MSGTableModel(QObject *parent, Memory_t *m, Comm_t *c);
    ~MSGTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void addData(Comm_Package_t *p);
    int send(Comm_Package_t *p);
};

#endif // MSGTABLEMODEL_H
