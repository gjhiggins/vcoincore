#include "publishingpage.h"
#include <qt/forms/ui_publishingpage.h>

#include <qt/guiutil.h>

#include <QWidget>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QWebEngineView>

PublishingPage::PublishingPage(QWidget *parent, WalletModel *wm) :
    QWidget(parent), walletModel(wm),
    ui(new Ui::PublishingPage)
{
    ui->setupUi(this);
};

PublishingPage::~PublishingPage()
{
    delete ui;
}

void PublishingPage::show() {

    QWebEngineView *view = new QWebEngineView;
    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///plume/index.html";
    QString fpath = "/minkiz/VCore/working-vc/src/qt/res/plume/index.html";
    QString res = cwd + fpath;
    QFile file(res);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        html = QString("<html><body><p>Cannot read file in ") + res + QString("</p></body></html>");
    } else {
        html = QTextStream(&file).readAll();
    };
    view->setHtml(html, QUrl());
    view->show();
};

