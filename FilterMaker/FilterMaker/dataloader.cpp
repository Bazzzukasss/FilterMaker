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

bool EddyconDataLoader::load(const QString &aFilename)
{
    if( DataLoader::load(aFilename) )
    {
        char* pSettings = mData.data();
        short int* pData = (short int*)(pSettings + sizeof(TSRDeviceSettings));
        mpDeviceSettings = reinterpret_cast<TSRDeviceSettings*>(pSettings);

        mFrequencies.resize(mpDeviceSettings->TotalChan);
        for(FrequencyData& freqData : mFrequencies)
        {
            freqData.mXData.clear();
            freqData.mYData.clear();
        }

        for(int i = 0; i< mpDeviceSettings->TotalSmp; ++i)
        {
            for(FrequencyData& freqData : mFrequencies)
            {
                freqData.mXData.push_back( *pData++ );
                freqData.mYData.push_back( *pData++ );
            }
            pData+=2;

        }
        mIsDataLoaded = true;
        return true;
    }
    return false;
}


