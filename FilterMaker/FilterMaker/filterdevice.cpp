#include "filterdevice.h"
#include <future>
#include <thread>

void FilterDevice::setFilterType(unsigned int _type)
{
    filterType=_type;
    genLenBuffer(filterLenBuffer,getFilterLength());
}

void FilterDevice::setFIRFilterLength(unsigned int _length)
{
    firFilterLength=_length;
    averageFilter.setLength(_length);
    differentialFilter.setLength(_length);
    firLopassFilter.setLength(_length);
    firHipassFilter.setLength(_length);
    firBandpassFilter.setLength(_length);
    genLenBuffer(filterLenBuffer,getFilterLength());
}

void FilterDevice::setFIRFilterWindow(unsigned int _window)
{
    firFilterWindow=_window;
    firLopassFilter.setWindow(_window);
    firHipassFilter.setWindow(_window);
    firBandpassFilter.setWindow(_window);
}

void FilterDevice::setIIRFilterWindow(unsigned int _window)
{
    iirFilterWindow=_window;
    iirLopassFilter.setWindow(_window);
    iirHipassFilter.setWindow(_window);
}

void FilterDevice::setAverage(unsigned int _average)
{
    average=_average;
    averageFilter.setAverage(_average);
}

void FilterDevice::setDifferential(unsigned int _differential)
{
    differential=_differential;
    differentialFilter.setDiff(_differential);
}

void FilterDevice::setLf(unsigned int _lf)
{
    lf=_lf;
    firHipassFilter.setFreq((double)((double)lf/(double)fsmp));
    iirHipassFilter.setFreq((double)((double)lf/(double)fsmp));
    firBandpassFilter.setLFreq((double)((double)lf/(double)fsmp));
}

void FilterDevice::setHf(unsigned int _hf)
{
    hf=_hf;
    firLopassFilter.setFreq((double)((double)hf/(double)fsmp));
    iirLopassFilter.setFreq((double)((double)hf/(double)fsmp));
    firBandpassFilter.setHFreq((double)((double)hf/(double)fsmp));
}

const vector<double> &FilterDevice::getCoefficients()
{
    switch(filterType)
    {
        case FT_FIR_AVERAGE:
            return averageFilter.getCoefficients();
            break;
        case FT_FIR_DIFFERENTIAL:
            return differentialFilter.getCoefficients();
            break;
        case FT_FIR_LOPASS:
            return firLopassFilter.getCoefficients();
            break;
        case FT_FIR_HIPASS:
            return firHipassFilter.getCoefficients();
            break;
        case FT_FIR_BANDPASS:
            return firBandpassFilter.getCoefficients();
            break;
        case FT_IIR_LOPASS:
            return iirLopassFilter.getCoefficients();
            break;
        case FT_IIR_HIPASS:
            return iirHipassFilter.getCoefficients();
        break;
    }
    return blankFilter.getCoefficients();
}

void FilterDevice::genRespounse(int beg_freq,int freq_num,vector<double>* resData)
{
    for(int i=beg_freq;i<beg_freq+freq_num;++i)
    {
        procData(freqData[i],resData[i]);
        filterResponse[i]=*max_element(resData[i].begin()+firFilterLength,resData[i].end());
    }
}

void FilterDevice::threadFunc(int f,int count)
{
    cout<<"Thread func:"<<f<<" "<<count<<endl;
}

void FilterDevice::generateFilterRespounse()
{
    #ifdef MULTI_THREAD_CALCULATION_ENABLED

        future<void> f[THREAD_NUM];
        int freq;
        for(int t=0;t<THREAD_NUM;t++)
        {
            freq=t*FREQ_PER_THREAD;
            f[t]=async(launch::async,&FilterDevice::genRespounse,this,freq,FREQ_PER_THREAD,resFreqData);
        }
        for(int t=0;t<THREAD_NUM;t++)
            f[t].get();

    #else
        genRespounse(0,FREQ_NUM,resFreqData);
    #endif
}

void FilterDevice::genFrequencies()
{
    double f_step((double)(1.0/(2.0*FREQ_NUM)));

    for(unsigned int i=0;i<FREQ_NUM;++i)
        genFreqData(freqData[i],FREQ_AMP,i*f_step,POINTS_PER_FREQ);
    genLenBuffer(responceLenBuffer,FREQ_NUM);
    filterResponse.resize(FREQ_NUM);
}

void FilterDevice::genFreqData(vector<double> &data, double amp, double freq, unsigned int points_num)
{
    int i=0;
    data.resize(points_num);
    for(auto& el:data)
    {
        el=(amp*cos(freq* i*2*M_PI));
        i++;
    }
}

void FilterDevice::genLenBuffer(vector<double> &vec, int len)
{
    vec.resize(len);
    iota(vec.begin(),vec.end(),0);
}





















void FilterDevice::generatePulseResponce()
{
    int len(getFilterLength());
    inputData.assign(len,0);
    inputData[0]=1024.0f;
    procData(inputData,outputData);
    genLenBuffer(dataLenBuffer,len);
}

void FilterDevice::procData(vector<double> &in_data, vector<double> &out_data)
{
    vector<double>* in=&in_data;
    vector<double>* out=&out_data;
    size_t size=in_data.size();
    if(size==0)
        return;
    if(out_data.size()<size)
        out_data.resize(size);
    for(unsigned int  p=0;p<passes_number;p++)
    {
        if(p>0)
        {
            in=out=&out_data;
        }


        switch(filterType)
        {
        case FT_FIR_AVERAGE:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),averageFilter);
            break;
        case FT_FIR_DIFFERENTIAL:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),differentialFilter);
            break;
        case FT_FIR_LOPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),firLopassFilter);
            break;
        case FT_FIR_HIPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),firHipassFilter);
            break;
        case FT_FIR_BANDPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),firBandpassFilter);
            break;
        case FT_IIR_LOPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),iirLopassFilter);
            break;
        case FT_IIR_HIPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),iirHipassFilter);
            break;
        case FT_IIR_BANDPASS:
            transform((*in).cbegin(),(*in).cend(),(*out).begin(),iirLopassFilter);
            transform((*out).cbegin(),(*out).cend(),(*out).begin(),iirHipassFilter);
            break;
        case FT_NONE:
        default:
            break;
        }
    }
}
