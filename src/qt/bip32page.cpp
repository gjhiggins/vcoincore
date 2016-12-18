#include "bip32page.h"
#include "ui_bip32page.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "platformstyle.h"

#include <QAbstractItemDelegate>
#include <QPainter>
// #include <QtWebEngineView>
// #include <QFile>


#define DECORATION_SIZE 64
#define NUM_ITEMS 3

BIP32Page::BIP32Page(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BIP32Page),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);


    nam = new QNetworkAccessManager(this);
    DoHttpGet();

    QTimer *timer = new QTimer(this);
    ui->textBrowser->setHidden(true);

    connect(timer, SIGNAL(timeout()), this, SLOT(DoHttpGet()));
    timer->start(35000);

    connect(nam,SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));

    /*
    QWebEngineView *view = new QWebEngineView;
    view->show();

    QFile file("qrc:///bip32/index.html");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QString html = QTextStream(&file).readAll();
    view->setHtml(html);

    return a.exec();
    */
}

BIP32Page::~BIP32Page()
{
    delete ui;
}

void BIP32Page::setClientModel(PlatformStyle *platformStyle, ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void BIP32Page::setWalletModel(PlatformStyle *platformStyle, WalletModel *model)
{
    this->walletModel = model;
}

void BIP32Page::finished(QNetworkReply *reply) {
  ui->textBrowser->setHidden(false);

  if(reply->error() == QNetworkReply::NoError) {
    ui->textBrowser->setText(reply->readAll());
  } else {
    ui->textBrowser->setText(reply->errorString());
  }
}

void BIP32Page::DoHttpGet() {
  // QString url = "https://en.bitcoin.it/wiki/BIP_0032";
  QString res = "qrc:///bip32/index.html";
  nam->get(QNetworkRequest(QUrl(res)));
}

/*
    MyWebView webView;    
    webView.load(QUrl("http://www.qt-project.org/"));
    webView.show();

*/
