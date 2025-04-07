#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 300

class IRReceiver {
public:
    void startReceiving() {
        Serial.println(F("START IR Receiver"));
        // Start the receiver
        IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
        Serial.print(F("Ready to receive IR signals of protocols: "));
        printActiveIRProtocols(&Serial);
        Serial.printf(" at pin %d\n", IR_RECEIVE_PIN);
    }

    bool receive() {
        // Check if a signal has been received
        if (IrReceiver.decode()) {
//            IrReceiver.printIRResultShort(&Serial);
//            IrReceiver.printIRSendUsage(&Serial);            
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
//                Serial.println(F("Received noise or an unknown protocol"));
//                IrReceiver.printIRResultRawFormatted(&Serial, true);

                return false;
            }
            Serial.println();

            // Resume receiving for the next data
            IrReceiver.resume();

            // Check if the received command matches a specific value
            if (IrReceiver.decodedIRData.command == 0x1C) {
                interrupts(); // Re-enable interrupts
                return true; // Command matches
            }
        }
        interrupts(); // Re-enable interrupts
        return false; // No matching command received
    }
};
