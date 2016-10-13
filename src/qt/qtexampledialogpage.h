#ifndef EXAMPLEDIALOGPAGE_H
#define EXAMPLEDIALOGPAGE_H

#include "platformstyle.h"

#include <QWidget>
#include <QFrame>

namespace Ui
{
    class ExampleDialogPage;
}

/** Page for Example Dialog */
class ExampleDialogPage : public QWidget
{
    Q_OBJECT

public:
    explicit ExampleDialogPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~ExampleDialogPage();

private:
    const PlatformStyle *platformStyle;
    Ui::ExampleDialogPage *ui;

public Q_SLOTS:

private Q_SLOTS:

};

#endif // EXAMPLEDIALOGPAGE_H
