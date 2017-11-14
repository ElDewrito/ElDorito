/*
ratelimiter by mfycheng
https://github.com/mfycheng/ratelimiter */

#ifndef _rate_limiter_h_
#define _rate_limiter_h_

#include <mutex>
#include "rate_limiter_interface.hpp"

class RateLimiter : public RateLimiterInterface {
public:
    RateLimiter();
    long aquire();
    long aquire(int permits);

    bool try_aquire(int timeouts);
    bool try_aquire(int permits, int timeout);

    double get_rate() const;
    void set_rate(double rate);
private:
    void sync(unsigned long long now);
    std::chrono::microseconds claim_next(double permits);
private:
    double interval_;
    double max_permits_;
    double stored_permits_;

    unsigned long long next_free_;

    std::mutex mut_;
};


#endif