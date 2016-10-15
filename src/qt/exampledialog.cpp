#include "exampledialog.h"
#include "ui_exampledialog.h"
#include "guiutil.h"

exampleDialog::exampleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exampleDialog)
{
    ui->setupUi(this);
}

void exampleDialog::setModel(WalletModel *model)
{
    this->model = model;
}

exampleDialog::~exampleDialog()
{
    delete ui;
}
