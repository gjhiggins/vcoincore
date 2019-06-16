// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/inscriptiontablemodel.h>

#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <base58.h>
#include <wallet/wallet.h>

#include <QFont>
#include <QDateTime>




struct InscriptionTableEntry
{

    QString inscription;
    QDateTime date;

    InscriptionTableEntry() {}
    InscriptionTableEntry(const QString &inscription, const QDateTime &date):
        inscription(inscription),date(date) {}
};

struct InscriptionTableEntryLessThan
{
    bool operator()(const InscriptionTableEntry &a, const InscriptionTableEntry &b) const
    {
        return a.date < b.date;
    }
    // bool operator()(const InscriptionTableEntry &a, const QString &b) const
    // {
    //     return a.date < b;
    // }
    // bool operator()(const QString &a, const InscriptionTableEntry &b) const
    // {
    //     return a < b.date;
    // }
};
// Private implementation
class InscriptionTablePriv
{
public:
    CWallet *wallet;
    QList<InscriptionTableEntry> cachedInscriptionTable;
    InscriptionTableModel *parent;

    InscriptionTablePriv(CWallet *wallet, InscriptionTableModel *parent):
        wallet(wallet), parent(parent) {}

    void getLastInscription()
    {
        /*
        std::vector<std::pair<std::string, int> > vTxResults;
        this->wallet->GetTxMessages(vTxResults);

        QDateTime inscription_date = QDateTime::currentDateTime();

        std::string inscription = vTxResults[0].first;
        inscription_date.setSecsSinceEpoch((qint64)vTxResults[0].second);
        cachedInscriptionTable.append(
            InscriptionTableEntry(
                QString::fromStdString(inscription),
                inscription_date
                ));
        */
    }

    void getAllInscriptions()
    {
        /*
        std::vector<std::pair<std::string, int> > vTxResults;
        this->wallet->GetTxMessages(vTxResults);
        QDateTime inscription_date = QDateTime::currentDateTime();

        for(std::vector<std::pair<std::string, int> >::size_type i = 0; i != vTxResults.size(); i++) {
            std::string inscription = vTxResults[i].first;
            inscription_date.setMSecsSinceEpoch((qint64)vTxResults[i].second*1000);
            cachedInscriptionTable.append(
                InscriptionTableEntry(
                    QString::fromStdString(inscription),
                    inscription_date
            ));
        }
        */
    }

    void getInscriptions()
    {
        /*
        std::vector<std::pair<std::string, int> > vTxResults;
        this->wallet->GetMyTxMessages(vTxResults);
        QDateTime inscription_date = QDateTime::currentDateTime();

        for(std::vector<std::pair<std::string, int> >::size_type i = 0; i != vTxResults.size(); i++) {
            std::string inscription = vTxResults[i].first;
            inscription_date.setSecsSinceEpoch((qint64)vTxResults[i].second);
            cachedInscriptionTable.append(
                InscriptionTableEntry(
                    QString::fromStdString(inscription),
                    inscription_date
                ));
        }
        */
    }

    void refreshInscriptionTable()
    {
        cachedInscriptionTable.clear();
        {
            LOCK(wallet->cs_wallet);

            getInscriptions();

            // for (const std::pair<CTxDestination, CAddressBookData>& item : wallet->mapAddressBook)
            // {
            //     const CTxDestination& address = item.first;
            //     bool fMine = IsMine(*wallet, address);
            //     AddressTableEntry::Type addressType = translateTransactionType(
            //             QString::fromStdString(item.second.purpose), fMine);
            //     const std::string& strName = item.second.name;
            //     cachedAddressTable.append(AddressTableEntry(addressType,
            //                       QString::fromStdString(strName),
            //                       QString::fromStdString(EncodeDestination(address))));
            // }
        }
        // qLowerBound() and qUpperBound() require our cachedAddressTable list to be sorted in asc order
        // Even though the map is already sorted this re-sorting step is needed because the originating map
        // is sorted by binary address, not by base58() address.
        qSort(cachedInscriptionTable.begin(), cachedInscriptionTable.end(), InscriptionTableEntryLessThan());
    }

    int size()
    {
        return cachedInscriptionTable.size();
    }

    InscriptionTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedInscriptionTable.size())
        {
            return &cachedInscriptionTable[idx];
        }
        else
        {
            return 0;
        }
    }

    QString describe(InscriptionTableEntry *rec)
    {
        {
            QString strHTML;

            // LOCK2(cs_main, wallet->cs_wallet);
            strHTML.reserve(4000);
            strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";
            strHTML += "<b>Inscription content:</b> " + rec->inscription + "<br>";
            strHTML += "<b>Inscription date:</b> " + rec->date.toString() + "<br>";
            return strHTML;

        }
        return QString();
    }
};


InscriptionTableModel::InscriptionTableModel(CWallet *_wallet, WalletModel *parent) :
    QAbstractTableModel(parent),walletModel(parent),wallet(_wallet),priv(0)
{
    columns << tr("Content") << tr("Date") ;
    priv = new InscriptionTablePriv(wallet, this);
    priv->refreshInscriptionTable();
}

InscriptionTableModel::~InscriptionTableModel()
{
    delete priv;
}

int InscriptionTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int InscriptionTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

void InscriptionTableModel::refreshInscriptionTable()
{
    if (this) {
        printf("Refreshing inscription table");
        priv->refreshInscriptionTable();
        // Q_EMIT layoutChanged();
    } else {
        printf("Inscriptiontable uninitialised.");
    }
}

QVariant InscriptionTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    InscriptionTableEntry *rec = static_cast<InscriptionTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case Inscription:
            return rec->inscription;
        case Date:
            return GUIUtil::dateTimeStr(rec->date);
        }
    }
    else if(role == InscriptionRole)
    {
        return priv->describe(rec);
    }
        
    return QVariant();
}

QVariant InscriptionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole && section < columns.size())
        {
            return columns[section];
        }
    }
    return QVariant();
}

QModelIndex InscriptionTableModel::index(int row, int column, const QModelIndex & parent) const
{
   Q_UNUSED(parent);
    InscriptionTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}
