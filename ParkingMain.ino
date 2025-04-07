#include "IRReceiver.h"
#include "UltrasonicSensor.h"
#include "LightController.h"
#include "Lights.h" 
#include <Adafruit_NeoPixel.h>
#include "Stick.h"
#include "ServoMover.h"
#include "MQTT_Helper.h"

/*
Project - Interactive Smart Parking Gate

We made a project that suimulates a smart parking gate system, as seen in our awesome video.

This project implements a smart gate control system, integrating ultrasonic car detection, servo-based gate movement,
visual lighting feedback via  LEDs, an IR remote trigger for fire alarms and manual fire control,
and an MQTT-based data exchange. 
By employing non-blocking timing strategies (using millis()) and breaking the system into multiple classes, as taught in class.

Features:
1. MQTT Integration (IoT):
   • Connects to Adafruit IO using WiFi.
   • Publishes and subscribes to feeds for real-time updates (current price, fire alert, open command, etc.).
   • Tracks parking metrics (guests, members, total cars, revenue).

2. Automatic Gate Operation:
   • An Ultrasonic Sensor detects approaching cars when leaving the garage, to open the gate and reduce number of total cars.
   • A Servo Motor raises or lowers the gate, demonstrating open/close functionality.

3. Visual Feedback:
   • A NeoPixel LED Strip provides dynamic lighting:
        • Standard mode (all white).
        • “Full Parking” mode (solid red).
        • “Enter/Exit” animations (half red, half white).
        • “Fire Alarm” blinking effect (quick red flashes).
   • A separate LightController class provides a blinking LED, indicating gate or alarm states.

4. User Interactions:
   • IR Remote triggers fire alarm functionality.
   • Joystick Button for manual gate control (simulating a driver requesting entry).

5. Sound Alarm:
   • A tone-based alarm is driven by PWM output, toggled during fire alerts.

Circuit Components:
1. Inputs:
   • Ultrasonic Sensor for car detection.
   • IR Receiver for remote-based fire alert.
   • Joystick (button press) for manual gate entry requests.
   • Adafruit control from dashboard.
2. Outputs:
   • NeoPixel LED Strip for parking status and gate animations.
   • Single LED for blinking (LightController).
   • Servo Motor to physically represent gate movement.
   • Tone-based buzzer for fire alarm sound.

Functional Workflow:
1. Startup:
   • Initializes WiFi, connects to the MQTT broker, and sets up sensors and actuators.
   • Resets or retrieves parking data from MQTT feeds.

2. Real-time Operation:
   • Uses loops with smart delays (millis()) to periodically:
       - Publish updated metrics to MQTT.
       - Read IR remote signals.
       - Check joystick input.
       - Measure the distance to approaching cars.
       - Adjust lights and servo position accordingly.

3. Fire Alert:
   • If a fire alert is detected (from either IR remote or MQTT feed):
       - Gate automatically opens.
       - Alarm sound (tone).
       - Red blinking NeoPixel animation.

4. Car Enter/Exit:
   • Ultrasonic sensor triggers exit if a car is detected near the gate.
   • Joystick button triggers an entry if there is room.
   • NeoPixel lighting patterns reflect gate opening/closing.

- Adafruit Dashboard:
	1.	Revenue
	•	Displays the total accumulated parking revenue based on the current price and the
   number of arrivals (guests/members). This value increases whenever a new guest or member enters.

	2.	Status
	•	Shows the overall system status, such as “READY” when the system is connected and functional

	3.	Visitors Chart
	•	Plots the count of guests and members over time. Each data point is recorded whenever
   your code updates the “guests” or “members” feeds, providing a history of how many visitors arrived in each category.

	4.	Fire Alert
	•	A button (or feed) that, when toggled, indicates a fire emergency.

	5.	Gate
	•	A button (or feed) for manually sending an “OPEN” command to the gate if you want to simulate
   a remote/manual override. When pressed, the system logic should raise the servo and allow a car to enter/exit.

	6.	Number of Cars (Gauge)
	•	A circular gauge that displays the current count of cars in the parking lot.
   It visually indicates how full the lot is based on a set maximum,
    at 5 it's colorde red to show it's full (as well as the LEDS).

	7.	Current Price
	•	A slider that allows you to set or view the parking fee (in your local currency).
   This feed is read to determine how much to charge guests or members, which then affects the revenue feed.

These feeds are subscribed to or published by the ESP32, which uses the data to guide your parking system’s logic—for example,
 deciding whether a new car can enter based on the “Number of Cars” feed, controling lights and sounds as well.

Video link: https://youtube.com/shorts/xx8vsdRDkPM?si=snKenACvGb3_9-I0
Adafruit IO link: https://io.adafruit.com/hangs_treads_4d/dashboards/parking-project-iot

Created By:
Theodore_Dai_Maman #211541594  
Omer_Dan #322952466  
Amit_Kaminsky #207487661
*/

// Thresholds
#define MAX_OPEN_DISTANCE 7
#define MIN_OPEN_DISTANCE 3
#define MAX_MEASURED_DISTANCE 30
#define MAX_CARS 5

// Lightstrip configuration
#define LED_PIN 2
#define LED_COUNT 24
boolean lightsOn = false;
int currentLight = 0;
Lights lights;
Adafruit_NeoPixel lightStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Sound
#define TONE_OUTPUT_PIN 26
#define NOTE_C4  262
const int TONE_PWM_CHANNEL = 0;
boolean isBeeping = false;
int noteDuration = 250;
unsigned long previousMillis_sound;

// Joystick switch pin
#define JOYSTICK_SW_PIN 32
Stick stick;

// Single Light configuration
#define LightPin 5
LightController lightController(LightPin, 250, 250);

// Ultrasonic ultraSonicSensor Configuration
#define TRIGGER_PIN 16
#define ECHO_PIN 17
UltrasonicSensor ultraSonicSensor(TRIGGER_PIN, ECHO_PIN, MAX_MEASURED_DISTANCE);

// Servo Configuration
#define SERVO_PIN 19
ServoMover servo_mover(SERVO_PIN);

// IRReceiver Configuration
IRReceiver irReceiver;


// Variables
int numCars = 0;
int fireAlarm = 0;
int openG = 0;
boolean notFired = true;
boolean notOpened = true;

// Delay
unsigned long previousMillis = 0;
int Delay = 250;
int openGateDelay = 3000;

void setup() {
  Serial.begin(115200);

   // Start the IR receiver
  irReceiver.startReceiving();

  // Initialize the ultrasonic ultraSonicSensor
  ultraSonicSensor.begin();

  // Initialize light lightStrip
  lightStrip.begin();
  
  lights.standard(lightStrip);
  lightStrip.show();
  lightStrip.setBrightness(50);


  //Sound
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
  
  Serial.println("MQTT Tester Starting...");
  mqtt_init();
  numCars = mqtt_getTotalCars();
}

void openGate(int onExit){
  Serial.println("Opening the gate");
  
  while(servo_mover.goUp()) {
    lightController.blink();

    if (onExit == 1){ // Car exit
      lights.playExitLights(lightStrip);
    }
    else if (onExit == 0) { // Car enter
      lights.playEnterLights(lightStrip);
      
    } else if (onExit == 2) { // Fire
      
    }
  }
}

void closeGate(){
  Serial.println("Closing the gate");

  while(servo_mover.goDown()) {
    lightController.blink();
  }
}

void playAlarm(){
  unsigned long currentMillis = millis();
      if (isBeeping) {
        if (currentMillis - previousMillis_sound >= noteDuration) {
          ledcWrite(TONE_PWM_CHANNEL, 0);  // Stop the tone
          isBeeping = false;
          previousMillis_sound = currentMillis;
        }
  } else {
      if (currentMillis - previousMillis_sound >= noteDuration) {
        ledcWriteTone(TONE_PWM_CHANNEL, NOTE_C4);  // Start the tone
        isBeeping = true;
        previousMillis_sound = currentMillis;
      }
  }
}

void loop() {
    int distance = ultraSonicSensor.measureDistance();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= Delay) {
      previousMillis = currentMillis;
      // Ada fruit comms
      mqtt_pushData(); // Inner delay
  
      
      if (servo_mover.isOpen() && fireAlarm != 1) {
        // Wait some time for car to pass
        delay(openGateDelay);
        
        // Close the gate
        closeGate();
        lights.standard(lightStrip);

      }

      openG = mqtt_getOpen();
      // Check if there is FIRE ALARM
      fireAlarm = mqtt_getFireAlert();

      if (distance < MAX_OPEN_DISTANCE && distance > MIN_OPEN_DISTANCE) {
        // Car is near the exit gate - opening the gate
        Serial.print("Open distance:");
        Serial.println(distance);
        openGate(1);
        numCars--;
        mqtt_setTotalCars(numCars);
        
      } else if (!stick.isPressed()) {
          // Opening the gate - car wants to enter
          if (numCars < MAX_CARS) {
            Serial.println("A guest has arrived");
            openGate(0);
            numCars++;
            mqtt_setTotalCars(numCars);
            mqtt_incGuests();
          
          } else {
              Serial.println("Parking lot is full!");
              lights.full(lightStrip);   
          }

        // Member has arrived
        } else if (openG == 1 && notOpened) {
             delay(200);
             Serial.println("A member has arrived");
             openGate(false);
             numCars++;
             mqtt_setTotalCars(numCars);
             mqtt_incMembers();
             notOpened = false;

        // Fire alarm using the IR remote
        } else if (irReceiver.receive() || fireAlarm == 1) {
              if (fireAlarm == 1) {
                // true for blinking, false for not blinking
                lights.blink(lightStrip);
              
                playAlarm();
                openGate(2);
                notFired = false;
            
            } else {
              ledcWrite(TONE_PWM_CHANNEL, 0);
            }
        }
            
      // Parking is full - turn on the red light
      if (numCars == MAX_CARS) {
        lights.full(lightStrip);
      }
    }
    

}
