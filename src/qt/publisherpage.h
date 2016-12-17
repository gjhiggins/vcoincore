#ifndef PUBLISHERPAGE_H
#define PUBLISHERPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class PublisherPage;
}
class ClientModel;
class WalletModel;
class QDialog;
class PlatformStyle;

QT_BEGIN_NAMESPACE
  class PublisherPage;
QT_END_NAMESPACE

/** BIP32 page widget */
class PublisherPage : public QDialog
{
    Q_OBJECT

public:
    explicit PublisherPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~PublisherPage();

    void setClientModel(PlatformStyle *platformStyle, ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::PublisherPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    PlatformStyle *platformStyle;

private Q_SLOTS:
    void show();
};

#endif // PUBLISHERPAGE_H
