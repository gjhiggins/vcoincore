#include "bip32hdpage.h"
#include "ui_bip32hdpage.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "util.h"
#include "cookiejar.h"
#include "webview.h"

using namespace GUIUtil;

BIP32HDPage::BIP32HDPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BIP32HDPage),
    walletModel(0)
{
    ui->setupUi(this);

    /*
    // Setup header and styles
    if (fNoHeaders)
        GUIUtil::header(this, QString(""));
    else if (fSmallHeaders)
        GUIUtil::header(this, QString(":icons/headerBIP32Small"));
    else
        GUIUtil::header(this, QString(":icons/headerBIP32"));
    */

    GUIUtil::header(this, QString(""));
    this->layout()->setContentsMargins(0, HEADER_HEIGHT, 0, 0);

    CookieJar *getBIP32HDPageJar = new CookieJar;
    ui->webView->page()->networkAccessManager()->setCookieJar(getBIP32HDPageJar);

    ui->webView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    ui->frame->setVisible(true); // Set to true to enable webView navigation buttons
    connect(ui->webView->page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )), ui->webView, SLOT(sslErrorHandler(QNetworkReply*, const QList<QSslError> & )));
    connect(ui->webView->page(), SIGNAL(linkClicked(QUrl)), ui->webView, SLOT(myOpenUrl(QUrl)));

    // buttons
    ui->back->setDisabled(true);
    ui->home->setDisabled(true);
    ui->forward->setDisabled(true);
    ui->webView->sendButtons(ui->back, ui->home, ui->forward);
    connect(ui->back, SIGNAL(clicked()), ui->webView, SLOT(myBack()));
    connect(ui->home, SIGNAL(clicked()), ui->webView, SLOT(myHome()));
    connect(ui->forward, SIGNAL(clicked()), ui->webView, SLOT(myForward()));
    connect(ui->reload, SIGNAL(clicked()), ui->webView, SLOT(myReload()));
}

BIP32HDPage::~BIP32HDPage()
{
    delete ui;
}

void BIP32HDPage::setModel(WalletModel *model)
{
    this->walletModel = model;

    // QUrl url(QString(walletUrl).append("wallet/getsolarcoin.html"));
    QUrl url(QString("qrc:///bip32/index.html"));
    ui->webView->myOpenUrl(url);
}
