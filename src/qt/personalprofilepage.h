#ifndef PERSONALPROFILEPAGE_H
#define PERSONALPROFILEPAGE_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class PersonalProfilePage;
}
class QDialog;
class PlatformStyle;

QT_BEGIN_NAMESPACE
  class PersonalProfilePage;
QT_END_NAMESPACE

/** Web page widget */
class PersonalProfilePage : public QDialog
{
    Q_OBJECT

public:
    explicit PersonalProfilePage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~PersonalProfilePage();

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::PersonalProfilePage *ui;
    PlatformStyle *platformStyle;

private Q_SLOTS:
    void show();
};

#endif // PERSONALPROFILEPAGE_H
