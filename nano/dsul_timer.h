// DSUL - Disturb State USB Light : DsulTimer class

class DsulTimer {
public:
  uint32_t Interval;
  uint32_t lastUpdate;

  void (*OnComplete)();

  DsulTimer(uint32_t setTime, void (*callback)()) {
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
  void reset() { lastUpdate = millis(); }
};
