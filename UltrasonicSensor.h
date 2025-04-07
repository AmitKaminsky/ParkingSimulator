#include <Arduino.h>
/////////////////////////
// Ultrasonic Sensor //
/*
* Connections:
*    VCC -> 5V
*    GND -> GND
*    TRIG -> 16
*    ECHO -> 17
*/
/////////////////////////

class UltrasonicSensor {
private:
    int trigPin;       // Pin for the trigger signal
    int echoPin;       // Pin for the echo signal
    int maxDistance;   // Maximum measurable distance in cm

public:
    // Constructor to initialize the pins and max distance
    UltrasonicSensor(int triggerPin, int echoPin, int maxDist)
        : trigPin(triggerPin), echoPin(echoPin), maxDistance(maxDist) {}

    // Initialize the sensor pins
    void begin() {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    // Measure distance in cm
    int measureDistance() {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(trigPin, LOW);

        int duration = pulseIn(echoPin, HIGH, maxDistance * 59); // Timeout based on maxDistance
        return duration / 59; // Convert to cm
    }
};