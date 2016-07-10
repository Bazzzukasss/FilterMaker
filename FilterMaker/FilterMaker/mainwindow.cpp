#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filterDevice.setFIRFilterLength(ui->spinBox_Length->value());
    filterDevice.setFilterType(ui->comboBox_Type->currentIndex());
    filterDevice.setFIRFilterWindow(ui->comboBox_FIRWindow->currentIndex());
    filterDevice.setIIRFilterWindow(ui->comboBox_IIRWindow->currentIndex());
    filterDevice.setAverage(ui->horizontalSlider_Averag->value());
    filterDevice.setDifferential(ui->horizontalSlider_Differential->value());
    filterDevice.setFsmp(ui->spinBox_Fsmp->value());
    filterDevice.setHf(ui->horizontalSlider_HF->value());
    filterDevice.setLf(ui->horizontalSlider_LF->value());

    filterDevice.genFrequencies();

    ui->horizontalSlider_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->horizontalSlider_LF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_LF->setMaximum(ui->spinBox_Fsmp->value()/2);

    ui->horizontalSlider_Averag->setMaximum(ui->spinBox_Length->value());
    ui->horizontalSlider_Differential->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Average->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Differential->setMaximum(ui->spinBox_Length->value());

    initPlot(ui->plot_FilterCoefficientes,1,QBrush(QColor(Qt::white)),QBrush(Qt::white),"Coefficientes","",QCPGraph::lsImpulse,QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
    setPlotScale(ui->plot_FilterCoefficientes,0,filterDevice.getFilterLength()-1,-1,1);

    filterDevice.generatePulseResponce();
    initPlot(ui->plot_Data,2,QBrush(QColor(Qt::white)),QBrush(Qt::white),"Data","",QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    setPlotScale(ui->plot_Data,0,filterDevice.getDataLenBuffer().size()-1,-pow(2,ui->verticalSlider_Scale->value()),pow(2,ui->verticalSlider_Scale->value()));

    filterDevice.generateFilterRespounse();
    initPlot(ui->plot_FilterResponse,1,QBrush(QColor(Qt::white)),QBrush(QColor(0, 0, 255, 20)),"Response","",QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    setPlotScale(ui->plot_FilterResponse,0,filterDevice.getResponseLenBuffer().size()-1,0,1024*2);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_horizontalSlider_Averag_valueChanged(int value)
{
    filterDevice.setAverage(value);
    ui->spinBox_Average->setValue(value);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_comboBox_Type_currentIndexChanged(int index)
{
    filterDevice.setFilterType(index);
    setPlotScaleX(ui->plot_FilterCoefficientes,-1,filterDevice.getFilterLength());
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_comboBox_FIRWindow_currentIndexChanged(int index)
{
    filterDevice.setFIRFilterWindow(index);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}
void MainWindow::on_comboBox_IIRWindow_currentIndexChanged(int index)
{
    filterDevice.setIIRFilterWindow(index);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}
void MainWindow::on_horizontalSlider_Differential_valueChanged(int value)
{
    filterDevice.setDifferential(value);
    ui->spinBox_Differential->setValue(value);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_spinBox_Fsmp_valueChanged(int arg1)
{
    filterDevice.setFsmp(arg1);
    ui->horizontalSlider_HF->setMaximum(arg1/2);
    ui->horizontalSlider_LF->setMaximum(arg1/2);
    ui->spinBox_HF->setMaximum(arg1/2);
    ui->spinBox_LF->setMaximum(arg1/2);
    filterDevice.setHf(ui->horizontalSlider_HF->value());
    filterDevice.setLf(ui->horizontalSlider_LF->value());

    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_horizontalSlider_LF_valueChanged(int value)
{
    filterDevice.setLf(value);
    ui->spinBox_LF->setValue(value);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_horizontalSlider_HF_valueChanged(int value)
{
    filterDevice.setHf(value);
    ui->spinBox_HF->setValue(value);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}

void MainWindow::on_spinBox_Length_valueChanged(int arg1)
{
    filterDevice.setFIRFilterLength(arg1);
    setPlotScaleX(ui->plot_FilterCoefficientes,-1,filterDevice.getFilterLength());
    ui->horizontalSlider_Averag->setMaximum(arg1);
    ui->horizontalSlider_Differential->setMaximum(arg1-1);
    ui->spinBox_Average->setMaximum(arg1);
    ui->spinBox_Differential->setMaximum(arg1-1);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}
void MainWindow::on_spinBox_Passes_valueChanged(int arg1)
{
    filterDevice.setPassesNumber(arg1);
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
    redrawPlots();
}
void MainWindow::redrawData()
{
    ui->plot_Data->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getDataLenBuffer()),QVector<double>::fromStdVector(filterDevice.getInputData()));
    ui->plot_Data->graph(1)->setData( QVector<double>::fromStdVector(filterDevice.getDataLenBuffer()),QVector<double>::fromStdVector(filterDevice.getOutputData()));
    ui->plot_Data->replot();
    ui->plot_Data->graph(0)->clearData();
}
void MainWindow::redrawFilterResponse()
{
    ui->plot_FilterResponse->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getResponseLenBuffer()),QVector<double>::fromStdVector(filterDevice.getFilterResponse()));
    ui->plot_FilterResponse->replot();
    ui->plot_FilterResponse->graph(0)->clearData();
}
void MainWindow::redrawCoefficientes()
{
    ui->plot_FilterCoefficientes->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getFilterLenBuffer()),QVector<double>::fromStdVector(filterDevice.getCoefficientes()));
    ui->plot_FilterCoefficientes->replot();
    ui->plot_FilterCoefficientes->graph(0)->clearData();
}
void MainWindow::initPlot(QCustomPlot* plot,unsigned int graph_count,const QBrush& background_brush,const QBrush& graph_brush,const QString& x_label,const QString& y_label,QCPGraph::LineStyle lineStyle,QCPScatterStyle scatterStyle)
{
    plot->setBackground(background_brush);
    for(unsigned int i=0;i<graph_count;++i)
    {
        plot->addGraph();
        plot->graph(0)->setBrush(graph_brush);
        plot->graph(0)->setLineStyle(lineStyle);
        plot->graph(0)->setScatterStyle(scatterStyle);

    }
    plot->xAxis->setLabel(x_label);
    plot->yAxis->setLabel(y_label);
    plot->xAxis->setTickLabels(false);
    plot->yAxis->setTickLabels(false);
    //plot->yAxis->setVisible(false);
    //plot->xAxis->setVisible(false);
    plot->axisRect()->setupFullAxesBox();
    plot->axisRect()->setAutoMargins(QCP::msNone);
    plot->axisRect()->setMargins(QMargins(1,1,1,1));
}

void MainWindow::on_verticalSlider_Scale_valueChanged(int value)
{
    setPlotScale(ui->plot_Data,0,filterDevice.getDataLenBuffer().size()-1,-pow(2,value),pow(2,value));
    redrawData();
}




