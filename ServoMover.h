#include <ESP32Servo.h>

int openingAngle = 5;
int closingAngle = 90;

class ServoMover {
    Servo myservo;

    int servoPin;
    int currentAngle = closingAngle;
    int Delay;
    unsigned long previousMillis;

  public:

    ServoMover(int pin) {
      servoPin = pin;
      previousMillis = 0;
      Delay = 50;
      
      ESP32PWM::allocateTimer(3); // TODO: parametrize that 0!
      delay(100);  // Add stabilization delay
      myservo.setPeriodHertz(50);
      delay(100);  // Add stabilization delay
      myservo.attach(servoPin, 500, 2500); 
    }


    boolean goUp() {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= Delay) {
            if (currentAngle > openingAngle) {
                currentAngle--;  // Decrement angle to go up
                myservo.write(currentAngle);
    //            Serial.println(currentAngle);
                previousMillis = millis();
                return true;
            }
            return false;  // Done moving up
    }
    return true;  // Wait for delay
}

  boolean goDown() {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= Delay) {
          if (currentAngle < closingAngle) {
              currentAngle++;  // Increment angle to go down
              myservo.write(currentAngle);
//              Serial.println(currentAngle);
              previousMillis = millis();
              return true;
          }
          return false;  // Done moving down
      }
      return true;  // Wait for delay
  }

  boolean isOpen(){
    return (currentAngle == openingAngle);
    }
  boolean isClose(){
    return (currentAngle == closingAngle);
    }
    
};
