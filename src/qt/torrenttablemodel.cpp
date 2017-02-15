// Copyright (c) 2015-2016 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "torrenttablemodel.h"

#include "base58.h"
#include "rpc/server.h"
#include "rpc/client.h"
#include "guiutil.h"
#include "util.h"
#include "walletmodel.h"

#include <QFont>
#include <QColor>

#include "ui_interface.h"
// #include "json/json_spirit_reader_template.h"
// #include "json/json_spirit_writer_template.h"
// #include "json/json_spirit_utils.h"

#include <boost/algorithm/string.hpp>
#include <stdlib.h>

using namespace boost;
// using namespace json_spirit;

class TorrentTablePriv
{
public:

    QList<TorrentTableEntry> cachedTorrentTable;
    TorrentTableModel *parent;

    TorrentTablePriv(TorrentTableModel *parent):
         parent(parent) {}

    void getLastTorrent()
    {
        try{
            QSqlQuery query;
            query.exec(QString("select txid from torrent  order by blockindex desc limit 1000"));
            while (query.next())
            {
                std::vector<std::string> args;
                args.push_back("gettorrent");
                args.push_back(query.value(0).toString().toStdString());
                UniValue jsonstr = RPCConvertValues(
                            args[0], std::vector<std::string>(args.begin() + 1, args.end()));
                LogPrintf("%s", jsonstr.write());
                /*

                JSONRPCRequest req;
                req.params = RPCConvertValues(stack.back()[0], std::vector<std::string>(stack.back().begin() + 1, stack.back().end()));
                req.strMethod = stack.back()[0];
                lastResult = tableRPC.execute(req);


                UniValue value = tableRPC.execute(
                        args[0],
                        RPCConvertValues(
                            args[0],
                            std::vector<std::string>(args.begin() + 1, args.end())));
                if (value.type() == obj_type)
                {
                    Object reply = value.get_obj();

                    std::string  title  = find_value(reply, "title").get_str();
                    std::string  link  = find_value(reply, "link").get_str();
                    std::string  address  = find_value(reply, "address").get_str();
                    std::string  amount  = find_value(reply, "amount").get_str();

                    cachedTorrentTable.append(TorrentTableEntry(
                                      QString::fromStdString(title),
                                     QString::fromStdString(link),
                                     QString::fromStdString(address),
                                     QString::fromStdString(amount)));
                }
                */
            }
        /*
        }catch (json_spirit::Object& objError)
        {
        */
        }
        catch(std::runtime_error &) 
        {  

        }
        catch (std::exception& e)
        {

        }
    }

    void refreshTable()
    {
        cachedTorrentTable.clear();
        getLastTorrent();
    }

    int size()
    {
        return cachedTorrentTable.size();
    }

    TorrentTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedTorrentTable.size())
        {
            return &cachedTorrentTable[idx];
        }
        else
        {
            return 0;
        }
    }

    QString describe(TorrentTableEntry *rec)
    {

        QString strHTML;

        strHTML.reserve(4000);
        strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";
        strHTML += "<b>Title:</b> " + rec->title + "<br>";
        strHTML += "<b>Send Address:</b> " + rec->address + "<br>";
        strHTML += "<b>Amount:</b> " + rec->amount + "<br>";
        strHTML += "<b>Link:</b> " + rec->link + "<br>";
        return strHTML;


    }
};

TorrentTableModel::TorrentTableModel(WalletModel *parent) :
    QAbstractTableModel(parent),
    // priv(new TorrentTablePriv(this)),
    priv(0)
{
    columns << tr("Title");
    columns << tr("Address");
    columns << tr("Amount");
    priv = new TorrentTablePriv(this);

    /* TODO: check incoming blocks
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateExpiration()));
    timer->start(MODEL_UPDATE_DELAY);
    */
    // subscribeToCoreSignals();
}

TorrentTableModel::~TorrentTableModel()
{
    // unsubscribeFromCoreSignals();
    delete priv;
}

int TorrentTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int TorrentTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

void TorrentTableModel::refreshTorrentTable()
{
    // priv->refreshTable();
    // Q_EMIT layoutChanged();
}

QVariant TorrentTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TorrentTableEntry *rec = static_cast<TorrentTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case Title:
            return rec->title;
        case Address:
            return rec->address;
        case Amount:
            return rec->amount;
        }
    }
    else if(role == TorrentRole)
    {
        return priv->describe(rec);
    }
        
    return QVariant();
}

QVariant TorrentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
    }
    return QVariant();
}

QModelIndex TorrentTableModel::index(int row, int column, const QModelIndex & parent) const
{
   Q_UNUSED(parent);
    TorrentTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}
