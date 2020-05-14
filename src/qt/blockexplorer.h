// Copyright (c) 2020 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BLOCKEXPLORER_H
#define BLOCKEXPLORER_H

#include <uint256.h>
#include <base58.h>
#include <qt/guiutil.h>
#include <QMainWindow>

class ClientModel;
class PlatformStyle;
class CBlockIndex;
class CTransaction;
class CBlockTreeDB;
class CTxOut;
class COutPoint;

namespace Ui {
class BlockExplorer;
}

std::string getexplorerBlockHash(int64_t);
const CBlockIndex* getexplorerBlockIndex(int64_t);
CTxOut getPrevOut(const COutPoint& out);
void getNextIn(const COutPoint *Out, uint256 *Hash, unsigned int n);

class BlockExplorer : public QMainWindow
{
    Q_OBJECT

public:
    explicit BlockExplorer(const PlatformStyle *platformStyle, QWidget *parent = nullptr);
    ~BlockExplorer();

    void setClientModel(ClientModel *clientModel);
    // void setModel(WalletModel *model);

protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent*);

public Q_SLOTS:

private:
    Ui::BlockExplorer *ui;
    const PlatformStyle *platformStyle;
    bool m_NeverShown;
    int m_HistoryIndex;
    QStringList m_History;
    ClientModel *clientModel;

    void setBlock(CBlockIndex* pBlock);
    bool switchTo(const QString& query);
    void setContent(const std::string &content);
    void updateNavButtons();

private Q_SLOTS:
    void onSearch();
    void goTo(const QString& query);
    void back();
    void forward();

Q_SIGNALS:

};

#endif // BLOCKEXPLORER_H
