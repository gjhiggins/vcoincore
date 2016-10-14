#ifndef EXAMPLEWINDOW_H
#define EXAMPLEWINDOW_H

#include "platformstyle.h"

#include <QWidget>
#include <QFrame>

namespace Ui
{
    class ExampleWindow;
}

/** Page for Example Dialog */
class ExampleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ExampleWindow(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~ExampleWindow();

private:
    const PlatformStyle *platformStyle;
    Ui::ExampleWindow *ui;

public Q_SLOTS:

private Q_SLOTS:

};

#endif // EXAMPLEWINDOW_H
