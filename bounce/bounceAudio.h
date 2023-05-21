#include <QThread>

class AudioThread : public QThread
{
    Q_OBJECT
    class RtAudio * m_adc;
    class InputData * m_data;
    volatile bool m_running;
    void run();
public :
    AudioThread();
    ~AudioThread();
public slots :
    void startAudio();
    void stopAudio();
    void outputAudioSlot(double start, double end);
signals :
    void outputAudio(double start, double end);
};

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

typedef signed short sound_t;

class QueueData
{
public :
    sound_t * m_buffer;
    unsigned int m_frames;
};

typedef double QueueResult;

#include "filter.h"

const unsigned int filterM = 40;
const double filterCutoff = 2000.0;

class InputData : public QObject
{
    Q_OBJECT
private :
    double filterSampleRate;
    double * filter;
    unsigned int filterSize;
    void initializeFilter(double rate)
    {
        delete [] filter;

        filterSampleRate = rate;
        if (filterCutoff / rate <= 0.5)
        {
            filterSize = filterM + 1;
            filter = new double [filterSize];
            computeLowpassFilter(filterM, filterCutoff / rate, filter);
        }
        else {
            filterSize = 1;
            filter = new double [filterSize];
            filter[0] = 1.0;
        }
    }

public :
    QMutex m_mutex_data;
    QQueue<QueueData> m_queue_data;
    QMutex m_mutex_filtered;
    QQueue<QueueData> m_queue_filtered;
    QMutex m_mutex_preresult;
    QQueue<QueueResult> m_queue_preresult;
    QMutex m_mutex_result;
    QQueue<QueueResult> m_queue_result;
    AudioThread * m_AudioThread;
    unsigned int channels;
    unsigned int sampleRate;

    void filterTick();
    void tick();
    void smoothTick();
    void tock();

    InputData() : filterSampleRate(0.0), filter(NULL), filterSize(0)
    {
    }
    ~InputData()
    {
        clear();
        delete [] filter;
    }
    void clear()
    {
        {
            QMutexLocker lock(&m_mutex_filtered);
            while (!m_queue_filtered.isEmpty())
                delete [] m_queue_filtered.dequeue().m_buffer;
        }
        {
            QMutexLocker lock(&m_mutex_data);
            while(!m_queue_data.isEmpty())
                delete [] m_queue_data.dequeue().m_buffer;
        }
        {
            QMutexLocker lock(&m_mutex_preresult);
            m_queue_preresult.clear();
        }
        {
            QMutexLocker lock(&m_mutex_result);
            m_queue_result.clear();
        }
    }
signals :
    void outputData(double,double);
};

