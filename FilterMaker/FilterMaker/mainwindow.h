#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filterdevice.h>
#include "qcustomplot.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_horizontalSlider_Averag_valueChanged(int value);

    void on_comboBox_Type_currentIndexChanged(int index);

    void on_comboBox_FIRWindow_currentIndexChanged(int index);

    void on_horizontalSlider_Differential_valueChanged(int value);

    void on_spinBox_Fsmp_valueChanged(int arg1);

    void on_horizontalSlider_LF_valueChanged(int value);

    void on_horizontalSlider_HF_valueChanged(int value);

    void on_spinBox_Length_valueChanged(int arg1);

    void on_verticalSlider_Scale_valueChanged(int value);

    void on_comboBox_IIRWindow_currentIndexChanged(int index);

    void on_spinBox_Passes_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    FilterDevice filterDevice;

    void initialize();
    void initializePlots();
    void applyValues();
    void setValuesLimits();
    void resizeEvent(QResizeEvent* e);

    void redrawCoefficientes();
    void redrawData();
    void redrawFilterResponse();
    void redrawPlots();

    void generateFilters();

    void setPlotScale(QCustomPlot* plot,int min_x,int max_x,int min_y,int max_y);
    void setPlotScaleX(QCustomPlot* plot,int min_x,int max_x);
    void setPlotScaleY(QCustomPlot* plot,int min_y,int max_y);
    void addGraph(QCustomPlot* aPlot, const QPen &aPen, const QBrush& aBrush, QCPGraph::LineStyle aLineStyle, QCPScatterStyle aScatterStyle);
    void initPlot(QCustomPlot* aPlot, const QBrush& aBackgroundBrush, const QString& aLabelX, const QString& aLabelY);
};

#endif // MAINWINDOW_H
