#ifndef DATALOADER_H
#define DATALOADER_H

#include <QByteArray>
#include <QVector>

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
        QVector<short int> mXData;
        QVector<short int> mYData;
    };

public:
    EddyconDataLoader(int aFrequenciesCount);

    bool load(const QString& aFilename) override;
    int getSamlesCount() const {return mSamplesCount; }
    int getFrequenciesCount() const{ return mFrequenciesCount; }

protected:
    int mFrequenciesCount;
    int mSamplesCount;
    QVector<FrequencyData> mFrequencies;
};

class EddyconCDataLoader : public EddyconDataLoader
{
public:
    EddyconCDataLoader()
        : EddyconDataLoader(3)
    {}

};

#endif // DATALOADER_H
