#ifndef PERSONALPROFILEPAGE_H
#define PERSONALPROFILEPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class PersonalProfilePage;
}
class QDialog;

/** Manage a personal profile */
class PersonalProfilePage : public QDialog
{
    Q_OBJECT

public:
    explicit PersonalProfilePage(QWidget *parent = 0);
    ~PersonalProfilePage();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::PersonalProfilePage *ui;

public Q_SLOTS:

private Q_SLOTS:
    void show();

Q_SIGNALS:
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

};



#endif // PERSONALPROFILEPAGE_H
