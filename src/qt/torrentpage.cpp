// Copyright (c) 2015-2016 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "torrentpage.h"
#include "ui_torrentpage.h"
#include "torrenttablemodel.h"
#include "clientmodel.h"
#include "bitcoingui.h"
#include "guiutil.h"
#include "util.h"
#include "validation.h"
#include "net.h"
#include "transactiondescdialog.h"
#include "platformstyle.h"
#include "rpc/client.h"
#include "rpc/server.h"
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>


#include "ui_interface.h"
// #include "json/json_spirit_reader_template.h"
// #include "json/json_spirit_writer_template.h"
// #include "json/json_spirit_utils.h"


#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QPainter>

using namespace boost;
// using namespace json_spirit;

int getTorrentBlockCount()
{
    QSqlQuery query;
    query.exec(QString("select blockindex from blockindex"));
    if (query.next())
    {
        return query.value(0).toInt();
    }
    return 0;
}

void updateTorrent(int index)
{
    try{
        QString s = QString::number(index, 10);  
        std::vector<std::string> args;
        args.push_back("getblockhash");
        args.push_back(s.toStdString());
        JSONRPCRequest req;
        req.params = RPCConvertValues(args[0], std::vector<std::string>(args.begin() + 1, args.end()));
        req.strMethod = args[0];
        UniValue result = tableRPC.execute(req);

        if (result.isStr())
        {
            args.clear();
            args.push_back("getblock");
            JSONRPCRequest sreq;
            sreq.params = RPCConvertValues(args[0], std::vector<std::string>(args.begin() + 1, args.end()));
            sreq.strMethod = args[0];
            UniValue result = tableRPC.execute(sreq);
            args.push_back(result.get_str());

            if (result.isObject())
            {
                UniValue reply = result.get_obj();
                UniValue txid (UniValue::VARR);
                txid = find_value(reply, "tx").get_array();
                /*
                BOOST_FOREACH(const UniValue::VARR &v, txid)
                {
                    args.clear();
                    args.push_back("gettorrent");
                    args.push_back(v.get_str());
                    JSONRPCRequest jreq;
                    jreq.params = RPCConvertValues(args[0], std::vector<std::string>(args.begin() + 1, args.end()));
                    jreq.strMethod = args[0];
                    UniValue value = tableRPC.execute(jreq);
                    if (value.isObj())
                    {
                        UniValue reply (UniValue::VOBJ);
                        reply.setObj(value.get_obj());
                        QSqlQuery query;
                        query.exec(QString(
                              "insert into torrent values('%1','%2',%3)").arg(
                                 QString::fromStdString(find_value(reply, "title").get_str())).arg(
                                    QString::fromStdString(v.get_str())).arg(index));
                    }
                }
                */
            }
        }
        QSqlQuery query;
        query.exec(QString("UPDATE blockindex set blockindex = %1").arg(index));
    }
    catch (const UniValue& objError)
    {
        throw std::runtime_error("updateTorrent error");

    }
    catch(std::runtime_error& e) 
    {  
          throw std::runtime_error("updateTorrent error");
    }
    catch (std::exception& e)
    {
        throw std::runtime_error("updateTorrent error");
    }
}

class TDBExecutor: public QObject
{
    Q_OBJECT

    public Q_SLOTS:
        void start();

    Q_SIGNALS:
    void refreshTorrentTable();
};

#include "torrentpage.moc"

void TDBExecutor::start()
{
    try{
        int torrentblockcount = getTorrentBlockCount();
        int nBestHeight = chainActive.Height();
        while((torrentblockcount < nBestHeight))
        { 
            updateTorrent(torrentblockcount);
            torrentblockcount++;
        } 
    }catch(std::runtime_error& e) 
    {  
          Q_EMIT refreshTorrentTable();
    }
    Q_EMIT refreshTorrentTable();
}

TorrentPage::TorrentPage(const PlatformStyle *_platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TorrentPage),
    model(0)
{
    ui->setupUi(this);
    startExecutor();

}

TorrentPage::~TorrentPage()
{
    Q_EMIT stopExecutor();
    
    delete ui;
}

void TorrentPage::setModel(TorrentTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableView->setModel(model);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableView->horizontalHeader()->setResizeMode(TorrentTableModel::Title, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(TorrentTableModel::Address, QHeaderView::ResizeToContents);
#else
    ui->tableView->horizontalHeader()->setSectionResizeMode(TorrentTableModel::Title, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(TorrentTableModel::Address, QHeaderView::ResizeToContents);
#endif

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClicked(QModelIndex)));
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
}

void TorrentPage::setClientModel(ClientModel *_model)
{
    this->clientModel = _model;
    if(!model)
    {
       return;
    }
}

void TorrentPage::startExecutor()
{
    QThread* thread = new QThread;
    TDBExecutor *executor = new TDBExecutor();
    executor->moveToThread(thread);

    connect(executor, SIGNAL(refreshTorrentTable()), this, SLOT(refreshTorrentTable()));
    connect(thread, SIGNAL(started()), executor, SLOT(start()));
    connect(this, SIGNAL(stopExecutor()), executor, SLOT(deleteLater()));
    connect(this, SIGNAL(stopExecutor()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void TorrentPage::refreshTorrentTable()
{
    model->refreshTorrentTable();
}

void TorrentPage::showDetails()
{
    if(!ui->tableView->selectionModel())
        return;
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if(!selection.isEmpty())
    {
        // QString html = selection.at(0).data(TorrentTableModel::TorrentRole).toString();
        // TransactionDescDialog dlg(html);
        // dlg.exec();
        QModelIndex idx = selection.at(0);
        TransactionDescDialog dlg(idx, this);
        dlg.exec();
    }
}

void TorrentPage::searchButtonClicked()
{
   /* TODO: implement search for torrentModel */
}
