#include "essentialspage.h"
#include "ui_essentialspage.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "platformstyle.h"

#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QWebEngineView>
#include <QWebEngineSettings>

class QWebEngineView;

#define DECORATION_SIZE 64
#define NUM_ITEMS 3

EssentialsPage::EssentialsPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EssentialsPage),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);

}

EssentialsPage::~EssentialsPage()
{
    delete ui;
}

void EssentialsPage::setClientModel(PlatformStyle *platformStyle, ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void EssentialsPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;

}

void EssentialsPage::show()
{
    QWebEngineView *view = new QWebEngineView;
    view->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    view->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    view->page()->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    view->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///vess/index.html";
    QString fpath = "/src/qt/res/vess/index.html";
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
