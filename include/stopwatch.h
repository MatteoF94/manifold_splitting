//
// Created by matteo on 28/07/18.
//

#ifndef MANIFOLD_SPLITTING_STOPWATCH_H
#define MANIFOLD_SPLITTING_STOPWATCH_H

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

class Stopwatch {
public:
    Stopwatch () {
        mTimeStart = 0.;
    }

    void start() {
        mTimeStart = timestamp();
    }

    double stop() {
        return timestamp() - mTimeStart;
    }

private:
    inline double timestamp() {
        struct timeval tp{};
        gettimeofday(&tp,NULL);

        return double(tp.tv_sec) + tp.tv_usec / 1000000.;
    }

    double mTimeStart;

};

#endif //MANIFOLD_SPLITTING_STOPWATCH_H
