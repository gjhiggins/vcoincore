// Copyright (c) 2015-2016 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TORRENTTABLEMODEL_H
#define TORRENTTABLEMODEL_H

#include "walletmodel.h"

#include <QAbstractTableModel>
#include <QStringList>
#include <QtSql>

class PlatformStyle;
class TorrentTablePriv;
class WalletModel;
/**
   Qt model of the list of torrents in the store.
   This allows views to access the list of torrents.
 */
class TorrentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TorrentTableModel(WalletModel *parent = 0);
    ~TorrentTableModel();

    enum ColumnIndex {
        Title = 0,
        Address,
        Amount
    };

    enum RoleIndex {
        /** Type of transaction */
        TypeRole = Qt::UserRole,
        /** Date and time this transaction was created */
        TorrentRole
    };

    /** @name Methods overridden from QAbstractTableModel
        @{*/
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    // Qt::ItemFlags flags(const QModelIndex &index) const;

    void refreshTorrentTable();    

private:
    WalletModel *walletModel;
    QStringList columns;
    QSqlDatabase db;
    TorrentTablePriv *priv;
    // const PlatformStyle *platformStyle;

    // void subscribeToCoreSignals();
    // void unsubscribeFromCoreSignals();

public Q_SLOTS:
    // void updateEntry(const QString &name, const QString &value, int nHeight, int status, int *outNewRowIndex = NULL);

    friend class TorrentTablePriv;
};

struct TorrentTableEntry
{

    QString title;
    QString link;
    QString address;
    QString amount;

    TorrentTableEntry() {}
    TorrentTableEntry(const QString &titlestr,const QString &linkstr, const QString &addressstr, const QString &amountstr):
        title(titlestr),link(linkstr),address(addressstr),amount(amountstr) {}
};


#endif // TORRENTTABLEMODEL_H
