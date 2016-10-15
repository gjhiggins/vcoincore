
#ifndef EXAMPLEDIALOG_H
#define EXAMPLEDIALOG_H

#include <QDialog>
#include <QObject>
#include <stdint.h>
#include "ui_exampledialog.h"
#include "walletmodel.h"

namespace Ui {
class exampleDialog;
}

class exampleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exampleDialog(QWidget *parent = 0);
    ~exampleDialog();

    void setModel(WalletModel *model);

private:
    Ui::exampleDialog *ui;
    WalletModel *model;

};


#endif // EXAMPLEDIALOG_H
