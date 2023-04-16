#ifndef I_CLOCK_H
#define I_CLOCK_H

class IClock {
public:
    virtual unsigned long milliseconds() = 0;
    virtual ~IClock() = default;
};

#endif
