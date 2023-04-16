#ifndef I_CLOCK_H
#define I_CLOCK_H

class IClock {
public:
    virtual ~IClock() = default;
    virtual unsigned long milliseconds() = 0;
};

#endif
