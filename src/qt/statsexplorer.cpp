#include "statsexplorer.h"
#include "ui_statsexplorer.h"
#include "rpcserver.h"
#include "wallet/wallet.h"
#include "guiutil.h"
#include "guiconstants.h"

#include "init.h"
#include "main.h"

#include <QAbstractItemDelegate>
#include <QPainter>

extern unsigned int nTargetSpacing;

QVector<double> nTimeData(0), myStakeData(0), netStakeData(0), difficultyData(0);
QVector<double> velTimeData(0), velAmountData(0);

StatsExplorer::StatsExplorer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsExplorer)
{
    ui->setupUi(this);
    loadStakeChart(true);

    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateTimer_timeout()));
    updateTimer.setInterval(60000); // every 60 seconds (approx 2 blocks)
    updateTimer.start();
}

void StatsExplorer::updateTimer_timeout()
{
    // if(fShutdown)
    // return;

    if(ui->checkBox->isChecked())
        loadStakeChart(false);
}

void StatsExplorer::on_recomputeButton_clicked()
{
    loadStakeChart(false);
}

/*
void OverviewPage::updatePlot()
{
    static int64_t lastUpdate = 0;
    if (GetTime() - lastUpdate < 60) { return; } // This is just so it doesn't redraw too rapidly during syncing
    int numLookBack = 4320;
    int64_t hashMax = 0;
    double diffMax = 0;
    CBlockIndex* pindex = pindexBest;
    int height = nBestHeight;
    int xStart = std::max<int>(height-numLookBack, 0) + 1;
    int xEnd = height;
    // Start at the end and walk backwards
    int i = numLookBack-1;
    int x = xEnd;
    // This should be a noop if the size is already 2000
    vX.resize(numLookBack);
    vY.resize(numLookBack);
    CBlockIndex* itr = pindex;
    while(i >= 0 && itr != NULL)
    {
        vX[i] = itr->nHeight;
        vY[i] = GetDifficulty(itr);
        diffMax = std::max<double>(diffMax, vY[i]);
        itr = itr->pprev;
        i--;
        x--;
    }
    ui->diffplot->graph(0)->setData(vX, vY);

    // set axes ranges, so we see all data:
    ui->diffplot->xAxis->setRange((double)xStart, (double)xEnd);
    ui->diffplot->yAxis->setRange(0, diffMax+(diffMax/10));
    ui->diffplot->xAxis->setAutoSubTicks(false);
    ui->diffplot->yAxis->setAutoSubTicks(false);
    ui->diffplot->xAxis->setSubTickCount(0);
    ui->diffplot->yAxis->setSubTickCount(0);
    ui->diffplot->replot();

    lastUpdate = GetTime();
}

void OverviewPage::updatePlot(int count)
{
    static int64_t lastUpdate = 0;
    if (GetTime() - lastUpdate < 60) { return; } // This is just so it doesn't redraw rapidly during syncing
    int numLookBack = 4320;
    int64_t hashMax = 0;
    int64_t diffMax = 0;
    CBlockIndex* pindex = chainActive.Tip();
    int height = chainActive.Height();
    int xStart = std::max<int>(height-numLookBack, 0) + 1;
    int xEnd = height;

    // Start at the end and walk backwards
    int i = numLookBack-1;
    int x = xEnd;

    // This should be a noop if the size is already 2000
    vX.resize(numLookBack);
    vY.resize(numLookBack);
    vZ.resize(numLookBack);

    CBlockIndex* itr = pindex;
    while(i >= 0 && itr != NULL)
    {
        vX[i] = itr->nHeight;
        vY[i] = GetNetworkHashPS(120, vX[i]).get_int64();
        vZ[i] = GetDifficulty(vX[i]).get_int64();
        diffMax = std::max<int64_t>(diffMax, vY[i]);
        hashMax = std::max<int64_t>(hashMax, vY[i]);

        itr = itr->pprev;
        i--;
        x--;
    }

    ui->hashplot->graph(0)->setData(vX, vY);
    // set axes ranges, so we see all data:
    ui->hashplot->xAxis->setRange((double)xStart, (double)xEnd);
    ui->hashplot->yAxis->setRange(0, hashMax+(hashMax/10));
    ui->hashplot->xAxis->setAutoSubTicks(false);
    ui->hashplot->yAxis->setAutoSubTicks(false);
    ui->hashplot->xAxis->setSubTickCount(0);
    ui->hashplot->yAxis->setSubTickCount(0);
    ui->hashplot->replot();

    ui->diffplot->graph(0)->setData(vX, vZ);
    // set axes ranges, so we see all data:
    ui->diffplot->xAxis->setRange((double)xStart, (double)xEnd);
    ui->diffplot->yAxis->setRange(0, diffMax+(diffMax/10));
    ui->diffplot->xAxis->setAutoSubTicks(false);
    ui->diffplot->yAxis->setAutoSubTicks(false);
    ui->diffplot->xAxis->setSubTickCount(0);
    ui->diffplot->yAxis->setSubTickCount(0);
    ui->diffplot->replot();

}

*/


void StatsExplorer::loadStakeChart(bool firstRun)
{
    // if(fShutdown)
    // return;

    nTimeData.clear();
    netStakeData.clear();
    myStakeData.clear();
    difficultyData.clear();
    velTimeData.clear();
    velAmountData.clear();

    // go back this many blocks max
    int max = ui->spinBox->value();
    int i = 0;
    int64_t diffMax = 0;
    // int64_t hashMax = 0;
    double velMax = 0;

    BOOST_FOREACH(const PAIRTYPE(uint256, CBlockIndex*)& item, mapBlockIndex)
    {

        if(i >= max)
            break;

        CBlockIndex* pindex = item.second;
        nTimeData.append(pindex->nTime);
        netStakeData.append(0);
        velTimeData.append(pindex->nTime);

        // if(pindex->IsProofOfStake())
        if (true)
        {
            // netStakeData.append(pindex->nMint / COIN);
            netStakeData.append(0);

            // Read the block in and check if the coinstake is ours
            CBlock block;
            ReadBlockFromDisk(block, pindex);

            // if(block.IsProofOfStake()) // this should always be true here
            if(true) // this should always be true here
            {
                velTimeData.append(pindex->nTime);
                double blockOutAmount = 0;
                for(uint j=0; j<block.vtx.size(); j++)
                {
                    blockOutAmount += block.vtx[j].GetValueOut() / COIN;
                }
                velMax = std::max<double>(velMax, blockOutAmount);
                velAmountData.append(blockOutAmount);
                int64_t d = GetDifficulty(pindex);
                diffMax = std::max<int64_t>(diffMax, d);
                difficultyData.append(d);
                // if(IsMine(block.vtx[1]))
                if(false)
                {
                    // myStakeData.append(pindex->nMint / COIN);
                    myStakeData.append(0);
                }
                else
                {
                    myStakeData.append(0);
                }
            }
            else
            {
                myStakeData.append(0); // should never happen
            }
        }
        ++i;
    }

    if(!firstRun)
    {
        uint64_t nMinWeight = 0, nMaxWeight = 0, nWeight = 0;
        nWeight = nMinWeight * nMaxWeight;
        // pwalletMain->GetStakeWeight(*pwalletMain, nMinWeight, nMaxWeight, nWeight);

        uint64_t nNetworkWeight = 0;
        // if(pindexBest)
        //    nNetworkWeight = GetPoSKernelPS();
        // bool staking = nLastCoinStakeSearchInterval && nWeight;
        bool staking = false;
        // int nExpectedTime = staking ? (nTargetSpacing * nNetworkWeight / nWeight) : -1;
        /*
        ui->stakingLabel->setText(staking ? "Enabled" : "Disabled");
        bool pindexBest = true;
        if(pindexBest)
            ui->difficultyLabel->setText(QString::number(GetDifficulty(mapBlockIndex[chainActive.Tip()->GetBlockHash()])));
        ui->weightLabel->setText(QString::number(nWeight));
        ui->netWeightLabel->setText(QString::number(nNetworkWeight));
        ui->timeToStakeLabel->setText(QString::number(nExpectedTime) + " secs");
        */
    }

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(10, 10, 10));
    plotGradient.setColorAt(1, QColor(0, 0, 0));

    QLinearGradient diffPlotGradient;
    diffPlotGradient.setStart(0, 0);
    diffPlotGradient.setFinalStop(0, 350);
    diffPlotGradient.setColorAt(0, QColor(10, 10, 10));
    diffPlotGradient.setColorAt(1, QColor(0, 0, 0));

    QLinearGradient velPlotGradient;
    velPlotGradient.setStart(0, 0);
    velPlotGradient.setFinalStop(0, 150);
    velPlotGradient.setColorAt(0, QColor(10, 10, 10));
    velPlotGradient.setColorAt(1, QColor(0, 0, 0));

    ui->difficultyPlot->clearPlottables();
    ui->difficultyPlot->clearGraphs();
    ui->difficultyPlot->clearItems();
    ui->difficultyPlot->addGraph();
    ui->difficultyPlot->graph(0)->setPen(QPen(QColor(43, 239, 209))); // line color orange for first graph
    ui->difficultyPlot->graph(0)->setBrush(QBrush(QColor(43, 239, 209, 20))); // first orange will be filled with translucent green
    ui->difficultyPlot->graph(0)->setData(nTimeData, difficultyData);
    ui->difficultyPlot->setBackground(diffPlotGradient);
    // ui->difficultyPlot->xAxis->setRangeLower(nTimeData.first());
    // ui->difficultyPlot->xAxis->setRangeUpper(nTimeData.last());
    ui->difficultyPlot->yAxis->setRangeLower(0);
    ui->difficultyPlot->yAxis->setRangeUpper(diffMax+(diffMax/10));
    ui->difficultyPlot->xAxis->grid()->setVisible(false);
    ui->difficultyPlot->yAxis->grid()->setVisible(false);
    ui->difficultyPlot->xAxis->grid()->setSubGridVisible(false);
    ui->difficultyPlot->yAxis->grid()->setSubGridVisible(false);
    ui->difficultyPlot->xAxis->setAutoTickStep(false);
    ui->difficultyPlot->xAxis->setTickStep(3600 * 24); // 24 hr tickstep
    ui->difficultyPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->difficultyPlot->xAxis->setDateTimeSpec(Qt::UTC);
    ui->difficultyPlot->xAxis->setDateTimeFormat("dd MMM");
    ui->difficultyPlot->xAxis->setTickLabelRotation(33);
    ui->difficultyPlot->xAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->difficultyPlot->yAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->difficultyPlot->rescaleAxes();
    ui->difficultyPlot->yAxis->setTickStep(0.00005);
    ui->difficultyPlot->xAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->difficultyPlot->yAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->difficultyPlot->yAxis->setLabel("Difficulty");
    ui->difficultyPlot->xAxis->setTickLabels(true);
    //ui->difficultyPlot->xAxis->setLabel("Time");
    ui->difficultyPlot->replot();

    /*
    ui->customPlot->clearPlottables();
    ui->customPlot->clearGraphs();
    ui->customPlot->clearItems();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(QColor(43, 239, 209))); // line color cream for first graph
    ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 255, 139, 20))); // first graph will be filled with translucent cream
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(QColor(43, 239, 209))); // line color orange for second graph
    ui->customPlot->graph(1)->setBrush(QBrush(QColor(0, 255, 139, 20)));

    if(ui->networkCheckBox->isChecked())
        ui->customPlot->graph(0)->setData(nTimeData, netStakeData);
    ui->customPlot->graph(1)->setData(nTimeData, myStakeData);
    ui->customPlot->setBackground(plotGradient);
    ui->customPlot->xAxis->setRangeLower(nTimeData.first());
    ui->customPlot->xAxis->setRangeUpper(nTimeData.last());
    ui->customPlot->xAxis->grid()->setVisible(false);
    ui->customPlot->yAxis->grid()->setVisible(false);
    ui->customPlot->xAxis->grid()->setSubGridVisible(false);
    ui->customPlot->yAxis->grid()->setSubGridVisible(false);
    ui->customPlot->xAxis->setAutoTickStep(false);
    ui->customPlot->xAxis->setTickStep(3600 * 24); // 24 hr tickstep
    ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot->xAxis->setDateTimeSpec(Qt::UTC);
    ui->customPlot->xAxis->setDateTimeFormat("dd. MMM hh:mm");
    ui->customPlot->xAxis->setTickLabelRotation(33);
    ui->customPlot->xAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->customPlot->yAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->customPlot->rescaleAxes();
    ui->customPlot->xAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->customPlot->yAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->customPlot->yAxis->setLabel("VCN Minted");
    ui->customPlot->xAxis->setLabel("Stake Block Generation Time");
    ui->customPlot->replot();
    */

    ui->velocityPlot->clearPlottables();
    ui->velocityPlot->clearGraphs();
    ui->velocityPlot->clearItems();
    ui->velocityPlot->addGraph();
    ui->velocityPlot->graph(0)->setPen(QPen(QColor(43, 239, 209))); // line color orange for first graph
    ui->velocityPlot->graph(0)->setBrush(QBrush(QColor(43, 239, 209, 20))); // first graph will be filled with translucent orange
    ui->velocityPlot->graph(0)->setData(velTimeData, velAmountData);
    ui->velocityPlot->setBackground(velPlotGradient);
    // ui->velocityPlot->xAxis->setRangeLower(velTimeData.first());
    // ui->velocityPlot->xAxis->setRangeUpper(velTimeData.last());
    ui->velocityPlot->yAxis->setRangeLower(0);
    ui->velocityPlot->yAxis->setRangeUpper(velMax+(velMax/10));
    ui->velocityPlot->xAxis->grid()->setVisible(false);
    ui->velocityPlot->yAxis->grid()->setVisible(false);
    ui->velocityPlot->xAxis->grid()->setSubGridVisible(false);
    ui->velocityPlot->yAxis->grid()->setSubGridVisible(false);
    ui->velocityPlot->xAxis->setAutoTickStep(false);
    ui->velocityPlot->xAxis->setTickStep(3600 * 24); // 24 hr tickstep
    ui->velocityPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->velocityPlot->xAxis->setDateTimeSpec(Qt::UTC);
    ui->velocityPlot->xAxis->setDateTimeFormat("dd MMM");
    ui->velocityPlot->xAxis->setTickLabelRotation(33);
    ui->velocityPlot->xAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->velocityPlot->yAxis->setTickLabelColor(QColor(43, 239, 209, 95));
    ui->velocityPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->velocityPlot->yAxis->setTickStep(1000);
    ui->velocityPlot->xAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->velocityPlot->yAxis->setLabelColor(QColor(43, 239, 209, 95));
    ui->velocityPlot->yAxis->setLabel("VCN");
    ui->velocityPlot->xAxis->setTickLabels(true);
    //ui->velocityPlot->xAxis->setLabel("Financial Velocity");
    ui->velocityPlot->rescaleAxes();
    ui->velocityPlot->replot();

}

StatsExplorer::~StatsExplorer()
{
    delete ui;
}
