#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filterdevice.h>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FilterDevice filterDevice;
    QSettings* mpSettings;

    void initialize();
    void initializePlots();
    void loadSettings();
    void saveSettings();

    void apply();
    void copyCoefficients();
    void saveCoefficients();
    void showCoefficients();

    void applyValues();
    void showValues();
    void updateUI();

    void resizeEvent(QResizeEvent* e);

    void redrawCoefficients();
    void redrawData();
    void redrawFilterResponse();
    void redrawPlots();

    void generateFilter();

    void setPlotScale(QCustomPlot* plot,int min_x,int max_x,int min_y,int max_y);
    void setPlotScaleX(QCustomPlot* plot,int min_x,int max_x);
    void setPlotScaleY(QCustomPlot* plot,int min_y,int max_y);
    void addGraph(QCustomPlot* aPlot, const QPen &aPen, const QBrush& aBrush, QCPGraph::LineStyle aLineStyle, QCPScatterStyle aScatterStyle);
    void initPlot(QCustomPlot* aPlot, const QBrush& aBackgroundBrush, const QString& aLabelX, const QString& aLabelY);
};

#endif // MAINWINDOW_H
