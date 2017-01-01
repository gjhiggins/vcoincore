#include "inscriptionpage.h"
#include "ui_inscriptionpage.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "amount.h"
#include "platformstyle.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "util.h"

#include "ui_interface.h"

#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QWebEngineView>
#include <QTextStream>
#include <QFile>
#include <QMenu>
#include <QLabel>

InscriptionPage::InscriptionPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    platformStyle(platformStyle),
    ui(new Ui::InscriptionPage),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);
}

InscriptionPage::~InscriptionPage()
{
    delete ui;
}

void InscriptionPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void InscriptionPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}

void InscriptionPage::show()
{
    QWebEngineView *view = new QWebEngineView;
    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///pbt/index.html";
    QString fpath = "/src/qt/res/pbt/index.html";
    QString res = cwd + fpath;
    QFile file(res);
    // QFile file("file:////res/pbt/index.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        html = QString("<html><body><p>Cannot read file in ") + res + QString("</p></body></html>");
    } else {
        html = QTextStream(&file).readAll();
    };
    view->setHtml(html, QUrl());
    view->show();
}
