// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <qt/inscriptionpage.h>
#include <qt/forms/ui_inscriptionpage.h>

#include <qt/inscriptiontablemodel.h>
#include <qt/bitcoingui.h>
#include <qt/csvmodelwriter.h>
#include <qt/editaddressdialog.h>
#include <qt/guiutil.h>
#include <qt/platformstyle.h>

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>

InscriptionPage::InscriptionPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InscriptionPage),
    model(0)
{
    ui->setupUi(this);

    setWindowTitle(tr("Inscribed on the V Core blockchain"));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    ui->tableView->setFocus();
    ui->closeButton->setText(tr("Close"));
    ui->labelExplanation->setText(tr("These are your V Core inscriptions."));

    // Context menu actions
    QAction *copyInscriptionAction = new QAction(tr("Copy &Inscription"), this);

    // Build context menu
    contextMenu = new QMenu(this);
    contextMenu->addAction(copyInscriptionAction);
    contextMenu->addSeparator();

    // Connect signals for context menu actions
    connect(copyInscriptionAction, SIGNAL(triggered()), this, SLOT(onCopyInscriptionAction()));

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

InscriptionPage::~InscriptionPage()
{
    delete ui;
}

void InscriptionPage::setModel(InscriptionTableModel *_model)
{
    this->model = _model;
    if(!_model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // proxyModel->setFilterRole(InscriptionTableModel::TypeRole);
    // proxyModel->setFilterFixedString('R');

    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    ui->tableView->horizontalHeader()->setSectionResizeMode(InscriptionTableModel::Date, QHeaderView::Stretch);
    // ui->tableView->horizontalHeader()->setSectionResizeMode(InscriptionTableModel::TransactionID, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(InscriptionTableModel::Inscription, QHeaderView::ResizeToContents);
}

void InscriptionPage::onCopyInscriptionAction()
{
    GUIUtil::copyEntryData(ui->tableView, InscriptionTableModel::Inscription);
}

//void InscriptionPage::on_exportButton_clicked()
//{
//    // CSV is currently the only supported format
//    QString filename = GUIUtil::getSaveFileName(this,
//        tr("Export Address List"), QString(),
//        tr("Comma separated file (*.csv)"), nullptr);

//    if (filename.isNull())
//        return;

//    CSVModelWriter writer(filename);

//    // name, column, role
//    writer.setModel(proxyModel);
//    writer.addColumn("Label", InscriptionTableModel::Label, Qt::EditRole);
//    writer.addColumn("Address", InscriptionTableModel::Address, Qt::EditRole);

//    if(!writer.write()) {
//        QMessageBox::critical(this, tr("Exporting Failed"),
//            tr("There was an error trying to save the address list to %1. Please try again.").arg(filename));
//    }
//}
