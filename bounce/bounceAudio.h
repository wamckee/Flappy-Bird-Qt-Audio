/*
MIT License
Copyright (c) 2023 wamckee
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

