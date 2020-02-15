#ifndef PUBLISHINGPAGE_H
#define PUBLISHINGPAGE_H

#include <QDialog>

class WalletModel;
class QWidget;

namespace Ui {
    class PublishingPage;
}

/** Manage a personal profile */
class PublishingPage : public QWidget
{
    Q_OBJECT

private:
    WalletModel *walletModel;

public:
    explicit PublishingPage(QWidget *parent, WalletModel *wm);
    ~PublishingPage();

private:
    Ui::PublishingPage *ui;

public Q_SLOTS:
    void show();

private Q_SLOTS:

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */

};

#endif // PUBLISHINGPAGE_H
