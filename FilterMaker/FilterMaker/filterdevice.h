#ifndef FILTERDEVICE_H
#define FILTERDEVICE_H

#include <filter.h>
#include <algorithm>
#include <QDebug>

using namespace Filters;
using namespace std;

enum FilterTypesEnum{FT_NONE,FT_FIR_AVERAGE,FT_FIR_DIFFERENTIAL,FT_FIR_LOPASS,FT_FIR_HIPASS,FT_FIR_BANDPASS,FT_IIR_LOPASS,FT_IIR_HIPASS,FT_IIR_BANDPASS};

#define FREQ_NUM        1024
#define POINTS_PER_FREQ 1024
#define FREQ_AMP        1024
#define THREAD_NUM      32
#define FREQ_PER_THREAD (FREQ_NUM/THREAD_NUM)

#define MULTI_THREAD_CALCULATION_ENABLED

class FilterDevice{

public:
    void setFilterType(unsigned int _type);
    void setFIRFilterLength(unsigned int _length);
    void setFIRFilterWindow(unsigned int _window);
    void setIIRFilterWindow(unsigned int _window);
    void setAverage(unsigned int _average);
    void setDifferential(unsigned int _differential);
    void setLf(unsigned int _lf);
    void setHf(unsigned int _hf);
    void setFsmp(unsigned int _fsmp){fsmp=_fsmp;}
    void setPassesNumber(unsigned int _passes_number){passes_number=_passes_number;}

    bool isFIRFilter() const { return (filterType < 6); }

    unsigned int getFilterLength() const
    {
        if(isFIRFilter())
            return firFilterLength;
        else
            return iirFilterLength;
    }
    const vector<double>& getCoefficients();
    const vector<double>& getInputData() const          {return inputData; }
    const vector<double>& getOutputData() const         {return outputData; }
    const vector<double>& getFilterResponse() const     {return filterResponse; }

    const vector<double>& getFilterLenBuffer() const    {return filterLenBuffer; }
    const vector<double>& getDataLenBuffer() const      { return dataLenBuffer; }
    const vector<double>& getResponseLenBuffer() const  { return responceLenBuffer; }

    unsigned int getFilterType() const                  { return filterType; }
    unsigned int getFIRFilterLength() const             { return firFilterLength; }
    unsigned int getFIRFilterWindow() const             { return firFilterWindow; }
    unsigned int getIIRFilterWindow() const             { return iirFilterWindow; }
    unsigned int getAverage() const                     { return average; }
    unsigned int getDifferential() const                { return differential; }
    unsigned int getFsmp() const                        { return fsmp; }
    unsigned int getLF() const                          { return lf; }
    unsigned int getHF() const                          { return hf; }
    unsigned int getIIRFilterPasses() const             { return passes_number; }

    void generateFrequencyRespounse();
    void generateFrequencies();
    void generateData();
    void procData(vector<double>& in_data,vector<double>& out_data);
    void generateLenBuffer(vector<double>& vec,int len);

private:
    unsigned int filterType;
    unsigned int firFilterLength;
    unsigned int iirFilterLength{IIR_FILTER_COEFFS_NUMBER};
    unsigned int firFilterWindow;
    unsigned int iirFilterWindow;
    unsigned int average;
    unsigned int differential;
    unsigned int fsmp;
    unsigned int lf;
    unsigned int hf;
    unsigned int passes_number{1};

    vector<double>inputData;
    vector<double>outputData;
    vector<double>filterResponse;
    vector<double>filterLenBuffer;
    vector<double>dataLenBuffer;
    vector<double>responceLenBuffer;
    vector<double>freqData[FREQ_NUM];
    vector<double>resFreqData[FREQ_NUM];

    AverageFilter averageFilter;
    DifferentialFilter differentialFilter;
    FIRHiPassFilter firHipassFilter;
    FIRLoPassFilter firLopassFilter;
    FIRBandPassFilter firBandpassFilter;
    BlankFilter blankFilter;
    IIRLoPassFilter iirLopassFilter;
    IIRHiPassFilter iirHipassFilter;

    void genFreqData(vector<double>& data,double amp,double freq,unsigned int points_num);
    void genRespounse(int beg_freq,int freq_num,vector<double>* resData);
    void close();
    void threadFunc(int f,int count);
};
#endif // FILTERDEVICE_H
