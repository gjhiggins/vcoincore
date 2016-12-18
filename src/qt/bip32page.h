#ifndef BIP32PAGE_H
#define BIP32PAGE_H

#include <QWidget>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QTimer>

namespace Ui {
    class BIP32Page;
}
class ClientModel;
class WalletModel;
class QDialog;
class PlatformStyle;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** BIP32 page widget */
class BIP32Page : public QDialog
{
    Q_OBJECT

public:
    explicit BIP32Page(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~BIP32Page();

    void setClientModel(PlatformStyle *platformStyle, ClientModel *clientModel);
    void setWalletModel(PlatformStyle *platformStyle, WalletModel *walletModel);

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::BIP32Page *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    PlatformStyle *platformStyle;

    QNetworkAccessManager *nam;

private Q_SLOTS:
    void finished(QNetworkReply *reply);
    void DoHttpGet();
};

/*
#include <QtEngineWidgets>
#include <QtGui>

class BIP32View:public QWebEngineView
{
  Q_OBJECT
  public:
    BIP32View():QWebView()
    {
      setWindowTitle("BIP32");
      m_pTextEdit  = new QTextEdit();
    }    
    ~BIP32View(){}
  
    void contextMenuEvent(QContextMenuEvent* event)
    {      
      QMenu menu;      
      QAction* pActionShowPageSource = menu.addAction("Show Page Source");
      
      connect(pActionShowPageSource,SIGNAL(triggered()),this,SLOT(slotShowPageSource()));
      
      menu.exec(mapToGlobal(QPoint(event->x(),event->y())));     
    }    
    
private slots:
   void slotShowPageSource()
   {     
     m_pTextEdit->setPlainText(page()->mainFrame()->toHtml());
     m_pTextEdit->setWindowTitle("BIP32View Page Source");
     m_pTextEdit->setFixedSize(400,300);
     m_pTextEdit->show();
   }
   
private:
  QTextEdit* m_pTextEdit;
};

*/

#endif // BIP32PAGE_H
