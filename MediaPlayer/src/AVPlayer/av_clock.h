#ifndef AVCLOCK_H
#define AVCLOCK_H

extern "C"
{
#include <libavutil/time.h>
}

class AVClock
{
public:
    AVClock()
        : m_pts(0.0),
          m_drift(0.0)
    {

    }

    inline void reset()
    {
        m_pts=0.0;
        m_drift=0.0;
    }

    inline void setClock(double pts) {
        setCloctAt(pts);
    }

    inline double getClock() {
        return m_drift+av_gettime_relative()/1000000.0;
    }

private:
    inline void setCloctAt(double pts) {
        m_drift=pts-av_gettime_relative()/1000000.0;
        m_pts=pts;
    }

    double m_pts;
    double m_drift;
};

#endif // AVCLOCK_H
