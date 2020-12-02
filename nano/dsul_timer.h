// // DSUL - Disturb State USB Light : DsulTimer class

class DsulTimer {
    public:

    unsigned long Interval;
    unsigned long lastUpdate;

    void (*OnComplete)();

    DsulTimer(unsigned long setTime, void (*callback)()) {
        Interval = setTime;
        OnComplete = callback;
    }

    // Update the timer and check if we should fire callback
    void update() {
        if ((millis() - lastUpdate) > Interval) {
            lastUpdate = millis();
            OnComplete();
        }
    }

    // Reset timer
    void reset() {
        lastUpdate = millis();
    }
};
