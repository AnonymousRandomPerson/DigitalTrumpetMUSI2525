#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QIODevice>
#include <QAudioDeviceInfo>
#include <QDebug>

#include <math.h>
#include <vector>
#include <array>

#include "TrumpetGenerator.h"

#define PI 3.14159265

using namespace std;

class AudioPlayer : public QObject
{
    Q_OBJECT

    public:
        AudioPlayer();
        ~AudioPlayer();
        void start(double frequency, double seconds);
        void start(double frequency, std::array<bool, 3> valves, double seconds);

    public slots:
        void slot_writeMoreData();

    private:
        QAudioOutput* _audio;
        QIODevice* _pAudioIOBuffer;

        float _samplingRate;

        unsigned long _IDWrittenSample;
        signed char* _buffer;
        int _sizeNolBuffer;

        std::vector<double> sound;
        int playbackIndex;

        TrumpetGenerator *trumpet;
};

#endif
