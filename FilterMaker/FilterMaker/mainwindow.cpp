#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    applyValues();
    setValuesLimits();
    filterDevice.genFrequencies();
    initializePlots();
}

void MainWindow::initializePlots()
{
    initPlot(ui->plot_FilterCoefficientes,QBrush(Qt::white),"Coefficientes","");
    addGraph(ui->plot_FilterCoefficientes,QPen(Qt::blue),QBrush(Qt::white),QCPGraph::lsImpulse,QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
    setPlotScale(ui->plot_FilterCoefficientes,0,filterDevice.getFilterLength()-1,-1,1);

    filterDevice.generatePulseResponce();
    initPlot(ui->plot_Data,QBrush(Qt::white),"Data","");
    addGraph(ui->plot_Data,QPen(Qt::blue),QBrush(Qt::white),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    addGraph(ui->plot_Data,QPen(Qt::red),QBrush(Qt::white),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    setPlotScale(ui->plot_Data,0,filterDevice.getDataLenBuffer().size()-1,-pow(2,ui->verticalSlider_Scale->value()),pow(2,ui->verticalSlider_Scale->value()));

    filterDevice.generateFilterRespounse();
    initPlot(ui->plot_FilterResponse,QBrush(Qt::white),"Response","");
    addGraph(ui->plot_FilterResponse,QPen(Qt::blue),QBrush(QColor(0, 0, 255, 20)),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    setPlotScale(ui->plot_FilterResponse,0,filterDevice.getResponseLenBuffer().size()-1,0,1024*2);
}

void MainWindow::applyValues()
{
    filterDevice.setFIRFilterLength(ui->spinBox_Length->value());
    filterDevice.setFilterType(ui->comboBox_Type->currentIndex());
    filterDevice.setFIRFilterWindow(ui->comboBox_FIRWindow->currentIndex());
    filterDevice.setIIRFilterWindow(ui->comboBox_IIRWindow->currentIndex());
    filterDevice.setAverage(ui->horizontalSlider_Averag->value());
    filterDevice.setDifferential(ui->horizontalSlider_Differential->value());
    filterDevice.setFsmp(ui->spinBox_Fsmp->value());
    filterDevice.setHf(ui->horizontalSlider_HF->value());
    filterDevice.setLf(ui->horizontalSlider_LF->value());

    //ui->spinBox_Average->setValue(ui->horizontalSlider_Averag->value());
    //ui->spinBox_Differential->setValue(ui->horizontalSlider_Differential->value());
    //ui->spinBox_LF->setValue(ui->horizontalSlider_LF->value());
    //ui->spinBox_HF->setValue(ui->horizontalSlider_HF->value());
}

void MainWindow::setValuesLimits()
{
    ui->horizontalSlider_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->horizontalSlider_LF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_LF->setMaximum(ui->spinBox_Fsmp->value()/2);

    ui->horizontalSlider_Averag->setMaximum(ui->spinBox_Length->value());
    ui->horizontalSlider_Differential->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Average->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Differential->setMaximum(ui->spinBox_Length->value());
}

void MainWindow::on_horizontalSlider_Averag_valueChanged(int value)
{
    filterDevice.setAverage(value);
    ui->spinBox_Average->setValue(value);
    generateFilters();
    redrawPlots();
}

void MainWindow::on_comboBox_Type_currentIndexChanged(int index)
{
    filterDevice.setFilterType(index);
    setPlotScaleX(ui->plot_FilterCoefficientes,-1,filterDevice.getFilterLength());
    generateFilters();
    redrawPlots();
}

void MainWindow::on_comboBox_FIRWindow_currentIndexChanged(int index)
{
    filterDevice.setFIRFilterWindow(index);
    generateFilters();
    redrawPlots();
}
void MainWindow::on_comboBox_IIRWindow_currentIndexChanged(int index)
{
    filterDevice.setIIRFilterWindow(index);
    generateFilters();
    redrawPlots();
}
void MainWindow::on_horizontalSlider_Differential_valueChanged(int value)
{
    filterDevice.setDifferential(value);
    ui->spinBox_Differential->setValue(value);
    generateFilters();
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

    generateFilters();
    redrawPlots();
}

void MainWindow::on_horizontalSlider_LF_valueChanged(int value)
{
    filterDevice.setLf(value);
    ui->spinBox_LF->setValue(value);
    generateFilters();
    redrawPlots();
}

void MainWindow::on_horizontalSlider_HF_valueChanged(int value)
{
    filterDevice.setHf(value);
    ui->spinBox_HF->setValue(value);
    generateFilters();
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
    generateFilters();
    redrawPlots();
}
void MainWindow::on_spinBox_Passes_valueChanged(int arg1)
{
    filterDevice.setPassesNumber(arg1);
    generateFilters();
    redrawPlots();
}

void MainWindow::on_verticalSlider_Scale_valueChanged(int value)
{
    setPlotScale(ui->plot_Data,0,filterDevice.getDataLenBuffer().size()-1,-pow(2,value),pow(2,value));
    redrawData();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(e)
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

void MainWindow::redrawPlots()
{
    redrawCoefficientes();
    redrawData();
    redrawFilterResponse();
}

void MainWindow::generateFilters()
{
    filterDevice.generateFilterRespounse();
    filterDevice.generatePulseResponce();
}

void MainWindow::setPlotScale(QCustomPlot *plot, int min_x, int max_x, int min_y, int max_y)
{
    setPlotScaleX(plot,min_x,max_x);
    setPlotScaleY(plot,min_y,max_y);
}

void MainWindow::setPlotScaleX(QCustomPlot *plot, int min_x, int max_x)
{
    plot->xAxis->setRange(min_x,max_x);
}

void MainWindow::setPlotScaleY(QCustomPlot *plot, int min_y, int max_y)
{
    plot->yAxis->setRange(min_y,max_y);
}

void MainWindow::initPlot(QCustomPlot* aPlot, const QBrush& aBackgroundBrush, const QString& aLabelX, const QString& aLabelY)
{
    aPlot->setBackground(aBackgroundBrush);
    aPlot->xAxis->setLabel(aLabelX);
    aPlot->yAxis->setLabel(aLabelY);
    aPlot->xAxis->setTickLabels(false);
    aPlot->yAxis->setTickLabels(false);
    //aPlot->yAxis->setVisible(false);
    //aPlot->xAxis->setVisible(false);
    aPlot->axisRect()->setupFullAxesBox();
    aPlot->axisRect()->setAutoMargins(QCP::msNone);
    aPlot->axisRect()->setMargins(QMargins(1,1,1,1));
}

void MainWindow::addGraph(QCustomPlot *aPlot, const QPen& aPen, const QBrush& aBrush, QCPGraph::LineStyle aLineStyle, QCPScatterStyle aScatterStyle)
{
    auto graph = aPlot->addGraph();
    graph->setPen(aPen);
    graph->setBrush(aBrush);
    graph->setLineStyle(aLineStyle);
    graph->setScatterStyle(aScatterStyle);
}
