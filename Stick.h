#define JOYSTICK_SW_PIN 32

class Stick {
    unsigned long previousMillis;
    int Delay;
    int mainDelay;
    int readSW;
    boolean wasPressed;

    public:
        Stick() {
            mainDelay = 1000;
            Delay = mainDelay;
            previousMillis = 0;
            readSW = 1;
            pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
        }

        int isPressed() {
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= Delay) {
                Delay = mainDelay;
                previousMillis = currentMillis;
                readSW = digitalRead(JOYSTICK_SW_PIN);
                if (readSW == 0) {
                  readSW = 1;
                  Delay *= 3;
                  return 0;
                }
            }
            return readSW; // 0 means pressed
        }

};
