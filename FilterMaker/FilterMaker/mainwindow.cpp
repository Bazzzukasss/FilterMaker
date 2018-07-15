#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::initialize()
{
    bool isSettingsFileExists = QFileInfo("config.ini").exists();
    mpSettings = new QSettings("config.ini",QSettings::IniFormat,this);
    if(isSettingsFileExists)
        loadSettings();

    connect(ui->horizontalSlider_Averag,        &QSlider::valueChanged, [&](){ showValues(); });
    connect(ui->horizontalSlider_Differential,  &QSlider::valueChanged, [&](){ showValues(); });
    connect(ui->horizontalSlider_HF,            &QSlider::valueChanged, [&](){ showValues(); });
    connect(ui->horizontalSlider_LF,            &QSlider::valueChanged, [&](){ showValues(); });

    connect(ui->horizontalSlider_Averag,        &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_Differential,  &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_HF,            &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_LF,            &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->comboBox_Type,                  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });
    connect(ui->comboBox_FIRWindow,             static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });
    connect(ui->comboBox_IIRWindow,             static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });
    connect(ui->spinBox_Fsmp,                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),          [&](){ apply(); });
    connect(ui->spinBox_Passes,                 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),          [&](){ apply(); });
    connect(ui->spinBox_Length,                 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),          [&](){ apply(); });

    connect(ui->verticalSlider_Scale,           &QSlider::valueChanged,     [&](int value){ setPlotScale(ui->plot_Data,0,filterDevice.getDataLenBuffer().size()-1,-pow(2,value),pow(2,value)); redrawData(); });
    connect(ui->pushButton_Copy,                &QPushButton::clicked,      [&](){ copyCoefficients(); });
    connect(ui->pushButton_Save,                &QPushButton::clicked,      [&](){ saveCoefficients(); });
    connect(ui->checkBox_Hex,                   &QCheckBox::toggled,        [&](){ showCoefficients(); });

    applyValues();
    updateUI();
    showValues();
    showCoefficients();
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

void MainWindow::loadSettings()
{
    mpSettings->beginGroup("user");
        ui->checkBox_Hex->setChecked( mpSettings->value("hexadecimal").toBool() );
        ui->verticalSlider_Scale->setValue( mpSettings->value("scale").toInt() );
    mpSettings->endGroup();
    mpSettings->beginGroup("filters");
        ui->comboBox_Type->setCurrentIndex( mpSettings->value("type").toInt() );
        ui->comboBox_FIRWindow->setCurrentIndex( mpSettings->value("firWindow").toInt() );
        ui->comboBox_IIRWindow->setCurrentIndex( mpSettings->value("iirWindow").toInt() );

        ui->spinBox_Length->setValue( mpSettings->value("length").toInt() );
        ui->spinBox_Passes->setValue( mpSettings->value("passes").toInt() );
        ui->spinBox_Fsmp->setValue( mpSettings->value("fsmp").toInt() );

        ui->horizontalSlider_Averag->setValue( mpSettings->value("average").toInt() );
        ui->horizontalSlider_Differential->setValue( mpSettings->value("differencial").toInt() );
        ui->horizontalSlider_LF->setValue( mpSettings->value("lf").toInt() );
        ui->horizontalSlider_HF->setValue( mpSettings->value("hf").toInt() );
    mpSettings->endGroup();
}

void MainWindow::saveSettings()
{
    mpSettings->beginGroup("user");
        mpSettings->setValue("hexadecimal",ui->checkBox_Hex->isChecked());
        mpSettings->setValue("scale",ui->verticalSlider_Scale->value());
    mpSettings->endGroup();
    mpSettings->beginGroup("filters");
        mpSettings->setValue("type",filterDevice.getFilterType() );
        mpSettings->setValue("firWindow",filterDevice.getFIRFilterWindow());
        mpSettings->setValue("iirWindow",filterDevice.getIIRFilterWindow());

        mpSettings->setValue("length",filterDevice.getFIRFilterLength());
        mpSettings->setValue("passes",filterDevice.getIIRFilterPasses());
        mpSettings->setValue("fsmp",filterDevice.getFsmp());

        mpSettings->setValue("average",filterDevice.getAverage());
        mpSettings->setValue("differencial",filterDevice.getDifferential());
        mpSettings->setValue("lf",filterDevice.getLF());
        mpSettings->setValue("hf",filterDevice.getHF());
    mpSettings->endGroup();
}

void MainWindow::apply()
{
    updateUI();
    applyValues();
    generateFilters();
    showCoefficients();
    redrawPlots();
}

void MainWindow::copyCoefficients()
{
    QApplication::clipboard()->setText(ui->plainTextEdit_coefficients->toPlainText());
}

void MainWindow::saveCoefficients()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save coefficients","","Text Files (*.txt)");

    if( filename.isEmpty() )
        return;

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << ui->plainTextEdit_coefficients->toPlainText();
        file.flush();
        file.close();
    }
}

void MainWindow::showCoefficients()
{
    auto coefficients = filterDevice.getCoefficients();
    ui->plainTextEdit_coefficients->clear();
    for(auto& coef : coefficients){
        QString value = ui->checkBox_Hex->isChecked() ? QString::number( (int)(coef*0xFFFF), 16 ) : QString::number( coef , 'f', 12 );
        ui->plainTextEdit_coefficients->appendPlainText( value );
    }
}

void MainWindow::applyValues()
{
    filterDevice.setPassesNumber( ui->spinBox_Passes->value() );
    filterDevice.setFIRFilterLength( ui->spinBox_Length->value() );
    filterDevice.setFilterType( ui->comboBox_Type->currentIndex() );
    filterDevice.setFIRFilterWindow( ui->comboBox_FIRWindow->currentIndex() );
    filterDevice.setIIRFilterWindow( ui->comboBox_IIRWindow->currentIndex() );
    filterDevice.setAverage (ui->horizontalSlider_Averag->value() );
    filterDevice.setDifferential( ui->horizontalSlider_Differential->value() );
    filterDevice.setFsmp( ui->spinBox_Fsmp->value() );
    filterDevice.setHf( ui->horizontalSlider_HF->value() );
    filterDevice.setLf( ui->horizontalSlider_LF->value() );
    setPlotScaleX( ui->plot_FilterCoefficientes,-1,filterDevice.getFilterLength() );
}

void MainWindow::showValues()
{
    ui->spinBox_Average->setValue(ui->horizontalSlider_Averag->value());
    ui->spinBox_Differential->setValue(ui->horizontalSlider_Differential->value());
    ui->spinBox_LF->setValue(ui->horizontalSlider_LF->value());
    ui->spinBox_HF->setValue(ui->horizontalSlider_HF->value());
}

void MainWindow::updateUI()
{
    ui->horizontalSlider_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->horizontalSlider_LF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_HF->setMaximum(ui->spinBox_Fsmp->value()/2);
    ui->spinBox_LF->setMaximum(ui->spinBox_Fsmp->value()/2);

    ui->horizontalSlider_Averag->setMaximum(ui->spinBox_Length->value());
    ui->horizontalSlider_Differential->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Average->setMaximum(ui->spinBox_Length->value());
    ui->spinBox_Differential->setMaximum(ui->spinBox_Length->value());

    ui->horizontalSlider_Averag->setEnabled(false);
    ui->horizontalSlider_Differential->setEnabled(false);
    ui->horizontalSlider_HF->setEnabled(false);
    ui->horizontalSlider_LF->setEnabled(false);

    ui->label_Average->setEnabled(false);
    ui->label_Differential->setEnabled(false);
    ui->label_LF->setEnabled(false);
    ui->label_HF->setEnabled(false);

    ui->spinBox_Length->setEnabled(false);
    ui->spinBox_Passes->setEnabled(false);
    ui->comboBox_FIRWindow->setEnabled(false);
    ui->comboBox_IIRWindow->setEnabled(false);

    ui->label_Length->setEnabled(false);
    ui->label_FIRWin->setEnabled(false);
    ui->label_Passes->setEnabled(false);
    ui->label_IIRWin->setEnabled(false);

    int filterType = ui->comboBox_Type->currentIndex();
    switch(filterType)
    {
        case FT_FIR_AVERAGE:
            ui->horizontalSlider_Averag->setEnabled(true);
            ui->label_Average->setEnabled(true);
            ui->spinBox_Length->setEnabled(true);
            ui->label_Length->setEnabled(true);
            break;
        case FT_FIR_DIFFERENTIAL:
            ui->horizontalSlider_Differential->setEnabled(true);
            ui->label_Differential->setEnabled(true);
            ui->spinBox_Length->setEnabled(true);
            ui->label_Length->setEnabled(true);
            break;
        case FT_FIR_LOPASS:
        case FT_IIR_LOPASS:
            ui->horizontalSlider_HF->setEnabled(true);
            ui->label_HF->setEnabled(true);
            if(filterType == FT_FIR_LOPASS)
            {
                ui->spinBox_Length->setEnabled(true);
                ui->comboBox_FIRWindow->setEnabled(true);
                ui->label_Length->setEnabled(true);
                ui->label_FIRWin->setEnabled(true);
            }
            else
            {
                ui->spinBox_Passes->setEnabled(true);
                ui->comboBox_IIRWindow->setEnabled(true);
                ui->label_Passes->setEnabled(true);
                ui->label_IIRWin->setEnabled(true);
            }
            break;
        case FT_FIR_HIPASS:
        case FT_IIR_HIPASS:
            ui->horizontalSlider_LF->setEnabled(true);
            ui->label_LF->setEnabled(true);
            if(filterType == FT_FIR_HIPASS)
            {
                ui->spinBox_Length->setEnabled(true);
                ui->comboBox_FIRWindow->setEnabled(true);
                ui->label_Length->setEnabled(true);
                ui->label_FIRWin->setEnabled(true);
            }
            else
            {
                ui->spinBox_Passes->setEnabled(true);
                ui->comboBox_IIRWindow->setEnabled(true);
                ui->label_Passes->setEnabled(true);
                ui->label_IIRWin->setEnabled(true);
            }
            break;
        case FT_FIR_BANDPASS:
        case FT_IIR_BANDPASS:
            ui->horizontalSlider_HF->setEnabled(true);
            ui->horizontalSlider_LF->setEnabled(true);
            ui->label_LF->setEnabled(true);
            ui->label_HF->setEnabled(true);
            if(filterType == FT_FIR_BANDPASS)
            {
                ui->spinBox_Length->setEnabled(true);
                ui->comboBox_FIRWindow->setEnabled(true);
                ui->label_Length->setEnabled(true);
                ui->label_FIRWin->setEnabled(true);
            }
            else
            {
                ui->spinBox_Passes->setEnabled(true);
                ui->comboBox_IIRWindow->setEnabled(true);
                ui->label_Passes->setEnabled(true);
                ui->label_IIRWin->setEnabled(true);
            }
            break;
        case FT_NONE:
        default:
            break;
    }
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

void MainWindow::redrawCoefficients()
{
    ui->plot_FilterCoefficientes->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getFilterLenBuffer()),QVector<double>::fromStdVector(filterDevice.getCoefficients()));
    ui->plot_FilterCoefficientes->replot();
    ui->plot_FilterCoefficientes->graph(0)->clearData();
}

void MainWindow::redrawPlots()
{
    redrawCoefficients();
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
    aPlot->axisRect()->setMargins(QMargins(1,2,2,1));
}

void MainWindow::addGraph(QCustomPlot *aPlot, const QPen& aPen, const QBrush& aBrush, QCPGraph::LineStyle aLineStyle, QCPScatterStyle aScatterStyle)
{
    auto graph = aPlot->addGraph();
    graph->setPen(aPen);
    graph->setBrush(aBrush);
    graph->setLineStyle(aLineStyle);
    graph->setScatterStyle(aScatterStyle);
}
