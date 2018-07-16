#ifndef DATALOADER_H
#define DATALOADER_H

#include <QByteArray>
#include <QVector>
///=============================Save Records====================================///
#define MAX_FRAME_ARRAY_POINT   (2)
#define MAX_CALIBR_POINTS_NUM   (128)
#define MAX_POLINOM_KOEF_COUNT  (MAX_CALIBR_POINTS_NUM+1)
#define MAX_CHAN_NUM            (3)
#define MAX_PAGE_NUM            (3)
typedef struct
{
        int    Type;
        int    Chan1;
        int    ChanN;
        int    ViewType;
        int    isAxis;
        int    CenterPos;
        int    CenterPosX;
        int    CenterPosY;
        int    MScale;
        int    VScale;
        int    HScale;
        int    GridType;
        int    Reserved[12];
}TSRPage;
typedef struct
{
        int    ChanNum1;
        int    ChanNum2;
        int    ChanNumRes;
        int    Type;
}TSRMix;
typedef struct
{
        int        Type;
        int        AverFilterValue;
        int        LoPassFilterValue;
        int        HiPassFilterValue;
        int        DifFilterValue;
        int        iirLoPassFilterValue;
        int        iirHiPassFilterValue;
        int        Reserved[8];
}TSRFilter;
typedef struct
{
        int         Type;
        int         Radius;
        int         BRadius;
        int         Width;
        int         Height;
        int         Alfa;
        int         Beta;
        int         isSimetric;
        int         Polarity;
        int         pX[MAX_FRAME_ARRAY_POINT];
        int         pY[MAX_FRAME_ARRAY_POINT];
        int         Direction;
        int         isAlarmed;
        int         PointsCount;
        int         Reserved[8];
}TSRASDFrame;
typedef struct
{
        TSRASDFrame     Frame;
}TSRASD;

typedef struct
{
        int             AmpI;
        float           AmpF;
        float           Phase;
        float           DefVal;
        int             X;
        int             Y;
        float           GapVal;
        int             Reserved;
}TSRPoint;

typedef struct
{
        int             PointsCount;
        int             Type;
        int             Units;
        float           BaseGain1;
        float           BaseGain2;
        int             BaseGenVoltage;
        TSRPoint        Points[MAX_CALIBR_POINTS_NUM];
        int             K;
        double          polinom_k[MAX_POLINOM_KOEF_COUNT];
        int             Reserved[32];
}TSRKalibrCurve;

typedef struct
{
        int             Freq;
        float           Gain1;
        float           Gain2;
        float           GainH;
        float           GainV;
        int             C_Num;
        int             GenVoltage;
        float           Rotation;
        TSRFilter       Filters;
        TSRASD          ASD;
        TSRKalibrCurve  KalibrCurve;
        int             isMix;
        int             Reserved[12];
}TSRChan;
typedef struct
{
        int             SweepWidth;
        int             SyncType;
        int             MeasFreq;
        int             PulsDPOnMm;
        int             MeasOnMm;
        int             AutoClearing;
        int             Brightness;
        int             Languich;
        int             TestMode;
        int             CurColorSheme;
        int             InputType;
        int             RVTPRoundsCount;
        int             Sound;
        int             DoubleFreqMode;
        int             MaxMeasFreq;
        int             RVTPType;
        int             isShowOnlyMix;
        int             RVTPAutoDetectMode;
        int             RVTPRPM;
        int             MScale;
        int             Sop;
        int             CorelScale;
        /*
        float           CondValue[2];
        float           GapValue;
        float           TempValue;
         */
        int             Reserved[5];
}TSRSystemSettings;

typedef struct{
        float  CondValue[2];
        float  GapValue;
        float  TempValue;
        float  ASDLevel[2];
        int    CondUnits;
        int    GapUnits;
        int    ASDPolarity;
}TSRSigmaSettings;
typedef struct{
    TSRPage             Page[MAX_PAGE_NUM];
    TSRChan             Chan[MAX_CHAN_NUM];
    TSRSystemSettings   SysSet;
    int                 TotalChan;
    int                 TotalSmp;
    int                 MixType;
    int                 MeasCursorType;
    TSRSigmaSettings    SigmaSet;
    int                 Reserved[1023-9];
}TSRDeviceSettings;


class DataLoader
{
public:
    DataLoader(){}
    virtual ~DataLoader(){}

    virtual bool load(const QString& aFilename);
    const QByteArray& getData() const { return mData; }
    int getDataSize() const { return mData.size(); }

protected:
    QByteArray mData;
};

class EddyconDataLoader : public DataLoader
{
    struct FrequencyData{
        std::vector<double> mXData;
        std::vector<double> mYData;
    };

public:
    EddyconDataLoader()
        : DataLoader()
    {}

    bool load(const QString& aFilename) override;
    int getSamlesCount() const {return mpDeviceSettings->TotalSmp; }
    int getFrequenciesCount() const{ return mpDeviceSettings->TotalChan; }
    const FrequencyData& getFrequencyData(int aFreqIndex) { return mFrequencies[aFreqIndex]; }

protected:
    TSRDeviceSettings* mpDeviceSettings;
    std::vector<FrequencyData> mFrequencies;
};

#endif // DATALOADER_H
