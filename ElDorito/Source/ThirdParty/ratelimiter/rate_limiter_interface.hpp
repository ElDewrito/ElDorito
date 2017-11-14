/*
  ratelimiter by mfycheng
  https://github.com/mfycheng/ratelimiter */

#ifndef _rate_limiter_interface_h_
#define _rate_limiter_interface_h_

class RateLimiterInterface {
public:
    virtual ~RateLimiterInterface() {}

    virtual long aquire() = 0;
    virtual long aquire(int permits) = 0;

    virtual bool try_aquire(int timeout) = 0;
    virtual bool try_aquire(int permits, int timeout) = 0;

    virtual double get_rate() const = 0;
    virtual void set_rate(double rate) = 0;
};

#endif