class IClock {
public:
    virtual unsigned long milliseconds() = 0;
    virtual ~IClock() = default;
};
