// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "qtexamplewindow.h"
#include "ui_qtexamplewindow.h"

#include "guiutil.h"
#include "platformstyle.h"

#include <openssl/crypto.h>

#include <univalue.h>

#ifdef ENABLE_WALLET
#include <db_cxx.h>
#endif

#include <QWidget>

#if QT_VERSION < 0x050000
#include <QUrl>
#endif

using namespace std;

ExampleWindow::ExampleWindow(const PlatformStyle *_platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExampleWindow),
    platformStyle(_platformStyle)
{
    ui->setupUi(this);
    GUIUtil::restoreWindowGeometry("nExampleWindow", this->size(), this);

}

ExampleWindow::~ExampleWindow()
{
    GUIUtil::saveWindowGeometry("nExampleWindow", this);
    delete ui;
}
