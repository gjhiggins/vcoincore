// Copyright (c) 2009-2018 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Developers
// Copyright (c) 2014-2018 The Dash Core Developers
// Copyright (c) 2016-2018 Duality Blockchain Solutions Developers
// Copyright (c) 2020 The V Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/miningpage.h>
#include <qt/forms/ui_miningpage.h>
#include <qt/clientmodel.h>
#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <miner.h>
#include <net.h>
#include <rpc/server.h>
#include <univalue.h>
#include <util.h>
#include <utiltime.h>
#include <validation.h>

#include <boost/thread.hpp>
#include <stdio.h>

#include <QtDebug>

extern UniValue GetNetworkHashPS(int lookup, int height);

/* NOTE: (CBigNum(1) << 256).GetHex() == "10000000000000000000000000000000000000000000000000000000000000000"
   but (arith_uint256(1) << 256).GetHex() == "00000000000000000000000000000000000000000000000000000000000000000"
*/

const arith_uint256 MaxArith256 = UintToArith256(uint256S("1000000000000000000000000000000000000000000000000000000000000000"));


MiningPage::MiningPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage),
    hasMiningprivkey(false)
{
    ui->setupUi(this);

    int nMaxUseThreads = GUIUtil::MaxThreads();

    /*
    std::string PrivAddress = gArgs.GetArg("-miningprivkey", "");
    if (!PrivAddress.empty())
    {
        CBitcoinSecret Secret;
        Secret.SetString(PrivAddress);
        if (Secret.IsValid())
        {
            CBitcoinAddress Address;
            Address.Set(Secret.GetKey().GetPubKey().GetID());
            ui->labelAddress->setText(QString("All mined coins will go to %1").arg(Address.ToString().c_str()));
            hasMiningprivkey = true;
        }
    }
    */

    ui->sliderCores->setMinimum(0);
    ui->sliderCores->setMaximum(nMaxUseThreads);
    ui->sliderCores->setValue(nMaxUseThreads);
    ui->labelNCores->setText(QString("%1").arg(nMaxUseThreads));
    ui->sliderGraphSampleTime->setMaximum(0);
    ui->sliderGraphSampleTime->setMaximum(6);

    ui->sliderCores->setToolTip(tr("Use the slider to select the amount of CPU threads to use"));
    ui->labelNetHashRate->setToolTip(tr("This shows the overall hashrate of the Dynamic network"));
    ui->labelMinerHashRate->setToolTip(tr("This shows the hashrate of your CPU whilst mining"));
    ui->labelNextBlock->setToolTip(tr("This shows the average time between the blocks you have mined"));

    isMining = gArgs.GetBoolArg("-gen", false)? 1 : 0;

    connect(ui->sliderCores, SIGNAL(valueChanged(int)), this, SLOT(changeNumberOfCores(int)));
    connect(ui->sliderGraphSampleTime, SIGNAL(valueChanged(int)), this, SLOT(changeSampleTime(int)));
    connect(ui->pushSwitchMining, SIGNAL(clicked()), this, SLOT(switchMining()));
    connect(ui->pushButtonClearData, SIGNAL(clicked()), this, SLOT(clearHashRateData()));
    connect(ui->checkBoxShowGraph, SIGNAL(stateChanged(int)), this, SLOT(showHashRate(int)));

    ui->minerHashRateWidget->graphType = HashRateGraphWidget::GraphType::MINER_HASHRATE;
    ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::FIVE_MINUTES);
    
    showHashMeterControls(false);

    updateUI(isMining);
    startTimer(8000);
}

MiningPage::~MiningPage()
{
    delete ui;
}

void MiningPage::setModel(WalletModel *model)
{
    this->model = model;
}

void MiningPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
}

void MiningPage::updateUI(bool fGenerate)
{

    qint64 NetworkHashrate = GUIUtil::GetNetworkHashPS(120, -1);
    // int64_t hashrate = (int64_t)dHashesPerSec;
    qint64 Hashrate = GUIUtil::GetHashRate();

    ui->labelNetHashRate->setText(GUIUtil::FormatHashRate(NetworkHashrate));
    ui->labelMinerHashRate->setText(GUIUtil::FormatHashRate(Hashrate));
    
    QString NextBlockTime = QChar(L'∞');
    if (Hashrate > 0)
    {
        arith_uint256 Target;
        Target.SetCompact(chainActive.Tip()->nBits);
        arith_uint256 ExpectedTime = MaxArith256 / (Target * Hashrate);
        NextBlockTime = GUIUtil::FormatTimeInterval(ExpectedTime);
    }
    ui->labelNextBlock->setText(NextBlockTime);

    if (GUIUtil::GetHashRate() == 0) {
        ui->pushSwitchMining->setToolTip(tr("Click 'Start mining' to begin mining."));
        ui->pushSwitchMining->setText(tr("Start mining."));
        ui->pushSwitchMining->setEnabled(true);
     }
     else {
        ui->pushSwitchMining->setToolTip(tr("Click 'Stop mining' to finish mining."));
        ui->pushSwitchMining->setText(tr("Stop mining."));
        ui->pushSwitchMining->setEnabled(true);
    }
    ui->pushSwitchMining->setEnabled(true);

    QString status = QString("Not Mining V Core");
    if (fGenerate)
        status = QString("Mining V Core with %1/%2 threads - hashrate: %3 (%4 hashes per sec.)")
                .arg((int)ui->sliderCores->value())
                .arg(GUIUtil::MaxThreads())
                .arg(GUIUtil::FormatHashRate(Hashrate))
                .arg(dHashesPerSec);
    ui->miningStatistics->setText(status);
}

void MiningPage::restartMining(bool fGenerate, int nThreads)
{
    isMining = fGenerate;
    if (nThreads <= maxGenProc)
        nUseThreads = nThreads;

    // unlock wallet before mining

  #ifndef __linux__
    if (fGenerate && !hasMiningprivkey && !unlockContext.get())
    {
        this->unlockContext.reset(new WalletModel::UnlockContext(model->requestUnlock()));
        if (!unlockContext->isValid())
        {
            unlockContext.reset(nullptr);
            return;
        }
    }
  #endif

    /* FIXME: gjh - mining status signalling
    clientModel->setMining(fGenerate, dHashesPerSec);
    */
    GenerateVCoins(true, nThreads, Params());

    // lock wallet after mining
    if (!fGenerate && !hasMiningprivkey)
        unlockContext.reset(nullptr);

    updateUI(fGenerate);
}

void MiningPage::StartMiner()
{
    int nThreads = (int)ui->sliderCores->value();
    GenerateVCoins(true, nThreads, Params());
    isMining = true;
    updateUI(isMining);
}

void MiningPage::StopMiner()
{
    isMining = false;
    GenerateVCoins(false, 0, Params()/*, *g_connman*/);
    updateUI(isMining);
}

void MiningPage::changeNumberOfCores(int i)
{
    // restartMining(isMining, i);

    ui->labelNCores->setText(QString("%1").arg(i));
    if (i == 0) {
        StopMiner();
    }
    else if (i > 0 && GUIUtil::GetHashRate() > 0) {  
        StartMiner();
    }
}

void MiningPage::switchMining()
{
    // restartMining(!isMining, ui->sliderCores->value());

    int64_t hashRate = GUIUtil::GetHashRate();
    int nThreads = (int)ui->sliderCores->value();
    
    if (hashRate > 0) {
        ui->pushSwitchMining->setText(tr("Stopping."));
        StopMiner();
    }
    else if (nThreads == 0 && hashRate == 0){
        ui->sliderCores->setValue(1);
        ui->pushSwitchMining->setText(tr("Starting."));
        StartMiner();
    }
    else {
        ui->pushSwitchMining->setText(tr("Starting."));
        StartMiner();
    }
}

void MiningPage::timerEvent(QTimerEvent *)
{
    updateUI(isMining);
}

void MiningPage::showHashRate(int i)
{
    if (i == 0) {
        ui->minerHashRateWidget->StopHashMeter();
        showHashMeterControls(false);
    }
    else {
        ui->minerHashRateWidget->StartHashMeter();
        showHashMeterControls(true);
    }
}

void MiningPage::showHashMeterControls(bool show)
{
    if (show == false) {
        ui->sliderGraphSampleTime->setVisible(false);
        ui->labelGraphSampleSize->setVisible(false);
        ui->pushButtonClearData->setVisible(false);
    }
    else {
        ui->sliderGraphSampleTime->setVisible(true);
        ui->labelGraphSampleSize->setVisible(true);
        ui->pushButtonClearData->setVisible(true);
    }
}

void MiningPage::changeSampleTime(int i)
{
    if (i == 0) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::FIVE_MINUTES);
        ui->labelGraphSampleSize->setText(QString("5 minutes"));
    }
    else if (i == 1) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::TEN_MINUTES);
        ui->labelGraphSampleSize->setText(QString("10 minutes"));
    }
    else if (i == 2) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::THIRTY_MINUTES);
        ui->labelGraphSampleSize->setText(QString("30 minutes"));
    }
    else if (i == 3) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::ONE_HOUR);
        ui->labelGraphSampleSize->setText(QString("1 hour"));
    }
    else if (i == 4) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::EIGHT_HOURS);
        ui->labelGraphSampleSize->setText(QString("8 hours"));
    }
    else if (i == 5) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::TWELVE_HOURS);
        ui->labelGraphSampleSize->setText(QString("12 hours"));
    }
    else if (i == 6) {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::ONE_DAY);
        ui->labelGraphSampleSize->setText(QString("1 day"));
    }
    else {
        ui->minerHashRateWidget->UpdateSampleTime(HashRateGraphWidget::SampleTime::ONE_DAY);
        ui->labelGraphSampleSize->setText(QString("1 day"));
    }
}

void MiningPage::clearHashRateData()
{
    ui->minerHashRateWidget->clear();
}
