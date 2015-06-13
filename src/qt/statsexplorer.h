#ifndef STATSEXPLORER_H
#define STATSEXPLORER_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
    class StatsExplorer;
}

class StatsExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit StatsExplorer(QWidget *parent = 0);
    ~StatsExplorer();
    void loadStakeChart(bool firstRun);

private:
    Ui::StatsExplorer *ui;
    QTimer updateTimer;

private slots:
    virtual void updateTimer_timeout();
    void on_recomputeButton_clicked();
};

#endif // STATSEXPLORER_H
