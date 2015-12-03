#ifndef __LIBSOCKETIO_REF_H
#define __LIBSOCKETIO_REF_H
#ifdef __linux__
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#endif

inline long getTimeInMs() {
    struct timeval t;

    gettimeofday(&t,NULL);

    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

class Ref {
public:
    Ref():_count(1){
        _timeSpan = -1;
        _lastTime = getTimeInMs();
    }
    virtual ~Ref(){}

    int retain() {
        ++_count;
        return _count;
    }

    void release() {
        --_count;

        if(_count <= 0)
            delete this;
    }

    virtual void update(long ft) {}

    inline void setScheduleTime(int ms) {
        _timeSpan = ms;
    }

    inline void schedule() {
        long nowTime = getTimeInMs();
        if(_timeSpan == 0) {
            update(nowTime - _lastTime);
            _lastTime = nowTime;
        } else if(_timeSpan > 0 && _lastTime + _timeSpan > nowTime) {
            update(nowTime - _lastTime);
            _lastTime = nowTime;
        } else
            _lastTime = nowTime;
    }

protected:
    volatile int _count;
    int _timeSpan;
    long _lastTime;
};
#endif
