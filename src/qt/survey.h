#ifndef SURVEY_H
#define SURVEY_H

#include <QWidget>
#include <QDialog>
#include <QNetworkReply>

namespace Ui {
    class Survey;
}
class QNetworkAccessManager;
class QNetworkReply;
class ClientModel;
class WalletModel;
class QDialog;
class QString;
class PlatformStyle;

QT_BEGIN_NAMESPACE
  class Survey;
QT_END_NAMESPACE

/** Web page widget */
class Survey : public QDialog
{
    Q_OBJECT

public:
    explicit Survey(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~Survey();

    void setClientModel(PlatformStyle *platformStyle, ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    static QString getVersion();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::Survey *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    PlatformStyle *platformStyle;
    QString os;

private Q_SLOTS:
    // void show();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void requestReceived();

};

/*
#include <QDialog>

class QNetworkAccessManager;
class QNetworkReply;
class PlatformStyle;
class QDialog;
class QString;

namespace Ui {
    class Survey;
}

class Survey : public QDialog {
	Q_OBJECT

public:
    explicit Survey(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~Survey();
	static QString getVersion();

private Q_SLOTS:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void requestReceived();

private:
    Ui::Survey *ui;
	QString os;
};
*/

#endif // SURVEY_H
