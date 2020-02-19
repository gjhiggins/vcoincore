#include "bip32hdpage.h"
#include "ui_bip32hdpage.h"

#include "guiutil.h"

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QWebEngineView>

BIP32HDPage::BIP32HDPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BIP32HDPage)
{
    ui->setupUi(this);
}

BIP32HDPage::~BIP32HDPage()
{
    delete ui;
}

void BIP32HDPage::show()
{
    QWebEngineView *view = new QWebEngineView;
    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///bip32/index.html";
    QString fpath = "/src/qt/res/bip32/index.html";
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
