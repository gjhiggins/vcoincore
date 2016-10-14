#include "qtexamplewindow.h"
#include "ui_qtexamplewindow.h"
#include "platformstyle.h"

#include "guiutil.h"
#include "base58.h"
#include "main.h"
#include "wallet/wallet.h"
#include "ui_interface.h"
#include "util.h"

using namespace std;

ExampleWindow::ExampleWindow(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    platformStyle(platformStyle),
    ui(new Ui::ExampleWindow)
{
    ui->setupUi(this);

}

ExampleWindow::~ExampleWindow()
{
    delete ui;
}
