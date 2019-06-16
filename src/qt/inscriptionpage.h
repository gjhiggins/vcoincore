// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INSCRIPTIONPAGE_H
#define BITCOIN_QT_INSCRIPTIONPAGE_H

#include <QDialog>

class InscriptionTableModel;
class PlatformStyle;

namespace Ui {
    class InscriptionPage;
}

QT_BEGIN_NAMESPACE
class QItemSelection;
class QMenu;
class QModelIndex;
class QSortFilterProxyModel;
QT_END_NAMESPACE

/** Widget that shows a list of sending or receiving addresses.
  */
class InscriptionPage : public QDialog
{
    Q_OBJECT

public:

    explicit InscriptionPage(QWidget *parent);
    ~InscriptionPage();

    void setModel(InscriptionTableModel *model);

public Q_SLOTS:
    // void done(int retval);

private:
    Ui::InscriptionPage *ui;
    InscriptionTableModel *model;
    QSortFilterProxyModel *proxyModel;
    QMenu *contextMenu;

private Q_SLOTS:
     /** Export button clicked */
//    void on_exportButton_clicked();
    void onCopyInscriptionAction();


Q_SIGNALS:
    void sendCoins(QString addr);
};

#endif // BITCOIN_QT_INSCRIPTIONPAGE_H
