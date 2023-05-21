#include "bounceAudio.h"

#include "RtAudio.h"

inline signed short Round16(double d)
 { return (d<0.0) ? (signed short)(d-0.5) : (signed short)(d+0.5); }

int RtAudio_input(void * /* outputBuffer */, void * inputBuffer, unsigned int nBufferFrames, double /* streamTime */, RtAudioStreamStatus /* status */, void * userData)
{
    InputData * data = (InputData *)userData;
    unsigned int channels = data->channels;

    Q_ASSERT(channels > 0);

    QueueData queue_data;
    queue_data.m_frames = nBufferFrames;
    queue_data.m_buffer = new sound_t [queue_data.m_frames];

    sound_t * buffer = (sound_t *) inputBuffer;
    for (unsigned int f = 0; f < queue_data.m_frames; ++f)
    {
        double t = 0.0;
        for (unsigned int c = 0; c < channels; ++c)
        {
            t += buffer[f * channels + c];
        }
        queue_data.m_buffer[f] = Round16(t / channels);
    }

    QMutexLocker lock(&data->m_mutex_data);
    data->m_queue_data.enqueue(queue_data);

    return 0;
}

AudioThread::AudioThread() : m_data(new InputData), m_running(false)
{
    connect(m_data, SIGNAL(outputData(double,double)), this, SLOT(outputAudioSlot(double,double)));

    unsigned int bufferFrames = 1024;

    m_adc = new RtAudio;
    std::vector<unsigned int> deviceIds = m_adc->getDeviceIds();
    if (deviceIds.size() < 1)
    {
    }
    else
    {
        RtAudio::StreamParameters iParams;
        iParams.deviceId = m_adc->getDefaultInputDevice();
        RtAudio::DeviceInfo info = m_adc->getDeviceInfo(iParams.deviceId);
        m_data->sampleRate = info.preferredSampleRate;
        m_data->channels = info.inputChannels;
        iParams.nChannels = m_data->channels;
        iParams.firstChannel = 0;

        if (m_adc->openStream(NULL, &iParams, RTAUDIO_SINT16, m_data->sampleRate, &bufferFrames, &RtAudio_input, (void *)m_data))
        {
        }
        else
        if (m_adc->isStreamOpen() == false)
        {
        }
    }
}

AudioThread::~AudioThread()
{
    stopAudio();

    delete m_adc;

    delete m_data;
}

void AudioThread::startAudio()
{
    start();

    if (m_adc->startStream())
    {
    }
}

void AudioThread::stopAudio()
{
    m_adc->stopStream();

    m_running = false; while(isRunning()) { yieldCurrentThread(); }

    m_data->clear();
}

void AudioThread::run()
{
    m_running = true;
    while(m_running)
    {
        m_data->filterTick();
        m_data->tick();
        m_data->smoothTick();
        m_data->tock();
        msleep(1);
    }
}

void AudioThread::outputAudioSlot(double start, double end)
{
    emit outputAudio(start, end);
}

inline double clip(double v) { if (v > 1.0) return 1.0; if (v < -1.0) return -1.0; return v; }

void InputData::filterTick()
{
    while (true)
    {
        QueueData queue_data, queue_head;
        {
            QMutexLocker lock(&m_mutex_data);
            if (m_queue_data.size() < 2) return;
            queue_data = m_queue_data.dequeue();
            queue_head = m_queue_data.front();
            Q_ASSERT(queue_data.m_frames == queue_head.m_frames);
        }

        unsigned int frames = queue_data.m_frames + queue_head.m_frames;
        sound_t * buffer = new sound_t [frames];
        for (unsigned int i = 0; i < queue_data.m_frames; ++i)
            buffer[i] = queue_data.m_buffer[i];
        for (unsigned int i = 0; i < queue_head.m_frames; ++i)
            buffer[queue_data.m_frames + i] = queue_head.m_buffer[i];

        delete [] queue_data.m_buffer;

        if (sampleRate != filterSampleRate)
        {
            initializeFilter(sampleRate);
        }

        unsigned outputSize = queue_data.m_frames / 4;
        sound_t * output = new sound_t [outputSize];
        for (unsigned int k = 0; k < outputSize; ++k)
        {
            double sum = 0.0;
            for (unsigned int m = 0; m < filterSize; ++m)
            {
                unsigned int idx = k * 4 + m;
                double v = buffer[idx] / 32767.0;
                sum += filter[m] * v;
            }
            output[k] = Round16(clip(sum) * 32767.0);
        }

        delete [] buffer;

        QueueData queue_filtered;
        queue_filtered.m_buffer = output;
        queue_filtered.m_frames = outputSize;

        {
            QMutexLocker lock(&m_mutex_filtered);
            m_queue_filtered.enqueue(queue_filtered);
        }
    }

}

extern "C" {
#include "Yin.h"
};

#include <cmath>

const double log_2 = log(2.0);

void InputData::tick()
{
    while (true)
    {
        QueueData queue_data, queue_head;
        {
            QMutexLocker lock(&m_mutex_filtered);
            if (m_queue_filtered.size() < 2) return;
            queue_data = m_queue_filtered.dequeue();
            queue_head = m_queue_filtered.front();
            Q_ASSERT(queue_data.m_frames == queue_head.m_frames);
        }

        unsigned int frames = queue_data.m_frames + queue_head.m_frames;
        sound_t * buffer = new sound_t [frames];
        for (unsigned int i = 0; i < queue_data.m_frames; ++i)
            buffer[i] = queue_data.m_buffer[i];
        for (unsigned int i = 0; i < queue_head.m_frames; ++i)
            buffer[queue_data.m_frames + i] = queue_head.m_buffer[i];

        delete [] queue_data.m_buffer;

        unsigned int chunksize = queue_data.m_frames / 5;
        for (unsigned int i = 0; i < 5; ++i)
        {
            unsigned int offset = i * chunksize;
            sound_t * chunk = buffer + offset;

            QueueResult queue_result;

            {
                Yin yin;
                Yin_init(&yin, queue_data.m_frames, 0.20f);
                queue_result = 
                    Yin_getPitch(&yin, chunk, (int)sampleRate / 4);
                Yin_free(&yin);

                if (queue_result != -1.0)
                    queue_result = log(queue_result) / log_2;
                if (queue_result > log(1024.0)/log_2) queue_result = -1.0;
            }

            {
                QMutexLocker lock(&m_mutex_preresult);
                m_queue_preresult.enqueue(queue_result);
            }
        }
        delete [] buffer;
    }
}

void InputData::smoothTick()
{
    while (true)
    {
        const int n = 7;
        const int mid = (n-1)/2;
        QueueResult queue_data[n];
        {
            QMutexLocker lock(&m_mutex_preresult);
            if (m_queue_preresult.size() < n) return;
            for (int i = 0; i < n; ++i)
                queue_data[i] = m_queue_preresult[i];
            m_queue_preresult.dequeue();
        }

        {
            QueueResult queue_result;

            {
                queue_result = queue_data[mid];

                int i = mid;
                while (i >= 0 && queue_data[i] == -1.0) --i;
                int j = mid;
                while (j < n && queue_data[j] == -1.0) ++j;

                if (i != j && i >= 0 && j < n)
                {
                    QueueResult a = queue_data[i];
                    QueueResult b = queue_data[j];
                    double d = b - a;
                    int D = d < 0.0 ? (int)(d - 0.5) : (int)(d + 0.5);
                    b -= D;
                    queue_result = a + (mid - i) * (b - a) / (j - i);
                }
            }

            {
                QMutexLocker lock(&m_mutex_result);
                m_queue_result.enqueue(queue_result);
            }
        }
    }
}

void InputData::tock()
{
    while (true)
    {
        QueueResult queue_result, queue_head;
        {
            QMutexLocker lock(&m_mutex_result);
            if (m_queue_result.size() < 2) return;
            queue_result = m_queue_result.dequeue();
            queue_head = m_queue_result.front();
        }

        bool good = true;
        if (queue_result == -1.0) good = false;
        if (queue_head == -1.0) good = false;
        // if (good && fabs(queue_result - queue_head) > 0.2) good = false;

        if (good)
            emit outputData(queue_result, queue_head);
        else
            emit outputData(-1.0, -1.0);
    }
}
