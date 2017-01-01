#ifndef INSCRIPTIONPAGE_H
#define INSCRIPTIONPAGE_H

#include "platformstyle.h"

#include <QWidget>

class WalletModel;
class ClientModel;

namespace Ui {
    class InscriptionPage;
}

QT_BEGIN_NAMESPACE
class QWebEngineView;
class QUrl;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class InscriptionPage : public QWidget
{
    Q_OBJECT

public:
    explicit InscriptionPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~InscriptionPage();

    void setClientModel(ClientModel *model);
    void setWalletModel(WalletModel *model);

private:
    const PlatformStyle *platformStyle;
    Ui::InscriptionPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    // QWidget *inscriptionPage;


public Q_SLOTS:	


private Q_SLOTS:
    void show();

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

};



#endif // INSCRIPTIONPAGE_H
