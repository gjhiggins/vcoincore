#include "personalprofilepage.h"
#include "ui_personalprofilepage.h"

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

class QWebEngineView;

#define DECORATION_SIZE 64
#define NUM_ITEMS 3

PersonalProfilePage::PersonalProfilePage(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonalProfilePage)
{
    ui->setupUi(this);
}

PersonalProfilePage::~PersonalProfilePage()
{
    delete ui;
}

void PersonalProfilePage::show()
{
    QWebEngineView *view = new QWebEngineView;
    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///ppro/index.html";
    QString fpath = "/src/qt/res/ppro/index.html";
    QString res = cwd + fpath;
    QFile file(res);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        html = QString("<html><body><p>Cannot read file in ") + res + QString("</p></body></html>");
    } else {
        html = QTextStream(&file).readAll();
    };
    view->setHtml(html, QUrl());
    view->show();
}
