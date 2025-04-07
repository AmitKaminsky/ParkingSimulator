#include <Arduino.h>

/*
    Connections:
    Connect the entire line to the same row on the ESP32 - row 5
    Black wire -> GND
*/

class LightController {
private:
    uint8_t pin; // The pin where the light (LED) is connected
    unsigned long onTime; // Duration to keep the light on (in milliseconds)
    unsigned long offTime; // Duration to keep the light off (in milliseconds)
    unsigned long lastToggleTime; // Last time the light state changed
    bool isLightOn; // Current state of the light
    bool isBlinking; // Flag to enable or disable blinking

public:
    // Constructor to initialize the pin and timings
    LightController(uint8_t pinNumber, unsigned long onDuration, unsigned long offDuration)
        : pin(pinNumber), onTime(onDuration), offTime(offDuration), lastToggleTime(0), isLightOn(false), isBlinking(true) {}

    // Setup the pin as an output
    void begin() {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, 0); // Start with the light off
    }

    // Blink the light based on the specified timings
    void blink() {
        if (!isBlinking) {
            digitalWrite(pin, 0); // Ensure the light is off if blinking is stopped
            isLightOn = false;
            return;
        }

        unsigned long currentTime = millis();
        if (isLightOn && (currentTime - lastToggleTime >= onTime)) {
            digitalWrite(pin, 0); // Turn off the light
            isLightOn = false;
            lastToggleTime = currentTime;
        } else if (!isLightOn && (currentTime - lastToggleTime >= offTime)) {
            digitalWrite(pin, 255); // Turn on the light
            isLightOn = true;
            lastToggleTime = currentTime;
        }
    }

    // Stop the blinking and turn off the light
    void stopBlinking() {
        isBlinking = false;

        digitalWrite(pin, 0); // Ensure the light is off
    }
};
