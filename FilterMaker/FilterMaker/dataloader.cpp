#include "dataloader.h"

#include <QFile>
#include <QDebug>

bool DataLoader::load(const QString& aFilename)
{
    QFile file(aFilename);
    file.open(QIODevice::ReadOnly);
    if(file.isOpen())
    {
        mData = file.readAll();
        qDebug()<< mData;
        return true;
    }
    else
    {
        qDebug()<<"DataLoader failed to load file!";
        return false;
    }
}


EddyconDataLoader::EddyconDataLoader(int aFrequenciesCount)
    : DataLoader()
    , mFrequenciesCount(aFrequenciesCount)
{
    mFrequencies.resize(aFrequenciesCount);
}

bool EddyconDataLoader::load(const QString &aFilename)
{
    if( DataLoader::load(aFilename) )
    {
        mSamplesCount = 2048;
        int index(0);
        for(FrequencyData& freqData : mFrequencies)
        {
            freqData.mXData.clear();
            freqData.mYData.clear();
        }

        for(int i = 0; i< mSamplesCount; ++i)
        {
            for(FrequencyData& freqData : mFrequencies)
            {
                freqData.mXData.push_back( mData[index++] );
                freqData.mYData.push_back( mData[index++] );
            }
        }
        index = 0;
        for(FrequencyData& freqData : mFrequencies)
        {
            qDebug()<<"X_DATA["<<index<<"]"<<freqData.mXData;
            qDebug()<<"Y_DATA["<<index++<<"]"<<freqData.mYData;
        }
        return true;
    }
    return false;
}


