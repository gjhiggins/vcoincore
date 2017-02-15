#include "publisherpage.h"
#include "ui_publisherpage.h"

#include "guiutil.h"

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QWebEngineView>

PublisherPage::PublisherPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PublisherPage)
{
    ui->setupUi(this);
}

PublisherPage::~PublisherPage()
{
    delete ui;
}

void PublisherPage::show()
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
