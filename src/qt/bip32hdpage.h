#ifndef BIP32HDPAGE_H
#define BIP32HDPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class BIP32HDPage;
}
class QDialog;

/* BIP32 HD page */
class BIP32HDPage : public QDialog
{
    Q_OBJECT

public:
    explicit BIP32HDPage(QWidget *parent = 0);
    ~BIP32HDPage();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::BIP32HDPage *ui;

public Q_SLOTS:

private Q_SLOTS:
    void show();

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

};



#endif // BIP32HDPAGE_H
