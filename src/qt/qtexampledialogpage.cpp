#include "qtexampledialogpage.h"
#include "ui_qtexampledialogpage.h"
#include "platformstyle.h"

#include "guiutil.h"
#include "base58.h"
#include "main.h"
#include "wallet/wallet.h"
#include "ui_interface.h"
#include "util.h"

using namespace std;

ExampleDialogPage::ExampleDialogPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    platformStyle(platformStyle),
    ui(new Ui::ExampleDialogPage)
    // model(0),
    // walletModel(0),
    // proxyModel(0)
{
    ui->setupUi(this);

}

ExampleDialogPage::~ExampleDialogPage()
{
    delete ui;
}
