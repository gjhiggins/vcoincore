#include "personalprofilepage.h"
#include "ui_personalprofilepage.h"

#include "guiutil.h"

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QWebEngineView>

PersonalProfilePage::PersonalProfilePage(QWidget *parent) :
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
    QString html = "qrc:///plume/index.html";
    QString fpath = "/src/qt/res/plume/index.html";
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
