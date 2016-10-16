#include "audioplayer.h"

#include "TrumpetGenerator.h"

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

    TrumpetGenerator *trumpet = new TrumpetGenerator();
    sound = trumpet->generateTrumpet(440, seconds);
    playbackIndex = 0;
    delete trumpet;
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::start()
{
    _audio->stop();
    //QObject::connect(_audio, SIGNAL(notify()), this, SLOT(slot_writeMoreData()));

    playbackIndex = 0;
    _pAudioIOBuffer = _audio->start();

    unsigned int periodSize = _audio->periodSize();
    _sizeNolBuffer = periodSize;
    _buffer = (signed char*) calloc(_sizeNolBuffer, sizeof(signed char));

    slot_writeMoreData();
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
