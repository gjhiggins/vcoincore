// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef EXAMPLEWINDOW_H
#define EXAMPLEWINDOW_H

#include "guiutil.h"

#include <QWidget>
#include <QCompleter>

class PlatformStyle;

namespace Ui {
    class ExampleWindow;
}

QT_BEGIN_NAMESPACE
class QMenu;
class QItemSelection;
QT_END_NAMESPACE

/** Example Qt Window. */
class ExampleWindow: public QWidget
{
    Q_OBJECT

public:
    explicit ExampleWindow(const PlatformStyle *platformStyle, QWidget *parent);
    ~ExampleWindow();

private:
    Ui::ExampleWindow *ui;
    const PlatformStyle *platformStyle;
};

#endif // EXAMPLEWINDOW_H
