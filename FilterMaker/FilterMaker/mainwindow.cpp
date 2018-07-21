#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include "dataloader.h"

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

    connect(ui->horizontalSlider_Averag,        &QSlider::valueChanged,                                                 [&](){ showValues(); });
    connect(ui->horizontalSlider_Differential,  &QSlider::valueChanged,                                                 [&](){ showValues(); });
    connect(ui->horizontalSlider_HF,            &QSlider::valueChanged,                                                 [&](){ showValues(); });
    connect(ui->horizontalSlider_LF,            &QSlider::valueChanged,                                                 [&](){ showValues(); });

    connect(ui->horizontalSlider_Averag,        &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_Differential,  &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_HF,            &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->horizontalSlider_LF,            &QSlider::sliderReleased,                                               [&](){ apply(); });
    connect(ui->spinBox_Fsmp,                   &QSpinBox::editingFinished,                                             [&](){ apply(); });
    connect(ui->spinBox_Length,                 &QSpinBox::editingFinished,                                             [&](){ apply(); });
    connect(ui->spinBox_Passes,                 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),          [&](){ apply(); });
    connect(ui->comboBox_Type,                  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });
    connect(ui->comboBox_FIRWindow,             static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });
    connect(ui->comboBox_IIRWindow,             static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ apply(); });

    connect(ui->verticalSlider_Scale,           &QSlider::valueChanged,                                                 [&](){ scalePlots(); });
    connect(ui->horizontalScrollBar_Position,   &QSlider::valueChanged,                                                 [&](){ scalePlots(); });
    connect(ui->comboBox_Sweep,                 static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](){ scalePlots(); });

    connect(ui->pushButton_Copy,                &QPushButton::clicked,                                                  [&](){ copyCoefficients(); });
    connect(ui->pushButton_Save,                &QPushButton::clicked,                                                  [&](){ saveCoefficients(); });
    connect(ui->checkBox_Hex,                   &QCheckBox::toggled,                                                    [&](){ showCoefficients(); });

    connect(ui->pushButton_Load,                &QPushButton::clicked,                                                  [&](){ loadData();showData(); });
    connect(ui->spinBox_Frequency,              static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),          [&](){ showData(); });

    initializePlots();
    applyValues();
    filterDevice.generateFrequencies();
    filterDevice.generateFrequencyRespounse();
    filterDevice.addInputData({});
    filterDevice.addInputData({});
    loadData(mDataFilename);
    showData();
    showValues();
    showCoefficients();
    updateUI();
}

void MainWindow::initializePlots()
{
    initPlot(ui->plot_FilterCoefficientes,QBrush(Qt::white),"Coefficientes","",false,false);
    addGraph(ui->plot_FilterCoefficientes,QPen(Qt::blue),QBrush(Qt::white),QCPGraph::lsImpulse,QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    initPlot(ui->plot_DataH,QBrush(Qt::white),"","",true,true);
    addGraph(ui->plot_DataH,QPen(Qt::blue),QBrush(Qt::transparent),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    addGraph(ui->plot_DataH,QPen(Qt::red),QBrush(Qt::transparent),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot_DataH->setInteraction(QCP::iRangeDrag);
    ui->plot_DataH->setInteraction(QCP::iRangeZoom);
    ui->plot_DataH->axisRect()->setRangeDrag(Qt::Vertical);
    ui->plot_DataH->axisRect()->setRangeZoom(Qt::Vertical);

    initPlot(ui->plot_DataV,QBrush(Qt::white),"","",true,true);
    addGraph(ui->plot_DataV,QPen(Qt::blue),QBrush(Qt::transparent),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    addGraph(ui->plot_DataV,QPen(Qt::red),QBrush(Qt::transparent),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot_DataV->setInteraction(QCP::iRangeDrag);
    ui->plot_DataV->setInteraction(QCP::iRangeZoom);
    ui->plot_DataV->axisRect()->setRangeDrag(Qt::Vertical);
    ui->plot_DataV->axisRect()->setRangeZoom(Qt::Vertical);

    initPlot(ui->plot_DataXY,QBrush(Qt::white),"","",true,true);
    addCurve(ui->plot_DataXY,QPen(Qt::blue),QBrush(Qt::transparent),QCPCurve::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    addCurve(ui->plot_DataXY,QPen(Qt::red),QBrush(Qt::transparent),QCPCurve::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
    ui->plot_DataXY->setInteraction(QCP::iRangeDrag);
    ui->plot_DataXY->setInteraction(QCP::iRangeZoom);

    initPlot(ui->plot_FilterResponse,QBrush(Qt::white),"Response","",false,false);
    addGraph(ui->plot_FilterResponse,QPen(Qt::blue),QBrush(QColor(0, 0, 255, 20)),QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
}

void MainWindow::loadSettings()
{
    mpSettings->beginGroup("user");
        ui->checkBox_Hex->setChecked( mpSettings->value("hexadecimal").toBool() );
        ui->verticalSlider_Scale->setValue( mpSettings->value("scale").toInt() );
        ui->comboBox_Sweep->setCurrentIndex( mpSettings->value("sweep").toInt() );
        ui->horizontalScrollBar_Position->setMaximum( mpSettings->value("total").toInt() );
        ui->horizontalScrollBar_Position->setValue( mpSettings->value("position").toInt() );
        mDataFilename = mpSettings->value("data").toString();
        ui->spinBox_Frequency->setValue(mpSettings->value("frequency").toInt());

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
        mpSettings->setValue("sweep",ui->comboBox_Sweep->currentIndex());
        mpSettings->setValue("position",ui->horizontalScrollBar_Position->value());
        mpSettings->setValue("total",ui->horizontalScrollBar_Position->maximum());
        mpSettings->setValue("data",mDataFilename);
        mpSettings->setValue("frequency",ui->spinBox_Frequency->value());

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

void MainWindow::loadData()
{
    QString filename = QFileDialog::getOpenFileName(this,"Open results","","Eddycon Files (*.res)");

    if( filename.isEmpty() )
        return;

    loadData(filename);
}

void MainWindow::loadData(const QString &aFilename)
{
    if( mDataLoader.load(aFilename) )
        mDataFilename = aFilename;
}

void MainWindow::showData()
{
    if(mDataLoader.isDataLoaded())
    {
        auto dataX = mDataLoader.getFrequencyData( ui->spinBox_Frequency->value() ).mXData;
        auto dataY = mDataLoader.getFrequencyData( ui->spinBox_Frequency->value() ).mYData;
        filterDevice.setInputData(dataX,0);
        filterDevice.setInputData(dataY,1);
        ui->label_Data->setText( mDataFilename );
        ui->spinBox_Frequency->setMaximum( mDataLoader.getFrequenciesCount() - 1 );
        filterDevice.generateData();

        scalePlots();
        redrawPlots();
    }
}

void MainWindow::apply()
{
    updateUI();
    applyValues();
    generateData();
    showCoefficients();
    scalePlots();
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

    bool isDataLoaded = mDataLoader.isDataLoaded();
    ui->spinBox_Frequency->setEnabled(isDataLoaded);

    bool horizontalSlider_Averag = false;
    bool horizontalSlider_Differential = false;
    bool horizontalSlider_HF = false;
    bool horizontalSlider_LF = false;
    bool label_Average = false;
    bool label_Differential = false;
    bool label_LF = false;
    bool label_HF = false;
    bool spinBox_Length = false;
    bool spinBox_Passes = false;
    bool comboBox_FIRWindow = false;
    bool comboBox_IIRWindow = false;
    bool label_Length = false;
    bool label_FIRWin = false;
    bool label_Passes = false;
    bool label_IIRWin = false;

    int filterType = ui->comboBox_Type->currentIndex();
    switch(filterType)
    {
        case FT_FIR_AVERAGE:
            horizontalSlider_Averag = true;
            label_Average = true;
            spinBox_Length = true;
            label_Length = true;
            break;
        case FT_FIR_DIFFERENTIAL:
            horizontalSlider_Differential = true;
            label_Differential = true;
            spinBox_Length = true;
            label_Length = true;
            break;
        case FT_FIR_LOPASS:
        case FT_IIR_LOPASS:
            horizontalSlider_HF = true;
            label_HF = true;
            if(filterType == FT_FIR_LOPASS)
            {
                spinBox_Length = true;
                comboBox_FIRWindow = true;
                label_Length = true;
                label_FIRWin = true;
            }
            else
            {
                spinBox_Passes = true;
                comboBox_IIRWindow = true;
                label_Passes = true;
                label_IIRWin = true;
            }
            break;
        case FT_FIR_HIPASS:
        case FT_IIR_HIPASS:
            horizontalSlider_LF = true;
            label_LF = true;
            if(filterType == FT_FIR_HIPASS)
            {
                spinBox_Length = true;
                comboBox_FIRWindow = true;
                label_Length = true;
                label_FIRWin = true;
            }
            else
            {
                spinBox_Passes = true;
                comboBox_IIRWindow = true;
                label_Passes = true;
                label_IIRWin = true;
            }
            break;
        case FT_FIR_BANDPASS:
        case FT_IIR_BANDPASS:
            horizontalSlider_HF = true;
            horizontalSlider_LF = true;
            label_LF = true;
            label_HF = true;
            if(filterType == FT_FIR_BANDPASS)
            {
                spinBox_Length = true;
                comboBox_FIRWindow = true;
                label_Length = true;
                label_FIRWin = true;
            }
            else
            {
                spinBox_Passes = true;
                comboBox_IIRWindow = true;
                label_Passes = true;
                label_IIRWin = true;
            }
            break;
        case FT_NONE:
        default:
            break;
    }

    ui->horizontalSlider_Averag->setEnabled(horizontalSlider_Averag);
    ui->horizontalSlider_Differential->setEnabled(horizontalSlider_Differential);
    ui->horizontalSlider_HF->setEnabled(horizontalSlider_HF);
    ui->horizontalSlider_LF->setEnabled(horizontalSlider_LF);

    ui->label_Average->setEnabled(label_Average);
    ui->label_Differential->setEnabled(label_Differential);
    ui->label_LF->setEnabled(label_LF);
    ui->label_HF->setEnabled(label_HF);

    ui->spinBox_Length->setEnabled(spinBox_Length);
    ui->spinBox_Passes->setEnabled(spinBox_Passes);
    ui->comboBox_FIRWindow->setEnabled(comboBox_FIRWindow);
    ui->comboBox_IIRWindow->setEnabled(comboBox_IIRWindow);

    ui->label_Length->setEnabled(label_Length);
    ui->label_FIRWin->setEnabled(label_FIRWin);
    ui->label_Passes->setEnabled(label_Passes);
    ui->label_IIRWin->setEnabled(label_IIRWin);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(e)
        redrawPlots();
}

void MainWindow::redrawHVPlot()
{
    ui->plot_DataH->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getIODataLenBuffer(0)),QVector<double>::fromStdVector(filterDevice.getInputData(0)));
    ui->plot_DataH->graph(1)->setData( QVector<double>::fromStdVector(filterDevice.getIODataLenBuffer(0)),QVector<double>::fromStdVector(filterDevice.getOutputData(0)));
    ui->plot_DataH->replot();

    ui->plot_DataV->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getIODataLenBuffer(1)),QVector<double>::fromStdVector(filterDevice.getInputData(1)));
    ui->plot_DataV->graph(1)->setData( QVector<double>::fromStdVector(filterDevice.getIODataLenBuffer(1)),QVector<double>::fromStdVector(filterDevice.getOutputData(1)));
    ui->plot_DataV->replot();
}

void MainWindow::redrawXYPlot()
{
    int sweep = ui->comboBox_Sweep->currentText().toInt();
    int hPosition = ui->horizontalScrollBar_Position->value();

    auto dataX = filterDevice.getInputData(0);
    auto dataY = filterDevice.getInputData(1);
    auto dataXProc = filterDevice.getOutputData(0);
    auto dataYProc = filterDevice.getOutputData(1);

    int total = dataX.size();
    int beg = hPosition;
    int end = hPosition + sweep;
    if(end >= total)
        end = total - 1;
    if(total == 0)
        return;
    std::vector<double> dataXY_H(&dataX[beg], &dataX[end]);
    std::vector<double> dataXY_V(&dataY[beg], &dataY[end]);

    total = dataXProc.size();
    if(end >= total)
        end = total - 1;

    std::vector<double> dataXY_HProc(&dataXProc[beg], &dataXProc[end]);
    std::vector<double> dataXY_VProc(&dataYProc[beg], &dataYProc[end]);

    reinterpret_cast<QCPCurve*>(ui->plot_DataXY->plottable(0))->setData( QVector<double>::fromStdVector(dataXY_H),QVector<double>::fromStdVector(dataXY_V));
    reinterpret_cast<QCPCurve*>(ui->plot_DataXY->plottable(1))->setData( QVector<double>::fromStdVector(dataXY_HProc),QVector<double>::fromStdVector(dataXY_VProc));
    ui->plot_DataXY->replot();
}

void MainWindow::redrawFilterResponsePlot()
{
    ui->plot_FilterResponse->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getResponseLenBuffer()),QVector<double>::fromStdVector(filterDevice.getFilterResponse()));
    ui->plot_FilterResponse->replot();
}

void MainWindow::redrawCoefficientsPlot()
{
    ui->plot_FilterCoefficientes->graph(0)->setData( QVector<double>::fromStdVector(filterDevice.getFilterLenBuffer()),QVector<double>::fromStdVector(filterDevice.getCoefficients()));
    ui->plot_FilterCoefficientes->replot();
}

void MainWindow::redrawPlots()
{
    redrawCoefficientsPlot();
    redrawHVPlot();
    redrawXYPlot();
    redrawFilterResponsePlot();
}

void MainWindow::scalePlots()
{
    int vScale = ui->verticalSlider_Scale->value();
    int sweep = ui->comboBox_Sweep->currentText().toInt();

    ui->horizontalScrollBar_Position->setPageStep( sweep );
    ui->horizontalScrollBar_Position->setMaximum( filterDevice.getIODataLenBuffer(0).size() - 1 );

    int hPosition = ui->horizontalScrollBar_Position->value();

    setPlotScale(ui->plot_DataH, hPosition, hPosition + sweep, -pow(2,vScale), pow(2,vScale));
    setPlotScale(ui->plot_DataV, hPosition, hPosition + sweep, -pow(2,vScale), pow(2,vScale));
    setPlotScale(ui->plot_DataXY, -pow(2,vScale), pow(2,vScale), -pow(2,vScale), pow(2,vScale));
    setPlotScaleX(ui->plot_FilterCoefficientes,-1,filterDevice.getFilterLength());
    setPlotScale(ui->plot_FilterResponse,0,filterDevice.getResponseLenBuffer().size()-1,0,1024*2);
    setPlotScale(ui->plot_FilterCoefficientes,0,filterDevice.getFilterLength()-1,-1,1);

    ui->plot_DataH->replot();
    ui->plot_DataV->replot();

    redrawXYPlot();
}

void MainWindow::generateData()
{
    filterDevice.generateFrequencyRespounse();
    filterDevice.generateData();
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

void MainWindow::initPlot(QCustomPlot* aPlot, const QBrush& aBackgroundBrush, const QString& aLabelX, const QString& aLabelY, bool aTicksX, bool aTicksY)
{
    aPlot->setBackground(aBackgroundBrush);
    aPlot->xAxis->setLabel(aLabelX);
    aPlot->yAxis->setLabel(aLabelY);
    aPlot->xAxis->setTickLabels(aTicksX);
    aPlot->yAxis->setTickLabels(aTicksY);
    //aPlot->yAxis->setVisible(false);
    //aPlot->xAxis->setVisible(false);
    aPlot->axisRect()->setupFullAxesBox();
    if(!(aTicksX || aTicksY))
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

void MainWindow::addCurve(QCustomPlot *aPlot, const QPen& aPen, const QBrush& aBrush, QCPCurve::LineStyle aLineStyle, QCPScatterStyle aScatterStyle)
{
    auto curve = new QCPCurve(aPlot->xAxis, aPlot->yAxis);
    aPlot->addPlottable(curve);
    curve->setPen(aPen);
    curve->setBrush(aBrush);
    curve->setLineStyle(aLineStyle);
    curve->setScatterStyle(aScatterStyle);
}

