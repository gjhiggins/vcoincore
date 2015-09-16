#include "tradingdialog.h"
#include "ui_tradingdialog.h"
#include <qmessagebox.h>
#include <qtimer.h>

#include <QHeaderView>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QDateTime>

#include <openssl/hmac.h>

#include <boost/lexical_cast.hpp>
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;
using namespace std;

//Coinbase API, latest BTC orice
const QString apiCoinbasePrice = "https://www.bitstamp.net/api/ticker/";

//Bleutrade API
const QString apiBleutradeMarketSummary = "https://bleutrade.com/api/v2/public/getmarketsummaries";
const QString apiBleutradeTrades = "https://bleutrade.com/api/v2/public/getmarkethistory?market=BTC_VCN";
const QString apiBleutradeOrders = "https://bleutrade.com/api/v2/public/getorderbook?market=VCN_BTC&type=ALL";

QString dequote(QString s) {
    string str(s.toStdString());
    sregex nums = sregex::compile( ":\\\"(-?\\d*(\\.\\d+))\\\"" );
    string nm( ":$1" );
    str = regex_replace( str, nums, nm );
    sregex tru = sregex::compile( "\\\"true\\\"" );
    string tr( "true" );
    str = regex_replace( str, tru, tr );
    sregex fal = sregex::compile( "\\\"false\\\"" );
    string fl( "false" );
    str = regex_replace( str, fal, fl );
    QString res = str.c_str();
    return res;
}

tradingDialog::tradingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tradingDialog)
{
    ui->setupUi(this);
    timerid = 0;
    // qDebug() <<  "Expected this";

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::green);

    InitTrading();

    ui->BuyCostLabel->setPalette(sample_palette);
    ui->SellCostLabel->setPalette(sample_palette);
    ui->VCNAvailableLabel->setPalette(sample_palette);
    ui->BtcAvailableLbl_2->setPalette(sample_palette);
    //Set tabs to inactive
    ui->TradingTabWidget->setTabEnabled(3,false);
    ui->TradingTabWidget->setTabEnabled(4,false);
    ui->TradingTabWidget->setTabEnabled(5,false);
    ui->TradingTabWidget->setTabEnabled(6,false);
    ui->TradingTabWidget->setTabEnabled(7,false);

    /*OrderBook Table Init*/
    CreateOrderBookTables(*ui->BidsTable,QStringList() << "Total(BTC)"<< "VCN(SIZE)" << "Bid(BTC)");
    CreateOrderBookTables(*ui->AsksTable,QStringList() << "Ask(BTC)"  << "VCN(SIZE)" << "Total(BTC)");
    /*OrderBook Table Init*/

    /*Market History Table Init*/
    ui->MarketHistoryTable->setColumnCount(5);
    ui->MarketHistoryTable->verticalHeader()->setVisible(false);

    ui->MarketHistoryTable->setHorizontalHeaderLabels(QStringList()<<"Date"<<"Buy/Sell"<<"Bid/Ask"<<"Total units(VCN)"<<"Total cost(BTC");
    ui->MarketHistoryTable->setRowCount(0);

    int Cellwidth =  ui->MarketHistoryTable->width() / 5;

    // ui->MarketHistoryTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    ui->MarketHistoryTable->horizontalHeader()->resizeSection(1,Cellwidth); // column 1, width 50
    ui->MarketHistoryTable->horizontalHeader()->resizeSection(2,Cellwidth);
    ui->MarketHistoryTable->horizontalHeader()->resizeSection(3,Cellwidth);
    ui->MarketHistoryTable->horizontalHeader()->resizeSection(4,Cellwidth);
    ui->MarketHistoryTable->horizontalHeader()->resizeSection(5,Cellwidth);
    ui->MarketHistoryTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->MarketHistoryTable->horizontalHeader()->setStyleSheet("QHeaderView::section, QHeaderView::section * {font-weight :bold;}");
    /*Market History Table Init*/

    /*Account History Table Init*/
    ui->TradeHistoryTable->setColumnCount(13);

    ui->TradeHistoryTable->setHorizontalHeaderLabels(QStringList() << "Date Time" << "Exchange" << "OrderType"  << "Limit" << "QTY" << "QTY_Rem" << "Price" << "PricePerUnit" <<
                                                     "Conditional" << "Condition" << "Condition Target" << "ImmediateOrCancel" << "Closed");
    ui->TradeHistoryTable->setRowCount(0);
    ui->TradeHistoryTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->TradeHistoryTable->horizontalHeader()->setStyleSheet("QHeaderView::section, QHeaderView::section * {font-weight :bold;}");
    ui->TradeHistoryTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    /*Account History Table Init*/

    /*Open Orders Table*/
    ui->OpenOrdersTable->setColumnCount(15);
    ui->OpenOrdersTable->setHorizontalHeaderLabels(QStringList() << "OrderId" << "Date Time" << "Exchange" << "OrderType"  << "Limit" << "QTY" << "QTY_Rem" << "Price" << "PricePerUnit" << "CancelInitiated" <<
                                                   "Conditional" << "Condition" << "Condition Target" << "ImmediateOrCancel" << "Cancel Order");
    ui->OpenOrdersTable->setRowCount(0);
    ui->OpenOrdersTable->setColumnHidden(0,true);
    ui->OpenOrdersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->OpenOrdersTable->horizontalHeader()->setStyleSheet("QHeaderView::section, QHeaderView::section * {font-weight :bold;}");

    ui->OpenOrdersTable->horizontalHeader()->resizeSection(1,120); // column 1, width 50
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(2,70);
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(3,70);
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(4,70);
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(5,70);
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(6,70);
    ui->OpenOrdersTable->horizontalHeader()->resizeSection(7,70);

    ui->OpenOrdersTable->setColumnHidden(9,true);
    ui->OpenOrdersTable->setColumnHidden(10,true);
    ui->OpenOrdersTable->setColumnHidden(11,true);
    ui->OpenOrdersTable->setColumnHidden(12,true);
    ui->OpenOrdersTable->setColumnHidden(13,true);

    //Create charts
    // ui->qCustomPlotBleutradeTrades->addGraph();
    ui->qCustomPlotBleutradeTrades->setBackground(QBrush(QColor("#edf1f7")));

    // create candlestick chart:
    ui->qCustomPlotBleutradeTrades->xAxis->setBasePen(Qt::NoPen);
    ui->qCustomPlotBleutradeTrades->xAxis->setTickLabels(false);
    ui->qCustomPlotBleutradeTrades->xAxis->setTicks(false); // only want vertical grid in main axis rect, so hide xAxis backbone, ticks, and labels
    ui->qCustomPlotBleutradeTrades->xAxis->setAutoTickStep(false);
    ui->qCustomPlotBleutradeTrades->xAxis->setTickStep(3600); // 1 hour day tickstep
    ui->qCustomPlotBleutradeTrades->rescaleAxes();
    ui->qCustomPlotBleutradeTrades->xAxis->scaleRange(1.025, ui->qCustomPlotBleutradeTrades->xAxis->range().center());
    ui->qCustomPlotBleutradeTrades->yAxis->scaleRange(1.1, ui->qCustomPlotBleutradeTrades->yAxis->range().center());


    // Order Depth Graph
    ui->qCustomPlotBleutradeOrderDepth->setBackground(QBrush(QColor("#edf1f7")));

    //One time primer
    // pollAPIs();
    // QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseNetworkResponse(QNetworkReply*)), Qt::AutoConnection);

    connect (ui->OpenOrdersTable, SIGNAL(cellClicked(int,int)), this, SLOT(CancelOrderSlot(int, int)));
    /*Open Orders Table*/

    /*populate static combo values*/
    ui->BuyBidcomboBox   -> addItems(QStringList()<<"Last"<<"Bid"<<"Ask");
    ui->buyOrdertypeCombo-> addItems(QStringList()<<"Limit"<<"Market");
    ui->SellBidcomboBox  -> addItems(QStringList()<<"Last"<<"Bid"<<"Ask");
    ui->SellOrdertypeCombo-> addItems(QStringList()<<"Limit"<<"Market");
    //ui->BuyTimeInForceCombo-> addItems(QStringList()<<"Good 'Til Cancelled"<<"Immediate Or Cancel");
    //ui->BuyConditionCombo->   addItems(QStringList()<<"Greater Than Or Equal To"<<"Less Than Or Equal To");
    //ui->BuyConditionCombo->hide();
    //ui->BuyWhenPriceLabel->hide();
    //ui->ConditionLineEdit->hide();
    /*populate static combo values*/
}

void tradingDialog::InitTrading()
{
    //todo - add internet connection/socket error checking.

    //Get default exchange info for the qlabels
    UpdaterFunction();
    // qDebug() << "Updater called";
    if (this->timerid == 0)
        {
            //Timer is not set,lets create one.
            this->timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(UpdaterFunction()));
            this->timer->start(50000);
            this->timerid = this->timer->timerId();
        }
}

void tradingDialog::UpdaterFunction()
{
    //First get the main exchange info in order to populate qLabels in maindialog. then get data
    //required for the current tab.

    int Retval = SetExchangeInfoTextLabels();

    if (Retval == 0)
    {
        ActionsOnSwitch(-1);
    }
}

QString tradingDialog::GetNonce()
{
    // There must be a better way, lol.
    QString str = "";
    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 nonce = currentDateTime.currentMSecsSinceEpoch();
    QString Response = str.setNum(nonce / 1000);
    return Response;
}

QString tradingDialog::GetCandles() {
    /*
    /public/getcandles
    Obtains candles format historical trades of a specific market.
    Required parameters:
    market
    period (1m, 2m, 3m, 4m, 5m, 6m, 10m, 12m, 15m, 20m, 30m, 1h, 2h, 3h, 4h, 6h, 8h, 12h, 1d)
    count (default: 1000, max: 999999)
    lasthours (default:24, max: 720)

        {
            "success":"true",
            "message":"",
            "result":[{
              "TimeStamp":"2014-07-31 10:15:00",
              "Open":"0.00000048",
              "High":"0.00000050",
              "Low":"0.00000048",
              "Close":"0.00000049",
              "Volume":"594804.73036048",
              "BaseVolume":"0.11510368"
            },{
              "TimeStamp":"2014-07-31 10:00:00",
              "Open":"0.00000037",
              "High":"0.00000048",
              "Low":"0.00000035",
              "Close":"0.00000048",
              "Volume":"623490.14936407",
              "BaseVolume":"0.13101303"
            }
            ]
        }
    */
    QString Response = sendRequest("https://bleutrade.com/api/v2/public/getcandles?market=VCN_BTC&period=1h&count=50&lasthours=720");
    return dequote(Response);
}

QString tradingDialog::GetMarketSummary()
{

    /*
        {"success":"true",
         "message":"",
         "result":[
             {"MarketName":"VCN_BTC",
              "PrevDay":"0.00000010",
              "High":"0.00000014",
              "Low":"0.00000010",
              "Last":"0.00000014",
              "Average":"0.00000012",
              "Volume":"25182.01003446",
              "BaseVolume":"0.00293166",
              "TimeStamp":"2015-06-05 11:23:56",
              "Bid":"0.00000008",
              "Ask":"0.00000014",
              "IsActive":"true"
             }
         ]
        }
    */

    QString Response = sendRequest("https://bleutrade.com/api/v2/public/getmarketsummary?market=VCN_BTC");
    return dequote(Response);
}

QString tradingDialog::GetMarketHistory()
{
    /*
    {"success":"true",
     "message":"",
     "result":[
         {"TimeStamp":"2015-06-05 07:13:51",
          "Quantity":"859.92857142",
          "Price":"0.00000014",
          "Total":"0.00012039",
          "OrderType":"BUY"},
         {"TimeStamp":"2015-06-02 19:57:31",
          "Quantity":"5221.19923810",
          "Price":"0.00000012",
          "Total":"0.00062654",
          "OrderType":"SELL"}
       ]
    }
    */

    QString Response = sendRequest("https://bleutrade.com/api/v2/public/getmarkethistory?market=VCN_BTC");
    return dequote(Response);
}

QString tradingDialog::GetOrderBook()
{
    /*
    {"success":"true",
     "message":"",
     "result":
         {"buy":[
              {"Quantity":"20000.0","Rate":"0.0000001"},
              {"Quantity":"14000.0","Rate":"0.00000008"},
              {"Quantity":"14000.0","Rate":"0.00000007"},
              {"Quantity":"20000.0","Rate":"0.00000004"},
              {"Quantity":"4000.0","Rate":"0.00000003"},
              {"Quantity":"178616.99999999","Rate":"0.00000002"},
              {"Quantity":"10000.0","Rate":"0.00000001"}
          ],
          "sell":[
              {"Quantity":"50573.35015995","Rate":"0.00000014"},
              {"Quantity":"10049.60505143","Rate":"0.00000015"},
              {"Quantity":"18247.94231398","Rate":"0.00000017"},
              {"Quantity":"20233.34882011","Rate":"0.00000018"},
              {"Quantity":"3187.34960345","Rate":"0.0000002"},
              {"Quantity":"3051.52857223","Rate":"0.0000003"},
              {"Quantity":"335.00211363","Rate":"0.00000032"},
              {"Quantity":"335.00211363","Rate":"0.00000034"},
              {"Quantity":"11277.50211664","Rate":"0.00000035"},
              {"Quantity":"1232.00211363","Rate":"0.00000036"},
              {"Quantity":"1434.00211363","Rate":"0.00000037"},
              {"Quantity":"305.00211363","Rate":"0.00000038"},
              {"Quantity":"305.00211363","Rate":"0.00000039"},
              {"Quantity":"23028.83530067","Rate":"0.0000004"},
              {"Quantity":"250.00211363","Rate":"0.00000041"},
              {"Quantity":"250.00211363","Rate":"0.00000042"},
              {"Quantity":"1355.39946248","Rate":"0.00000043"},
              {"Quantity":"482.04346889","Rate":"0.00000044"},
              {"Quantity":"250.00211363","Rate":"0.00000062"},
              {"Quantity":"155.00211363","Rate":"0.00000065"}
          ]
       }
    }
    */
    QString  Response = sendRequest("https://bleutrade.com/api/v2/public/getorderbook?market=VCN_BTC&type=ALL");
    return dequote(Response);
}

QString tradingDialog::GetBalance(QString Currency)
{

    QString URL = "https://bleutrade.com/api/v2/account/getbalance?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&currency=";
            URL += Currency;
    // QMessageBox::information(this,"URL",URL);

    QString Response = sendRequest(URL);
     return dequote(Response);
}

QString tradingDialog::GetOpenOrders()
{
    QString URL = "https://bleutrade.com/api/v2/market/getopenorders?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&market=VCN_BTC";

    QString Response = sendRequest(URL);
    return dequote(Response);
}

QString tradingDialog::BuyVCN(QString OrderType, double Quantity, double Rate)
{

    QString str = "";
    QString URL = "https://bleutrade.com/api/v2/market/";
            URL += OrderType;
            URL += "?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&market=VCN&quantity=";
            URL += str.number(Quantity,'i',8);
            URL += "&rate=";
            URL += str.number(Rate,'i',8);

    QString Response = sendRequest(URL);
    return dequote(Response);
}

QString tradingDialog::SellVCN(QString OrderType, double Quantity, double Rate)
{

    QString str = "";
    QString URL = "https://bleutrade.com/api/v2/market/";
            URL += OrderType;
            URL += "?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&market=VCN&quantity=";
            URL += str.number(Quantity,'i',8);
            URL += "&rate=";
            URL += str.number(Rate,'i',8);

    QString Response = sendRequest(URL);
    return dequote(Response);
}

QString tradingDialog::CancelOrder(QString OrderId)
{
    QString URL = "https://bleutrade.com/api/v2/market/cancel?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL +="&uuid=";
            URL += OrderId;

    QString Response = sendRequest(URL);
    return dequote(Response);
}

QString tradingDialog::GetDepositAddress()
{

    QString URL = "https://bleutrade.com/api/v2/account/getdepositaddress?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&currency=VCN";

    QString Response = sendRequest(URL);
    return dequote(Response);
}

QString tradingDialog::GetAccountHistory()
{

    QString URL = "https://bleutrade.com/api/v2/account/getorderhistory?apikey=";
            URL += this->ApiKey;
            URL += "&nonce=",
            URL += tradingDialog::GetNonce(),
            URL += "&market=VCN_BTC";

    QString Response = sendRequest(URL);
    return dequote(Response);
}

int tradingDialog::SetExchangeInfoTextLabels()
{
    //Get the current exchange information + information for the current open tab if required.
    QString str = "";
    QString Response = GetMarketSummary();

    //Set the labels, parse the json result to get values.
    QJsonObject obj = GetResultObjectFromJSONArray(Response);

    //set labels to richtext to use css.
    ui->Bid->setTextFormat(Qt::RichText);
    ui->Ask->setTextFormat(Qt::RichText);
    ui->volumet->setTextFormat(Qt::RichText);
    ui->volumebtc->setTextFormat(Qt::RichText);

    ui->Ask->setText("<b>Ask:</b> <span style='font-weight:bold; font-size:11px; color:Red'>" + str.number(obj["Ask"].toDouble(),'i',8) + "</span> BTC");

    ui->Bid->setText("<b>Bid:</b> <span style='font-weight:bold; font-size:11px; color:Green;'>" + str.number(obj["Bid"].toDouble(),'i',8) + "</span> BTC");

    ui->volumet->setText("<b>VCN Volume:</b> <span style='font-weight:bold; font-size:11px; color:blue;'>" + str.number(obj["Volume"].toDouble(),'i',8) + "</span> VCN");

    ui->volumebtc->setText("<b>BTC Volume:</b> <span style='font-weight:bold; font-size:11px; color:blue;'>" + str.number(obj["BaseVolume"].toDouble(),'i',8) + "</span> BTC");

    obj.empty();

    return 0;
}

void tradingDialog::CreateOrderBookTables(QTableWidget& Table,QStringList TableHeader)
{

    Table.setColumnCount(3);
    Table.verticalHeader()->setVisible(false);

    Table.setHorizontalHeaderLabels(TableHeader);

    int Cellwidth =  Table.width() / 3;

    Table.horizontalHeader()->resizeSection(1,Cellwidth); // column 1, width 50
    Table.horizontalHeader()->resizeSection(2,Cellwidth);
    Table.horizontalHeader()->resizeSection(3,Cellwidth);

    Table.setRowCount(0);

    // Table.horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    Table.horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    Table.horizontalHeader()->setStyleSheet("QHeaderView::section, QHeaderView::section * { font-weight :bold;}");
}

void tradingDialog::DisplayBalance(QLabel &BalanceLabel,QLabel &Available, QLabel &Pending, QString Currency,QString Response)
{

    QString str;

    BalanceLabel.setTextFormat(Qt::RichText);
    Available.setTextFormat(Qt::RichText);
    Pending.setTextFormat(Qt::RichText);

    //Set the labels, parse the json result to get values.
    QJsonObject ResultObject = GetResultObjectFromJSONObject(Response);

    BalanceLabel.setText("<span style='font-weight:bold; font-size:11px; color:green'>" + str.number( ResultObject["Balance"].toDouble(),'i',8) + "</span> " + Currency);
    Available.setText("<span style='font-weight:bold; font-size:11px; color:green'>" + str.number( ResultObject["Available"].toDouble(),'i',8) + "</span> " +Currency);
    Pending.setText("<span style='font-weight:bold; font-size:11px; color:green'>" + str.number( ResultObject["Pending"].toDouble(),'i',8) + "</span> " +Currency);
}

void tradingDialog::ParseAndPopulateOpenOrdersTable(QString Response)
{

    int itteration = 0, RowCount = 0;

    QJsonArray jsonArray = GetResultArrayFromJSONObject(Response);
    QJsonObject obj;

    ui->OpenOrdersTable->setRowCount(0);

    Q_FOREACH (const QJsonValue & value, jsonArray)
        {
            QString str = "";
            obj = value.toObject();

            RowCount = ui->OpenOrdersTable->rowCount();

            QString ios;
            QString IsConditional;
            QString ConditionTarget;
            QString CancelInitiated;

            obj["ImmediateOrCancel"].toBool() == true ? (ios = "true")             : ( ios = "false");
            obj["IsConditional"].toBool()     == true ? (IsConditional = "true")   : ( IsConditional = "false");
            obj["ConditionTarget"].toBool()   == true ? (ConditionTarget = "true") : ( ConditionTarget = "false");
            obj["CancelInitiated"].toBool()   == true ? (CancelInitiated = "true") : ( CancelInitiated = "false");

            ui->OpenOrdersTable->insertRow(RowCount);
            ui->OpenOrdersTable->setItem(itteration, 0, new QTableWidgetItem(obj["OrderUuid"].toString()));
            ui->OpenOrdersTable->setItem(itteration, 1, new QTableWidgetItem(BittrexTimeStampToReadable(obj["Opened"].toString())));
            ui->OpenOrdersTable->setItem(itteration, 2, new QTableWidgetItem(obj["Exchange"].toString()));
            ui->OpenOrdersTable->setItem(itteration, 3, new QTableWidgetItem(obj["OrderType"].toString()));
            ui->OpenOrdersTable->setItem(itteration, 4, new QTableWidgetItem(str.number(obj["Limit"].toDouble(),'i',8)));
            ui->OpenOrdersTable->setItem(itteration, 5, new QTableWidgetItem(str.number(obj["Quantity"].toDouble(),'i',8)));
            ui->OpenOrdersTable->setItem(itteration, 6, new QTableWidgetItem(str.number(obj["QuantityRemaining"].toDouble(),'i',8)));
            ui->OpenOrdersTable->setItem(itteration, 7, new QTableWidgetItem(str.number(obj["Price"].toDouble(),'i',8)));
            ui->OpenOrdersTable->setItem(itteration, 8, new QTableWidgetItem(str.number(obj["PricePerUnit"].toDouble(),'i',8)));
            ui->OpenOrdersTable->setItem(itteration, 9, new QTableWidgetItem(CancelInitiated));
            ui->OpenOrdersTable->setItem(itteration, 10, new QTableWidgetItem(IsConditional));
            ui->OpenOrdersTable->setItem(itteration, 11, new QTableWidgetItem(obj["Condition"].toString()));
            ui->OpenOrdersTable->setItem(itteration, 12, new QTableWidgetItem(ConditionTarget));
            ui->OpenOrdersTable->setItem(itteration, 13, new QTableWidgetItem(ios));
            ui->OpenOrdersTable->setItem(itteration, 14, new QTableWidgetItem(tr("Cancel Order")));

            //Handle the cancel link in open orders table
            QTableWidgetItem* CancelCell;
            CancelCell= ui->OpenOrdersTable->item(itteration, 14);    //Set the wtablewidget item to the cancel cell item.
            CancelCell->setForeground(QColor::fromRgb(255,0,0));      //make this item red.
            CancelCell->setTextAlignment(Qt::AlignCenter);
            }
}

void tradingDialog::CancelOrderSlot(int row, int col)
{

   QString OrderId = ui->OpenOrdersTable->model()->data(ui->OpenOrdersTable->model()->index(row,0)).toString();
   QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this,"Cancel Order","Are you sure you want to cancel the order?",QMessageBox::Yes|QMessageBox::No);

   if (reply == QMessageBox::Yes) {

          QString Response = CancelOrder(OrderId);

          QJsonDocument jsonResponse = QJsonDocument::fromJson(Response.toUtf8());
          QJsonObject ResponseObject = jsonResponse.object();

              if (ResponseObject["success"].toBool() == false){

                            QMessageBox::information(this,"Failed To Cancel Order",ResponseObject["message"].toString());

                  }else if (ResponseObject["success"].toBool() == true){
                             ui->OpenOrdersTable->model()->removeRow(row);
                             QMessageBox::information(this,"Success","You're order was cancelled.");
                  }
   } else {
           // qDebug() << "Do Nothing";
          }
}

void tradingDialog::ParseAndPopulateAccountHistoryTable(QString Response)
{

    int itteration = 0, RowCount = 0;

    QJsonArray jsonArray   = GetResultArrayFromJSONObject(Response);
    QJsonObject obj;

    ui->TradeHistoryTable->setRowCount(0);

    Q_FOREACH (const QJsonValue & value, jsonArray)
        {
            QString str = "";
            obj = value.toObject();

            RowCount = ui->TradeHistoryTable->rowCount();

            QString ios;
            QString IsConditional;
            QString ConditionTarget;

            obj["ImmediateOrCancel"].toBool() == true ? (ios = "true")             : ( ios = "false");
            obj["IsConditional"].toBool()     == true ? (IsConditional = "true")   : ( IsConditional = "false");
            obj["ConditionTarget"].toBool()   == true ? (ConditionTarget = "true") : ( ConditionTarget = "false");

            ui->TradeHistoryTable->insertRow(RowCount);
            ui->TradeHistoryTable->setItem(itteration, 0, new QTableWidgetItem(BittrexTimeStampToReadable(obj["TimeStamp"].toString())));
            ui->TradeHistoryTable->setItem(itteration, 1, new QTableWidgetItem(obj["Exchange"].toString()));
            ui->TradeHistoryTable->setItem(itteration, 2, new QTableWidgetItem(obj["OrderType"].toString()));
            ui->TradeHistoryTable->setItem(itteration, 3, new QTableWidgetItem(str.number(obj["Limit"].toDouble(),'i',8)));
            ui->TradeHistoryTable->setItem(itteration, 4, new QTableWidgetItem(str.number(obj["Quantity"].toDouble(),'i',8)));
            ui->TradeHistoryTable->setItem(itteration, 5, new QTableWidgetItem(str.number(obj["QuantityRemaining"].toDouble(),'i',8)));
            ui->TradeHistoryTable->setItem(itteration, 6, new QTableWidgetItem(str.number(obj["Price"].toDouble(),'i',8)));
            ui->TradeHistoryTable->setItem(itteration, 7, new QTableWidgetItem(str.number(obj["PricePerUnit"].toDouble(),'i',8)));
            ui->TradeHistoryTable->setItem(itteration, 8, new QTableWidgetItem(IsConditional));
            ui->TradeHistoryTable->setItem(itteration, 9, new QTableWidgetItem(obj["Condition"].toString()));
            ui->TradeHistoryTable->setItem(itteration, 10, new QTableWidgetItem(ConditionTarget));
            ui->TradeHistoryTable->setItem(itteration, 11, new QTableWidgetItem(ios));
            ui->TradeHistoryTable->setItem(itteration, 12, new QTableWidgetItem(obj["Closed"].toString()));
            itteration++;
        }

        obj.empty();
}

void tradingDialog::ParseAndPopulateOrderBookTables(QString OrderBook)
{

    QString str;
    QJsonObject obj;
    QJsonObject ResultObject = GetResultObjectFromJSONObject(OrderBook);

    int BuyItteration = 0,SellItteration = 0, BidRows = 0, AskRows = 0;

    QJsonArray  BuyArray  = ResultObject.value("buy").toArray();                //get buy/sell object from result object
    QJsonArray  SellArray = ResultObject.value("sell").toArray();               //get buy/sell object from result object

    double VCNSupply = 0;
    double VCNDemand = 0;
    double BtcSupply  = 0;
    double BtcDemand  = 0;

    ui->AsksTable->setRowCount(0);

    Q_FOREACH (const QJsonValue & value, SellArray)
    {
        obj = value.toObject();

        double x = obj["Rate"].toDouble(); //would like to use int64 here
        double y = obj["Quantity"].toDouble();
        double a = (x * y);

        VCNSupply = VCNSupply + y;
        BtcSupply  = BtcSupply  + a;

        AskRows = ui->AsksTable->rowCount();
        ui->AsksTable->insertRow(AskRows);

        ui->AsksTable->setItem(SellItteration, 0, new QTableWidgetItem(str.number(x,'i',8)));
        ui->AsksTable->setItem(SellItteration, 1, new QTableWidgetItem(str.number(y,'i',8)));
        ui->AsksTable->setItem(SellItteration, 2, new QTableWidgetItem(str.number(a,'i',8)));
        SellItteration++;
     }

    ui->BidsTable->setRowCount(0);

    Q_FOREACH (const QJsonValue & value, BuyArray)
    {
        obj = value.toObject();

        double x = obj["Rate"].toDouble(); //would like to use int64 here
        double y = obj["Quantity"].toDouble();
        double a = (x * y);

        VCNDemand = VCNDemand + y;
        BtcDemand  = BtcDemand  + a;

        BidRows = ui->BidsTable->rowCount();
        ui->BidsTable->insertRow(BidRows);
        ui->BidsTable->setItem(BuyItteration, 0, new QTableWidgetItem(str.number(a,'i',8)));
        ui->BidsTable->setItem(BuyItteration, 1, new QTableWidgetItem(str.number(y,'i',8)));
        ui->BidsTable->setItem(BuyItteration, 2, new QTableWidgetItem(str.number(x,'i',8)));
        BuyItteration++;
     }

        ui->VCNSupply->setText("<b>Supply:</b> <span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(VCNSupply,'i',8) + "</span><b> VCN</b>");
        ui->BtcSupply->setText("<span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(BtcSupply,'i',8) + "</span><b> BTC</b>");
        ui->AsksCount->setText("<b>Ask's :</b> <span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(ui->AsksTable->rowCount()) + "</span>");


        ui->VCNDemand->setText("<b>Demand:</b> <span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(VCNDemand,'i',8) + "</span><b> VCN</b>");
        ui->BtcDemand->setText("<span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(BtcDemand,'i',8) + "</span><b> BTC</b>");
        ui->BidsCount->setText("<b>Bid's :</b> <span style='font-weight:bold; font-size:11px; color:blue'>" + str.number(ui->BidsTable->rowCount()) + "</span>");
  obj.empty();
}

void tradingDialog::ParseAndPopulateMarketHistoryTable(QString Response)
{

    int counter = 0, RowCount = 0;
    QJsonArray jsonArray = GetResultArrayFromJSONObject(Response);
    QJsonObject obj;

    ui->MarketHistoryTable->setRowCount(0);

    Q_FOREACH (const QJsonValue & value, jsonArray)
        {
            QString str = "";
            obj = value.toObject();

            RowCount = ui->MarketHistoryTable->rowCount();

            ui->MarketHistoryTable->insertRow(RowCount);
            ui->MarketHistoryTable->setItem(counter, 0, new QTableWidgetItem(BittrexTimeStampToReadable(obj["TimeStamp"].toString())));
            ui->MarketHistoryTable->setItem(counter, 1, new QTableWidgetItem(obj["OrderType"].toString()));
            ui->MarketHistoryTable->setItem(counter, 2, new QTableWidgetItem(str.number(obj["Price"].toDouble(),'i',8)));
            ui->MarketHistoryTable->setItem(counter, 3, new QTableWidgetItem(str.number(obj["Quantity"].toDouble(),'i',8)));
            ui->MarketHistoryTable->setItem(counter, 4, new QTableWidgetItem(str.number(obj["Total"].toDouble(),'i',8)));
            ui->MarketHistoryTable->item(counter,1)->setBackgroundColor((obj["OrderType"] == QStringLiteral("BUY")) ? (QColor (0, 205, 0, 127)) : ( QColor (255, 99, 71, 127)));
            counter++;
        }
       obj.empty();
}

void tradingDialog::ParseAndPopulateOHLCChart(QString Response)
    {
    QJsonArray jsonArray = GetResultArrayFromJSONObject(Response);
    QJsonObject obj;
    double counter = 0;
    // double trades = jsonArray.count();
    double binSize = 3600*24; // bin data in 1 day intervals
    // QVector<double> tV(trades), oV(trades), hV(trades), lV(trades), cV(trades), nV(trades), pV(trades);

    // create candlestick chart:
    QCPFinancial *candlesticks = new QCPFinancial(ui->qCustomPlotBleutradeTrades->xAxis, ui->qCustomPlotBleutradeTrades->yAxis);
    // create ohlc chart:
    // QCPFinancial *ohlc = new QCPFinancial(ui->qCustomPlotBleutradeTrades->xAxis, ui->qCustomPlotBleutradeTrades->yAxis);

    // create bottom axis rect for volume bar chart:
    QCPAxisRect *volumeAxisRect = new QCPAxisRect(ui->qCustomPlotBleutradeTrades);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));

    Q_FOREACH (const QJsonValue & value, jsonArray)
    {
        QString str = "";
        obj = value.toObject();

        // QString datestamp = BittrexTimeStampToReadable(obj["TimeStamp"].toString());
        // double b = obj["BaseVolume"].toDouble();
        double datetimestamp = BittrexTimeStampToSeconds(obj["TimeStamp"].toString());
        double o = obj["Open"].toDouble();
        double h = obj["High"].toDouble();
        double l = obj["Low"].toDouble();
        double c = obj["Close"].toDouble();
        double v = obj["Volume"].toDouble();
        candlesticks->addData(QCPFinancialData(datetimestamp, o, h, l, c));
        // ohlc->addData(QCPFinancialData(datetimestamp, o, h, l, c));
        (v < 0 ? volumePos : volumeNeg)->addData(QCPBarData(datetimestamp, qAbs(v))); // add data to either volumeNeg or volumePos, depending on sign of v
        counter++;
    }
    obj.empty();

    // QCPFinancialDataMap trades = QCPFinancial::timeSeriesToOhlc(timeV, tradesV, binSize, startTime);
    // candlesticks->setData(&trades, true);

    // QCPFinancialDataMap ohlcdata = QCPFinancial::timeSeriesToOhlc(timeV, ohlcV, binSize/3.0, startTime); // divide binSize by 3 just to make the ohlc bars a bit denser
    // ohlc->setData(&ohlcdata, true);

    ui->qCustomPlotBleutradeTrades->addPlottable(candlesticks);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    candlesticks->setTwoColored(true);
    // candlesticks->setBrushPositive(QColor(245, 245, 245));
    // candlesticks->setBrushNegative(QColor(0, 0, 0));
    candlesticks->setBrushPositive(QColor(15, 208, 32));
    candlesticks->setBrushNegative(QColor(245, 40, 40));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));
    candlesticks->setWidth(binSize*0.1);

    /*
    ui->qCustomPlotBleutradeTrades->addPlottable(ohlc);
    ohlc->setName("OHLC");
    ohlc->setChartStyle(QCPFinancial::csOhlc);
    ohlc->setTwoColored(true);
    ohlc->setWidth(binSize*0.2);
    */
    ui->qCustomPlotBleutradeTrades->setAutoAddPlottableToLegend(false);
    ui->qCustomPlotBleutradeTrades->addPlottable(volumePos);
    ui->qCustomPlotBleutradeTrades->addPlottable(volumeNeg);
    volumePos->setWidth(3600*1.5);
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(15, 208, 32));
    volumeNeg->setWidth(3600*1.5);
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(245, 40, 40));

    // create bottom axis rect for volume bar chart:
    ui->qCustomPlotBleutradeTrades->plotLayout()->addElement(1, 0, volumeAxisRect);
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    // bring bottom and main axis rect closer together:
    ui->qCustomPlotBleutradeTrades->plotLayout()->setRowSpacing(0);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));

    // interconnect x axis ranges of main and bottom axis rects:
    connect(ui->qCustomPlotBleutradeTrades->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), ui->qCustomPlotBleutradeTrades->xAxis, SLOT(setRange(QCPRange)));
    // configure axes of both main and bottom axis rect:
    volumeAxisRect->axis(QCPAxis::atBottom)->setAutoTickStep(false);
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickStep(3600*24); // 1 day tickstep
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltDateTime);
    volumeAxisRect->axis(QCPAxis::atBottom)->setDateTimeSpec(Qt::UTC);
    volumeAxisRect->axis(QCPAxis::atBottom)->setDateTimeFormat("dd MMM");
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(35);
    volumeAxisRect->axis(QCPAxis::atLeft)->setAutoTickCount(3);

    // make axis rects' left side line up:
    QCPMarginGroup *group = new QCPMarginGroup(ui->qCustomPlotBleutradeTrades);
    ui->qCustomPlotBleutradeTrades->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);

}

void tradingDialog::ParseAndPopulateDepthChart(QString Response)
{
    double high = 0;
    double low = 100000;
    double sumBuys = 0;
    double sumSells = 0;
    double sumHighest = 0;
    QJsonObject obj;
    QString str;
    QJsonObject ResultObject = GetResultObjectFromJSONObject(Response);

    int buyi = 0, selli = 0;

    QJsonArray  BuyArray  = ResultObject.value("buy").toArray();                //get buy/sell object from result object
    QJsonArray  SellArray = ResultObject.value("sell").toArray();               //get buy/sell object from result object

    int nsells = SellArray.count();
    int nbuys = BuyArray.count();
    QVector<double> xAxisBuys(nbuys), yAxisBuys(nbuys);
    QVector<double> xAxisSells(nsells), yAxisSells(nsells);
    double srate = 0, brate = 0, bquantity = 0, squantity = 0;
    Q_FOREACH (const QJsonValue & value, SellArray)
    {
        obj = value.toObject();

        srate = obj["Rate"].toDouble(); //would like to use int64 here
        squantity = obj["Quantity"].toDouble();

        sumSells += squantity;
        xAxisSells[selli] = srate * 100000000;
        yAxisSells[selli] = sumSells;

        high = srate > high ? srate : high;
        low = srate < low ? srate : low;
        selli++;
     }
    obj.empty();

    Q_FOREACH (const QJsonValue & value, BuyArray)
    {
        obj = value.toObject();

        brate = obj["Rate"].toDouble(); //would like to use int64 here
        bquantity = obj["Quantity"].toDouble();

        sumBuys += bquantity;
        xAxisBuys[buyi] = brate * 100000000;
        yAxisBuys[buyi] = sumBuys;

        high = brate > high ? brate : high;
        low = brate < low ? brate : low;
        buyi++;
     }
    obj.empty();

    high *=  100000000;
    low *=  100000000;

    sumHighest = sumBuys > sumSells ? sumBuys : sumBuys < sumSells ? sumSells : sumBuys;
    ui->qCustomPlotBleutradeOrderDepth->addGraph();
    ui->qCustomPlotBleutradeOrderDepth->addGraph();
    ui->qCustomPlotBleutradeOrderDepth->graph(0)->setData(xAxisBuys, yAxisBuys);
    ui->qCustomPlotBleutradeOrderDepth->graph(1)->setData(xAxisSells, yAxisSells);
    ui->qCustomPlotBleutradeOrderDepth->graph(0)->setPen(QPen(QColor(34, 177, 76)));
    ui->qCustomPlotBleutradeOrderDepth->graph(0)->setBrush(QBrush(QColor(34, 177, 76, 20)));
    ui->qCustomPlotBleutradeOrderDepth->graph(1)->setPen(QPen(QColor(237, 24, 35)));
    ui->qCustomPlotBleutradeOrderDepth->graph(1)->setBrush(QBrush(QColor(237, 24, 35, 20)));
    ui->qCustomPlotBleutradeOrderDepth->xAxis->setRange(low, high);
    ui->qCustomPlotBleutradeOrderDepth->yAxis->setRange(low, sumHighest);
    ui->qCustomPlotBleutradeOrderDepth->replot();
}

void tradingDialog::ActionsOnSwitch(int index = -1) {

    QString Response = "";

    if (index == -1)
       index = ui->TradingTabWidget->currentIndex();

    switch (index) {
        case 0: // Chart tab is the current tab - update the info
               Response = GetCandles();
               if(Response.size() > 0 && Response != "Error"){
                  ParseAndPopulateOHLCChart(Response);
               }
               Response = GetOrderBook();
               if(Response.size() > 0 && Response != "Error"){
                  ParseAndPopulateDepthChart(Response);
               }
        break;

        case 1: // Order book tab is the current tab - update the info
               Response = GetOrderBook();
               if(Response.size() > 0 && Response != "Error"){
                  ParseAndPopulateOrderBookTables(Response);
               }
        break;

        case 2: // Market history tab
               Response = GetMarketHistory();
                   if(Response.size() > 0 && Response != "Error"){
                      ParseAndPopulateMarketHistoryTable(Response);
                 }
        break;

        case 3: // Open orders tab
               Response = GetOpenOrders();
                 if(Response.size() > 0 && Response != "Error"){
                    ParseAndPopulateOpenOrdersTable(Response);
                 }
        break;

        case 4: // Account history tab
               Response = GetAccountHistory();
                 if(Response.size() > 0 && Response != "Error"){
                    ParseAndPopulateAccountHistoryTable(Response);
                 }
        break;

        case 5: // Buy tab is active
               Response = GetMarketSummary();
               if(Response.size() > 0 && Response != "Error"){

                   QString balance = GetBalance("BTC");

                   QString str;
                   QJsonObject ResultObject =  GetResultObjectFromJSONObject(balance);

                   ui->BtcAvailableLbl->setText(str.number(ResultObject["Available"].toDouble(),'i',8));
                 }
        break;

        case 6: // Sell tab is active
               Response = GetMarketSummary();
               if(Response.size() > 0 && Response != "Error"){

                   QString balance = GetBalance("VCN");
                   QString str;
                   QJsonObject ResultObject =  GetResultObjectFromJSONObject(balance);

                   ui->VCNAvailableLabel->setText(str.number(ResultObject["Available"].toDouble(),'i',8));
                 }
        break;

        case 7: // Show balance tab
               Response = GetBalance("BTC");
               if(Response.size() > 0 && Response != "Error"){
                   DisplayBalance(*ui->BitcoinBalanceLabel,*ui->BitcoinAvailableLabel,*ui->BitcoinPendingLabel, QString::fromUtf8("BTC"),Response);
               }

               Response = GetBalance("VCN");

               if(Response.size() > 0 && Response != "Error"){
                   DisplayBalance(*ui->VCNBalanceLabel,*ui->VCNAvailableLabel,*ui->VCNPendingLabel, QString::fromUtf8("VCN"),Response);
               }
        break;

        case 8:

        break;

      }
}

void tradingDialog::on_TradingTabWidget_tabBarClicked(int index)
{
 //tab was clicked, interrupt the timer and restart after action completed.

    // qDebug() << "Stopping timer";
    if (this->timerid != 0)
    {
        this->timer->stop();
    }
    // qDebug() << "Switching";
    ActionsOnSwitch(index);

    // qDebug() << "Restarting timer";
    if (this->timerid != 0)
    {
        this->timer->start();
    }
}

QString tradingDialog::sendRequest(QString url)
{

    QString Response = "";
    QString Secret   = this->SecretKey;

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req = QNetworkRequest(QUrl(url));

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    //make this conditional,depending if we are using private api call
    req.setRawHeader("apisign",HMAC_SHA512_SIGNER(url,Secret).toStdString().c_str()); //set header for bittrex

    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        Response = reply->readAll();
        // QMessageBox::information(this,"Success",Response);
        delete reply;
    }
    else{
        //failure
        // qDebug() << "Failure" <<reply->errorString();
        Response = "Error";
        // QMessageBox::information(this,"Error",reply->errorString());
        delete reply;
        }

     return Response;
}

QString tradingDialog::BittrexTimeStampToReadable(QString DateTime)
{
    //Separate Time and date.
    // int TPos = DateTime.indexOf("T");
    // int sPos = DateTime.indexOf(".");
    int _Pos = DateTime.indexOf(" ");
    QDateTime Date = QDateTime::fromString(DateTime.left(_Pos),"yyyy-MM-dd"); //format to convert from
    DateTime.remove(0,_Pos+1);
    QDateTime Time = QDateTime::fromString(DateTime.right(_Pos),"hh:mm:ss");
    //Reconstruct time and date in our own format, one that QDateTime will recognise.
    QString DisplayDate = Date.toString("dd/MM/yyyy") + " " + Time.toString("hh:mm:ss A"); //formats to convert to

    return DisplayDate;
}

qint64 tradingDialog::BittrexTimeStampToSeconds(QString DateTime)
{
    QDateTime date = QDateTime::fromString(DateTime,"yyyy-MM-dd hh:mm:ss"); //format to convert from
    return date.toTime_t();
}

void tradingDialog::CalculateBuyCostLabel()
{

    double price    = ui->BuyBidPriceEdit->text().toDouble();
    double Quantity = ui->UnitsInput->text().toDouble();
    double cost = ((price * Quantity) + ((price * Quantity / 100) * 0.25));

    QString Str = "";
    ui->BuyCostLabel->setText(Str.number(cost,'i',8));
}

void tradingDialog::CalculateSellCostLabel()
{

    double price    = ui->SellBidPriceEdit->text().toDouble();
    double Quantity = ui->UnitsInputVCN->text().toDouble();
    double cost = ((price * Quantity) - ((price * Quantity / 100) * 0.25));

    QString Str = "";
    ui->SellCostLabel->setText(Str.number(cost,'i',8));
}

void tradingDialog::on_UpdateKeys_clicked()
{
    this->ApiKey    = ui->ApiKeyInput->text();
    this->SecretKey = ui->SecretKeyInput->text();
    this->Currency = "VCN";


    /*
    QString Balance = GetBalance(this->Currency).toUtf8();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(GetBalance(this->Currency).toUtf8()); //get json from str.
    QJsonObject ResponseObject = jsonResponse.object();                                 //get json obj
    QJsonValue rstatus = ResponseObject.value(QString("success"));
    QString isfalse = "false";
    QString istrue = "true";

    if ( rstatus == isfalse)
    */
    // QJsonDocument jsonResponse = QJsonDocument::fromJson(GetAccountHistory().toUtf8()); //get json from str.
    QJsonDocument jsonResponse = QJsonDocument::fromJson(GetBalance(this->Currency).toUtf8()); //get json from str.
    QJsonObject ResponseObject = jsonResponse.object();                                 //get json obj
    // QString Balance = GetBalance(this->Currency).toUtf8();
    // QMessageBox::information(this,"Response",Balance);

   if ( ResponseObject.value("success").toBool() == false)

    {
        QMessageBox::information(this,"API Configuration Failed","Api configuration was unsuccesful.");

    }
    else if ( ResponseObject.value("success").toBool() == true)
    {
         QMessageBox::information(this,"API Configuration Complete" ,"Api connection has been successfully configured and tested.");
         ui->ApiKeyInput->setEchoMode(QLineEdit::Password);
         ui->SecretKeyInput->setEchoMode(QLineEdit::Password);
         ui->TradingTabWidget->setTabEnabled(0,true);
         ui->TradingTabWidget->setTabEnabled(1,true);
         ui->TradingTabWidget->setTabEnabled(3,true);
         ui->TradingTabWidget->setTabEnabled(4,true);
         ui->TradingTabWidget->setTabEnabled(5,true);
         ui->TradingTabWidget->setTabEnabled(6,true);
  }

}

void tradingDialog::on_GenDepositBTN_clicked()
{
    QString response         =  GetDepositAddress();
    QJsonObject ResultObject =  GetResultObjectFromJSONObject(response);
    ui->DepositAddressLabel->setText(ResultObject["Address"].toString());
}

void tradingDialog::on_Sell_Max_Amount_clicked()
{
    //calculate amount of BTC that can be gained from selling VCN available balance
    QString responseA = GetBalance("VCN");
    QString str;
    QJsonObject ResultObject =  GetResultObjectFromJSONObject(responseA);

    double AvailableVCN = ResultObject["Available"].toDouble();

    ui->UnitsInputVCN->setText(str.number(AvailableVCN,'i',8));
}

void tradingDialog::on_Buy_Max_Amount_clicked()
{
    //calculate amount of currency than can be brought with the BTC balance available
    QString responseA = GetBalance("BTC");
    QString responseB = GetMarketSummary();
    QString str;

    QJsonObject ResultObject =  GetResultObjectFromJSONObject(responseA);
    QJsonObject ResultObj    =  GetResultObjectFromJSONArray(responseB);

    //Get the Bid ask or last value from combo
    QString value = ui->BuyBidcomboBox->currentText();

    double AvailableBTC = ResultObject["Available"].toDouble();
    double CurrentASK   = ResultObj[value].toDouble();
    double Result = (AvailableBTC / CurrentASK);
    double percentofnumber = (Result * 0.0025);

    Result = Result - percentofnumber;
    ui->UnitsInput->setText(str.number(Result,'i',8));
}

void tradingDialog::on_buyOrdertypeCombo_activated(const QString &arg1)
{
   if(arg1 == "Conditional"){
                             //ui->BuyWhenPriceLabel->show();
                             //ui->BuyConditionCombo->show();
                             //ui->ConditionLineEdit->show();
                             //ui->Conditionlabel->show();
                            }else if (arg1 == "Limit"){
                                                     //  ui->BuyWhenPriceLabel->hide();
                                                     //  ui->BuyConditionCombo->hide();
                                                     //  ui->ConditionLineEdit->hide();
                                                     //  ui->Conditionlabel->hide();
                                                      }
}

QJsonObject tradingDialog::GetResultObjectFromJSONObject(QString response)
{

    QJsonDocument jsonResponse = QJsonDocument::fromJson(response.toUtf8());          //get json from str.
    QJsonObject  ResponseObject = jsonResponse.object();                              //get json obj
    QJsonObject  ResultObject   = ResponseObject.value(QString("result")).toObject(); //get result object

    return ResultObject;
}

QJsonObject tradingDialog::GetResultObjectFromJSONArray(QString response)
{

    QJsonDocument jsonResponsea = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject   jsonObjecta   = jsonResponsea.object();
    QJsonArray    jsonArraya    = jsonObjecta["result"].toArray();
    QJsonObject   obj;

    Q_FOREACH (const QJsonValue & value, jsonArraya)
        {
        obj = value.toObject();
        }

    return obj;
}

QJsonArray tradingDialog::GetResultArrayFromJSONObject(QString response)
{

    QJsonDocument jsonResponse = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject   jsonObject   = jsonResponse.object();
    QJsonArray    jsonArray    = jsonObject["result"].toArray();

    return jsonArray;
}

QString tradingDialog::HMAC_SHA512_SIGNER(QString UrlToSign, QString Secret)
{

    QString retval = "";

    QByteArray byteArray = UrlToSign.toUtf8();
    const char* URL = byteArray.constData();

    QByteArray byteArrayB = Secret.toUtf8();
    const char* Secretkey = byteArrayB.constData();

    unsigned char* digest;

    // Using sha512 hash engine here.
    digest = HMAC(EVP_sha512(),  Secretkey, strlen( Secretkey), (unsigned char*) URL, strlen( URL), NULL, NULL);

    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    char mdString[128];

    for(int i = 0; i < 64; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    }
    retval = mdString;
    //qDebug() << "HMAC digest:"<< retval;

    return retval;
}

void tradingDialog::on_SellBidcomboBox_currentIndexChanged(const QString &arg1)
{
    QString response = GetMarketSummary();
    QJsonObject ResultObject = GetResultObjectFromJSONArray(response);
    QString Str;

    //Get the Bid ask or last value from combo
    ui->SellBidPriceEdit->setText(Str.number(ResultObject[arg1].toDouble(),'i',8));

    CalculateSellCostLabel(); //update cost
}

void tradingDialog::on_BuyBidcomboBox_currentIndexChanged(const QString &arg1)
{
    QString response = GetMarketSummary();
    QJsonObject ResultObject = GetResultObjectFromJSONArray(response);
    QString Str;

    //Get the Bid ask or last value from combo
    ui->BuyBidPriceEdit->setText(Str.number(ResultObject[arg1].toDouble(),'i',8));

    CalculateBuyCostLabel(); //update cost
}

void tradingDialog::on_BuyVCN_clicked()
{
    double Rate;
    double Quantity;

    Rate     = ui->BuyBidPriceEdit->text().toDouble();
    Quantity = ui->UnitsInput->text().toDouble();

    QString OrderType = ui->buyOrdertypeCombo->currentText();
    QString Order;

    if(OrderType == "Limit"){Order = "buylimit";}else if (OrderType == "Market"){ Order = "buymarket";}

    QString Msg = "Are you sure you want to buy ";
            Msg += ui->UnitsInput->text();
            Msg += "VCN @ ";
            Msg += ui->BuyBidPriceEdit->text();
            Msg += " BTC Each";

            QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Buy Order",Msg,QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {

                QString Response =  BuyVCN(Order,Quantity,Rate);

                QJsonDocument jsonResponse = QJsonDocument::fromJson(Response.toUtf8());          //get json from str.
                QJsonObject  ResponseObject = jsonResponse.object();                              //get json obj

                if (ResponseObject["success"].toBool() == false){

                    QMessageBox::information(this,"Buy Order Failed",ResponseObject["message"].toString());

                }else if (ResponseObject["success"].toBool() == true){
                        QMessageBox::information(this,"Buy Order Initiated","You Placed an order");
                        }
            }else{

                  //do nothing
                 }
}

void tradingDialog::on_SellVCNBTN_clicked()
{
    double Rate;
    double Quantity;

    Rate     = ui->SellBidPriceEdit->text().toDouble();
    Quantity = ui->UnitsInputVCN->text().toDouble();

    QString OrderType = ui->SellOrdertypeCombo->currentText();
    QString Order;

    if(OrderType == "Limit"){Order = "selllimit";}else if (OrderType == "Market"){ Order = "sellmarket";}

    QString Msg = "Are you sure you want to Sell ";
            Msg += ui->UnitsInputVCN->text();
            Msg += " VCN @ ";
            Msg += ui->SellBidPriceEdit->text();
            Msg += " BTC Each";

            QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Sell Order",Msg,QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {

            QString Response =  SellVCN(Order,Quantity,Rate);
            QJsonDocument jsonResponse = QJsonDocument::fromJson(Response.toUtf8());          //get json from str.
            QJsonObject  ResponseObject = jsonResponse.object();                              //get json obj

            if (ResponseObject["success"].toBool() == false){

                QMessageBox::information(this,"Sell Order Failed",ResponseObject["message"].toString());

                 }else if (ResponseObject["success"].toBool() == true){
                           QMessageBox::information(this,"Sell Order Initiated","You Placed an order");
                          }
}else{

      //do nothing
     }
}

void tradingDialog::on_AdvancedView_stateChanged(int arg1)
{
    //Show or hide columns in OpenOrders Table depending on checkbox state
    if(arg1 == 2){
        ui->OpenOrdersTable->setColumnHidden(9,false);
        ui->OpenOrdersTable->setColumnHidden(10,false);
        ui->OpenOrdersTable->setColumnHidden(11,false);
        ui->OpenOrdersTable->setColumnHidden(12,false);
        ui->OpenOrdersTable->setColumnHidden(13,false);
    }else if(arg1 == 0) {
        ui->OpenOrdersTable->setColumnHidden(9,true);
        ui->OpenOrdersTable->setColumnHidden(10,true);
        ui->OpenOrdersTable->setColumnHidden(11,true);
        ui->OpenOrdersTable->setColumnHidden(12,true);
        ui->OpenOrdersTable->setColumnHidden(13,true);

          }
}

void tradingDialog::on_UnitsInputVCN_textChanged(const QString &arg1)
{
     CalculateSellCostLabel(); //update cost
}

void tradingDialog::on_UnitsInput_textChanged(const QString &arg1)
{
     CalculateBuyCostLabel(); //update cost
}

void tradingDialog::on_BuyBidPriceEdit_textChanged(const QString &arg1)
{
     CalculateBuyCostLabel(); //update cost
}

void tradingDialog::on_SellBidPriceEdit_textChanged(const QString &arg1)
{
     CalculateSellCostLabel();
}

void tradingDialog::setModel(WalletModel *model)
{
    this->model = model;
}

tradingDialog::~tradingDialog()
{
    delete ui;
}
