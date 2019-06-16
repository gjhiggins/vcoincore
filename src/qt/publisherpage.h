#ifndef PUBLISHERPAGE_H
#define PUBLISHERPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class PublisherPage;
}
class QDialog;

/** Manage a personal profile */
class PublisherPage : public QDialog
{
    Q_OBJECT

public:
    explicit PublisherPage(QWidget *parent = 0);
    ~PublisherPage();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::PublisherPage *ui;

public Q_SLOTS:

private Q_SLOTS:
    void show();

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

};



#endif // PUBLISHERPAGE_H
