#ifndef FILTER
#define FILTER

#include <vector>
#include <deque>
#include <iostream>
#include <qmath.h>
using namespace std;
namespace ars{
    enum FIRWinFunction {WFT_NONE,WFT_HAMMING,WFT_HANNING,WFT_BLACKMAN,WFT_SIN,WFT_NUTTAL};
    enum IIRWinFunction {WFT_BUTTERWORTH,WTF_CRITICAL_DAMPED,WTF_BESSEL};
    #define FIR_FILTER_LENGTH   128
    #define IIR_PASSES_NUMBER   2
    #define IIR_FILTER_COEFFS_NUMBER    5

    class Filter
    {
        public:
            Filter(){}
            virtual ~Filter(){}
            friend ostream& operator<<(ostream& stream,const Filter& rhv)
            {
                stream<<"Filter:";
                for(auto el:rhv.coefficientes)
                    stream<<el<<" ";
                stream<<endl;
                return stream;
            }
            const vector<double>& getCoefficientes(){return coefficientes;}
        protected:
            deque<double> data;
            vector<double> coefficientes;
    };
///IIR Filters

    class IIRFilter:public Filter
    {
        public:
        IIRFilter(double _freq,unsigned int _passes,unsigned int _window):freq(_freq),passes(_passes),window(_window){}
            virtual ~IIRFilter(){}
        /*
                double result(0);
                data.pop_back();
                data.push_front(_value);
                deque<double>::const_iterator it=data.cbegin();
                for(const auto& el:coefficientes)
                    result+=(*it++)*el;
                return result;
*/
            double operator()(double _value)
            {
                double result(0);
                data.push_front(_value);
                data.pop_back();
                deque<double>::const_iterator it=data.cbegin();
                for(const auto& el:coefficientes)
                    result+=(*it++)*el;
                data[2]=result;
                return result;
            }
        protected:
            double freq;
            unsigned int passes;
            unsigned int window;
            unsigned int length{IIR_FILTER_COEFFS_NUMBER};
            double c,g,p,f;
            int sign;
            double K[3],B[3],A[3];
            void calculate_coeffs()
            {
                double w0,max=0;
                coefficientes.resize(5);
                w0=tan(M_PI*f);
                K[1]=p*w0;
                K[2]=g*pow(w0,2.0);

                A[0]=(K[2]/(1+K[1]+K[2]));
                A[1]=(2*A[0]);
                A[2]=(A[0]);
                B[1]=(2*A[0]*(1/K[2]-1));
                B[2]=(1-(A[0]+A[1]+A[2]+B[1]));

                coefficientes[0]=       A[0];
                coefficientes[1]=sign*  A[1];
                coefficientes[2]=       A[2];
                coefficientes[3]=sign*  B[1];
                coefficientes[4]=       B[2];

                for(auto el:coefficientes)
                    if(max<fabs(el))
                        max=el;
                if(max>1.0)
                    for(auto& el:coefficientes)
                        el/=max;
            }
    };
    class IIRLoPassFilter:public IIRFilter
    {
        public:
            IIRLoPassFilter(double _freq=0.5,unsigned int _passes=IIR_PASSES_NUMBER,unsigned int _window=IIRWinFunction::WFT_BUTTERWORTH):IIRFilter(_freq,_passes,_window)
            {
                generate();
            }
            void setFreq(double _freq)
            {
                freq=(_freq>0.5)?0.5:_freq;
                generate();
            }
            void setWindow(unsigned int _window)
            {
                window=_window;
                generate();
            }
            void setPasses(unsigned int _passes)
            {
                passes=_passes;
                generate();
            }

        private:
            void generate()
            {
                sign=1;
                data.assign(length,0);
                calculate_cgp();
                calculate_f();
                calculate_coeffs();
            }
            void calculate_f()
            {
                f=c*freq;
                /*
                cout<<"\tConditions: "<<0<<"<"<<f<<"<"<<1.0/8.0<<endl;
                if(f>=1.0/4.0)
                    cout<<"\tUnstable filter: f="<<dec<<f<<">="<<1.0/4.0<<endl;
                else
                    cout<<"\tStable filter: f="<<dec<<f<<"<="<<1.0/4.0<<endl;
                */
            }
            void calculate_cgp()
            {
                switch(window)
                {
                    case WFT_BUTTERWORTH:
                        c=pow( (pow(2.0,1.0/passes)-1), -1.0/4.0);
                        g=1;
                        p=sqrt(2);
                        break;
                    case WTF_CRITICAL_DAMPED:
                        c=pow( (pow(2.0,1.0/(2.0*passes))-1), -1.0/2.0);
                        g=1;
                        p=2;
                        break;
                    case WTF_BESSEL:
                        c=pow( (pow( (pow(2.0,1.0/passes)-3.0/4.0), 1.0/2.0 ) - 1.0/2.0) , -1.0/2.0) / sqrt(3.0);
                        g=3;
                        p=3;
                        break;
                }
            }
    };
    class IIRHiPassFilter:public IIRFilter
    {
        public:
            IIRHiPassFilter(double _freq=0.0,unsigned int _passes=IIR_PASSES_NUMBER,unsigned int _window=IIRWinFunction::WFT_BUTTERWORTH):IIRFilter(_freq,_passes,_window)
            {
                generate();
            }
            void setFreq(double _freq)
            {
                freq=(_freq>0.5)?0.5:_freq;
                generate();
            }
            void setWindow(unsigned int _window)
            {
                window=_window;
                generate();
            }
            void setPasses(unsigned int _passes)
            {
                passes=_passes;
                generate();
            }

        private:
            void generate()
            {
                sign=-1;
                data.assign(length,0);
                calculate_cgp();
                calculate_f();
                calculate_coeffs();
            }
            void calculate_f()
            {
                f=1.0/2.0-c*freq;
                /*
                cout<<"\tConditions: "<<3.0/8.0<<"<"<<f<<"<"<<1.0/2.0<<endl;
                if(f<=1.0/4.0)
                    cout<<"\tUnstable filter: f="<<dec<<f<<"<="<<1.0/4.0<<endl;
                else
                    cout<<"\tStable filter: f="<<dec<<f<<">="<<1.0/4.0<<endl;
                */
            }
            void calculate_cgp()
            {
                switch(window)
                {
                    case WFT_BUTTERWORTH:
                        c=pow( (pow(2.0,1.0/passes)-1), 1.0/4.0);
                        g=1;
                        p=sqrt(2);
                        break;
                    case WTF_CRITICAL_DAMPED:
                        c=pow( (pow(2.0,1.0/(2.0*passes))-1), 1.0/2.0);
                        g=1;
                        p=2;
                        break;
                    case WTF_BESSEL:
                        c=pow( (pow( (pow(2.0,1.0/passes)-3.0/4.0), 1.0/2.0 ) - 1.0/2.0) , 1.0/2.0)* sqrt(3.0);
                        g=3;
                        p=3;
                        break;
                }
            }
    };

///FIR Filters
    class FIRFilter:public Filter
    {
        public:
            FIRFilter(unsigned int _length,unsigned int _window):length(_length),window(_window){}
            virtual ~FIRFilter(){}
            double operator()(double _value)
            {
                double result(0);
                data.pop_back();
                data.push_front(_value);
                deque<double>::const_iterator it=data.cbegin();
                for(const auto& el:coefficientes)
                    result+=(*it++)*el;
                return result;
            }
        protected:
            unsigned int length;
            unsigned int window;
            double windowFunction(int n)
            {
                    if(n==0)
                        return (1);
                    switch(window)
                    {
                        case(WFT_HAMMING):       return(0.54+0.46*cos(2*M_PI*n/length));     break;
                        case(WFT_HANNING):       return(0.5+0.5*cos(2*M_PI*n/length));       break;
                        case(WFT_BLACKMAN):      return(0.42+0.5*cos(2*M_PI*n/(length-1))+0.08*cos(4*M_PI*n/(length-1)));break;
                        case(WFT_SIN):           return(cos(M_PI*n/(length-1)));             break;
                        case(WFT_NUTTAL):        return(1-(0.355768-0.487396*cos(2*M_PI*n/(length-1))+0.144233*cos(4*M_PI*n/(length-1))-0.012604*cos(6*M_PI*n/(length-1))));break;
                        case(WFT_NONE):
                        default:
                            return(1);
                        break;
                    }
                    return 0;
            }

    };
    class BlankFilter:public FIRFilter
    {
        public:
            BlankFilter():FIRFilter(0,FIRWinFunction::WFT_NONE){};
    };

    class AverageFilter:public FIRFilter
    {
        public:
            AverageFilter(unsigned int _average=1,unsigned int _length=FIR_FILTER_LENGTH,unsigned int _window=FIRWinFunction::WFT_NONE):FIRFilter(_length,_window),average(_average)
            {
                generate();
            }
            void set(unsigned int _average,unsigned int _length)
            {
                average=_average;
                length=_length;
                generate();
            }
            void setAverage(unsigned int _average)
            {
                average=_average;
                generate();
            }
            void setLength(unsigned int _length)
            {
                length=_length;
                generate();
            }
        private:
            unsigned int average;
            void generate()
            {
                if(average==0)average=1;
                if(average>length)average=length;
                data.assign(length,0);
                coefficientes.resize(length);
                vector<double>::iterator beg_it=coefficientes.begin()+(length-average)/2;
                vector<double>::iterator end_it=beg_it+average;
                std::fill(coefficientes.begin(),beg_it,0);
                std::fill(beg_it,end_it,1.0f/average);
                std::fill(end_it,coefficientes.end(),0);
            }
    };

    class DifferentialFilter:public FIRFilter
    {
        public:
            DifferentialFilter(unsigned int _diff=1,unsigned int _length=FIR_FILTER_LENGTH,unsigned int _window=FIRWinFunction::WFT_NONE):FIRFilter(_length,_window),diff(_diff)
            {
                generate();
            }
            void set(unsigned int _diff,unsigned int _length)
            {
                diff=_diff;
                length=_length;
                generate();
            }
            void setDiff(unsigned int _diff)
            {
                diff=_diff;
                generate();
            }
            void setLength(unsigned int _length)
            {
                length=_length;
                generate();
            }
        private:
            unsigned int diff;
            void generate()
            {
                if(diff>length-1)diff=length-1;
                unsigned beg((length-diff)/2),end(beg+diff);

                data.assign(length,0);
                coefficientes.assign(length,0);
                coefficientes[end]=-1.0f;
                coefficientes[beg]=1.0f;
            }
    };

    class FIRBandPassFilter:public FIRFilter
    {
        public:
            FIRBandPassFilter(double _lfreq=0.0,double _hfreq=0.5,unsigned int _length=FIR_FILTER_LENGTH,unsigned int _window=FIRWinFunction::WFT_NONE):FIRFilter(_length,_window),lfreq(_lfreq),hfreq(_hfreq)
            {
                generate();
            }
            void setBandFreq(double _lfreq,double _hfreq)
            {
                hfreq=(_hfreq>0.5)?0.5:_hfreq;
                lfreq=(_lfreq>0.5)?0.5:_lfreq;
                generate();
            }
            void setLength(unsigned int _length)
            {
                length=_length;
                generate();
            }
            void setWindow(unsigned int _window)
            {
                window=_window;
                generate();
            }
            void setHFreq(double _hfreq)
            {
                hfreq=(_hfreq>0.5)?0.5:_hfreq;
                generate();
            }
            void setLFreq(double _lfreq)
            {
                lfreq=(_lfreq>0.5)?0.5:_lfreq;
                generate();
            }
        protected:
            void generate()
            {
                int i=0,n;
                data.assign(length,0);
                coefficientes.resize(length);
                for(auto& el:coefficientes)
                {
                    n=i-length/2;
                    if (n==0)
                        el=(2*(hfreq-lfreq));
                    else
                        el=((sin(2*M_PI*hfreq*n)-sin(2*M_PI*lfreq*n))/(n*M_PI)*windowFunction(n));
                    i++;
                }
            }
        private:
            double lfreq;
            double hfreq;
    };
    class FIRHiPassFilter:public FIRBandPassFilter
    {
        public:
            FIRHiPassFilter(double _freq=0.0,unsigned int _length=FIR_FILTER_LENGTH,unsigned int _window=FIRWinFunction::WFT_NONE):FIRBandPassFilter(_freq,0.5f,_length,_window){}
            void setFreq(double _freq)
            {
                setBandFreq(_freq,0.5);
            }
    };
    class FIRLoPassFilter:public FIRBandPassFilter
    {
        public:
            FIRLoPassFilter(double _freq=0.5,unsigned int _length=FIR_FILTER_LENGTH,unsigned int _window=FIRWinFunction::WFT_NONE):FIRBandPassFilter(0.0f,_freq,_length,_window){}
            void setFreq(double _freq)
            {
                setBandFreq(0.0,_freq);
            }
    };
}
#endif // FILTER

