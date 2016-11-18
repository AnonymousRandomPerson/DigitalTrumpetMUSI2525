#include "audioplayer.h"

AudioPlayer::AudioPlayer()
{
    _samplingRate = TrumpetGenerator::SAMPLE_RATE;
    double seconds = 2;

    QAudioFormat format;
    format.setSampleRate(_samplingRate);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
        qWarning() << "Default format unsupported.";
        format = info.nearestFormat(format);
    }

    _audio = new QAudioOutput(format, this);
    _audio->setBufferSize((int)(_samplingRate * seconds));

    _IDWrittenSample = 0;
    _sizeNolBuffer = 0;

    trumpet = new TrumpetGenerator();
    playbackIndex = 0;
}

AudioPlayer::~AudioPlayer()
{
    delete trumpet;
}

void AudioPlayer::start(double frequency, double seconds)
{
    if (frequency <= 0 || seconds <= 0)
    {
        return;
    }

    _audio->stop();

    int soundBlocks = 43;
    int releaseBlocks = 5;
    std::vector<double> totalSound (0);
    trumpet->resetCumsum();
    for (int i = 0; i < soundBlocks; i++)
    {
        std::vector<double> newSound = trumpet->generateTrumpet(frequency, i * TrumpetGenerator::BLOCK_SIZE, false);
        trumpet->insertVector(totalSound, newSound);
    }
    for (int i = 0; i < releaseBlocks; i++)
    {
        std::vector<double> newSound = trumpet->generateTrumpet(frequency, (i + soundBlocks) * TrumpetGenerator::BLOCK_SIZE, true);
        trumpet->insertVector(totalSound, newSound);
    }
    sound = totalSound;

    playbackIndex = 0;
    _pAudioIOBuffer = _audio->start();

    unsigned int periodSize = _audio->periodSize();
    _sizeNolBuffer = periodSize;
    _buffer = (signed char*) calloc(_sizeNolBuffer, sizeof(signed char));

    slot_writeMoreData();
}

void AudioPlayer::start(double frequency, std::array<bool, 3> valves, double seconds) {
    int lastIndex = valves.size() - 1;
    int bitwiseValve = 0;
    for (int i = 0; i <= lastIndex; i++)
    {
        if (valves[i])
        {
            bitwiseValve += 1 << i;
        }
    }
    start(trumpet->mapToFrequency(frequency, bitwiseValve), seconds);
}

void AudioPlayer::slot_writeMoreData()
{
    int nbBytes = _audio->bytesFree();

    if (nbBytes > 0)
    {

        if (_sizeNolBuffer < nbBytes)
        {
            delete[] _buffer;
            _sizeNolBuffer = nbBytes;
            _buffer = (signed char*) calloc(_sizeNolBuffer, sizeof(signed char));
        }

        short int value = 0;
        for (int IDSample = 0; IDSample < nbBytes; ++IDSample)
        {
            if (playbackIndex < sound.size())
            {
                value = sound[playbackIndex++];
            }
            _buffer[IDSample] = value;
        }

        int nbWrittenBytes = _pAudioIOBuffer->write((const char*) _buffer, nbBytes);
    }
}
