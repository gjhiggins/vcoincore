#ifndef BIP32HDPAGE_H
#define BIP32HDPAGE_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QTimer>

namespace Ui {
    class BIP32HDPage;
}
class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Trade page widget */
class BIP32HDPage : public QWidget
{
    Q_OBJECT

public:
    explicit BIP32HDPage(QWidget *parent = 0);
    ~BIP32HDPage();

    void setModel(ClientModel *clientModel);
    void setModel(WalletModel *walletModel);

public slots:

// signals:

private:
    Ui::BIP32HDPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;

private slots:

};

#endif // BIP32HDPAGE_H
