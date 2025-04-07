#include <Adafruit_NeoPixel.h>

/*
    Connections:
    All 3 pins are connected to the same row on the ESP32 - row 2
    GND -> GND
    5V -> middle pin on the ESP32
    DIN -> Blue pin on the ESP32
*/

#define LED_PIN 2
#define LED_COUNT 24
#define NUM_BLINKS 4

class Lights {

    // NeoPixel LED configuration
    int currentLight = 0;

    int red = 255;
    int green = 0;
    int blue = 0;
    int white = 255;

    // temp
    float prev_temp = -273.15;

    // Delay
    unsigned long currentMillis = 0;
    unsigned long lastTime = 0;
    unsigned long timerDelay = 25;

    public:

    Lights() {
    }

    void fillStrip(Adafruit_NeoPixel &strip, int red, int green, int blue) {
        for (int i = 0; i < LED_COUNT; i++) {
            strip.setPixelColor(i, strip.Color(red, green, blue));

        }
//        Serial.printf("COLOR FILL: %d, %d, %d", red, green, blue);

        strip.show();
    }

    void blink(Adafruit_NeoPixel &strip){
      red = 255;
      green = 0;
      blue = 0;

      for (int i = 0; i < NUM_BLINKS; i++)
      {
        fillStrip(strip, red, green, blue);

        delay(25);
        fillStrip(strip, 0, 0, 0);
        delay(25);
      }
    }

    void standard(Adafruit_NeoPixel &strip)
    {
      for (int i = 0; i < LED_COUNT; i++) {
        if (i < LED_COUNT) {
            // White
            strip.setPixelColor(i, strip.Color(255, 255, 255));
        }
       
        strip.show();
//        Serial.println("Colored red");
      }
    }

    void playExitLights(Adafruit_NeoPixel &strip)
    {
        for (int i = 0; i < LED_COUNT; i++) {
            if (i < LED_COUNT / 2) {
                // Red
                strip.setPixelColor(i, strip.Color(red, 0, 0));
            }
            else {
                strip.setPixelColor(i, strip.Color(white, white, white));
            }
        }

        strip.show();
    }

    void playEnterLights(Adafruit_NeoPixel &strip)
    {
        for (int i = 0; i < LED_COUNT; i++) {
            if (i < LED_COUNT / 2) {
                // White
                strip.setPixelColor(i, strip.Color(white, white, white));
            }
            else {
              strip.setPixelColor(i, strip.Color(red, 0, 0));
            }
        }
        
        strip.show();
    }

    void full(Adafruit_NeoPixel &strip) {
        for (int i = 0; i < LED_COUNT; i++) {
            strip.setPixelColor(i, strip.Color(255, 0, 0));

        }
        strip.show();
    }
};
