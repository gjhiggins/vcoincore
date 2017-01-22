#ifndef INSCRIPTIONPAGE_H
#define INSCRIPTIONPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class InscriptionPage;
}
class QDialog;

/** Create an inscription */
class InscriptionPage : public QDialog
{
    Q_OBJECT

public:
    explicit InscriptionPage(QWidget *parent = 0);
    ~InscriptionPage();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::InscriptionPage *ui;

public Q_SLOTS:

private Q_SLOTS:
    void show();

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

};



#endif // INSCRIPTIONPAGE_H
