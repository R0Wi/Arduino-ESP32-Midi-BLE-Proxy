#ifndef ____SerialLogger__
#define ____SerialLogger__

class SerialLogger {
    public:
        void logDebug(String message) {
            #ifdef DEBUG
            Serial.println(message);
            #endif
        }
};
#endif