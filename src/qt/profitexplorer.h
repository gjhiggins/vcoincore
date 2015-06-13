#ifndef PROFITEXPLORER_H
#define PROFITEXPLORER_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
    class ProfitExplorer;
}

class ProfitExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit ProfitExplorer(QWidget *parent = 0);
    ~ProfitExplorer();
    void loadStakeChart(bool firstRun);

private:
    Ui::ProfitExplorer *ui;
    QTimer updateTimer;

private slots:
    virtual void updateTimer_timeout();
    void on_recomputeButton_clicked();
};

#endif // PROFITEXPLORER_H
