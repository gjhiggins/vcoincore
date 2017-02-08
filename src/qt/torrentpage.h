// Copyright (c) 2015-2016 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TORRENTPAGE_H
#define TORRENTPAGE_H

#include <QDialog>
#include <QThread>

namespace Ui {
    class TorrentPage;
}
class TorrentTableModel;
class ClientModel;
class WalletModel;
class PlatformStyle;

QT_BEGIN_NAMESPACE
class QTableView;
class QItemSelection;
class QSortFilterProxyModel;
class QMenu;
class QModelIndex;
QT_END_NAMESPACE

/** Widget that shows a list of sending or receiving addresses.
  */
class TorrentPage : public QDialog
{
    Q_OBJECT

public:

    explicit TorrentPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~TorrentPage();

    void setModel(TorrentTableModel *model);
    void setClientModel(ClientModel *model);

    const QString &getReturnValue() const { return returnValue; }
    void startExecutor();

public Q_SLOTS:
    void refreshTorrentTable();
private:
    Ui::TorrentPage *ui;
    TorrentTableModel *model;
    ClientModel *clientModel;
    QString returnValue;
    QSortFilterProxyModel *proxyModel;
    QMenu *contextMenu;
    QAction *deleteAction;
    QString newAddressToSelect;
    
Q_SIGNALS:
    void doubleClicked(const QModelIndex&);
    void updateRequest();
    void stopExecutor();

private Q_SLOTS:
    void searchButtonClicked();
    void showDetails();

// private:

};

#endif // TORRENTPAGE_H
