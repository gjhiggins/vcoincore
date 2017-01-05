#include "inscriptionpage.h"
#include "ui_inscriptionpage.h"

#include "guiutil.h"

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QWebEngineView>

InscriptionPage::InscriptionPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InscriptionPage)
{
    ui->setupUi(this);
}

InscriptionPage::~InscriptionPage()
{
    delete ui;
}

void InscriptionPage::show()
{
    QWebEngineView *view = new QWebEngineView;
    QDir dir;
    QString cwd = dir.currentPath();
    QString html = "qrc:///inscribe/index.html";
    QString fpath = "/src/qt/res/inscribe/index.html";
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


/*
void yourClass::mainFunction()
{
    View = new QWebEngineView(this);

    connect( View->page(), SIGNAL(loadFinished(bool)), this, SLOT(slotForRunJS(bool)));
}

void yourClass::slotForRunJS(bool ok)
{
    // read the js file using qfile
    file.open("path to jsFile");
    myJsApi = file.Readall();
    View->page()->runJavaScript(myjsapi);
    View->page()->runJavaScript("createRadioButton(\"button1\");");
}
*/
