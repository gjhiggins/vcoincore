#ifndef ESSENTIALSPAGE_H
#define ESSENTIALSPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class EssentialsPage;
}
class ClientModel;
class WalletModel;
class QDialog;
class PlatformStyle;

QT_BEGIN_NAMESPACE
  class EssentialsPage;
QT_END_NAMESPACE

/** Web page widget */
class EssentialsPage : public QDialog
{
    Q_OBJECT

public:
    explicit EssentialsPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~EssentialsPage();

    void setClientModel(PlatformStyle *platformStyle, ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::EssentialsPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    PlatformStyle *platformStyle;

private Q_SLOTS:
    void show();
};

#endif // ESSENTIALSPAGE_H
