// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INSCRIPTIONTABLEMODEL_H
#define BITCOIN_QT_INSCRIPTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class CWallet;
class InscriptionTablePriv;
class WalletModel;

/**
   Qt model of the inscription. This allows views to access the inscription data.
 */

class InscriptionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit InscriptionTableModel(CWallet* wallet, WalletModel *parent = 0);
    ~InscriptionTableModel();

    enum ColumnIndex {
        Inscription = 0,
        Date = 1
    };

    enum RoleIndex {
        /** Type of transaction */
        TypeRole = Qt::UserRole,
        /** Date and time this transaction was created */
        InscriptionRole
    };

    /** @name Methods overridden from QAbstractTableModel
        @{*/
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex & parent) const;
    void getLastInscription();
    void getInscriptions();

private:
    WalletModel *walletModel;
    CWallet* wallet;
    InscriptionTablePriv *priv;
    QStringList columns;

public Q_SLOTS:
    void refreshInscriptionTable();

    friend class InscriptionTablePriv;
};

#endif // BITCOIN_QT_INSCRIPTIONTABLEMODEL_H
