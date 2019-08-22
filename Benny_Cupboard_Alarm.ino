/*
   Adapted from Low Power SLEEP modes for Arduino UNO/Nano
   using Atmel328P microcontroller chip video.

   For full details see my video #115
   at https://www.youtube.com/ralphbacon

   This video is #159 to be found at the same place!

   All details can be found at https://github.com/ralphbacon

*/
#include "Arduino.h"
#include <avr/sleep.h>

#define wakePin 3   // when rising, makes 328P wake up, must be an interrupt pin (2 or 3 on ATMEGA328P)
#define ledPin 5    // output pin for the LED (to show it is awake)
#define ldrPIN A0   // Determines light/dark state and prevent PIR from waking up unless dark
#define pwrLDR 18   // A4 used as digital

// Array of pins in use; all others get set to INPUT to reduce power
// char pinsInUse[] = {8, 3, 5, 14, 18};

unsigned long awakeTimeMs = 0;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // Set all [unused?] pin to INPUT mode to reduce power
  for (auto cnt = 2; cnt < 20; cnt++) {
    pinMode(cnt, INPUT);
  }

  // Wake pin will be brought HIGH/LOW by PIR
  pinMode(wakePin, INPUT);

  // Flashing LED / BEEP just to show the µController is running
  digitalWrite(ledPin, LOW);
  pinMode(ledPin, OUTPUT);
  doBlink();

  // Power to potential divider with LDR. Ensures no power used until we read it.
  pinMode(pwrLDR, OUTPUT);
  digitalWrite(pwrLDR, LOW);

  Serial.println("Setup completed.");
}

// The loop just blinks an LED when not in sleep mode
void loop() {
  static int previousDark = 0;

  // Just blink LED twice to show we're running
  doBlink();

  // How dark is it?
  digitalWrite(pwrLDR, HIGH);
  delay(1);

  auto darkness = analogRead(ldrPIN);
  if (darkness != previousDark) {
    previousDark = darkness;
    Serial.println(darkness);
  }

  // Is it dark and we've been awake for 5 seconds?
  if (darkness > 400
      || millis() > awakeTimeMs + 5000)
  {
    // Turn off the LDR power
    digitalWrite(pwrLDR, LOW);

    // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;

    /* Set the type of sleep mode we want. Can be one of (in order of power saving):
        SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
        SLEEP_MODE_ADC
        SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
        SLEEP_MODE_EXT_STANDBY
        SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
        SLEEP_MODE_PWR_DOWN (Deep sleep)
    */
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Turn off Brown Out Detection (low voltage)
    // Thanks to Nick Gammon for how to do this (temporarily) in software rather than
    // permanently using an avrdude command line.
    //
    // Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
    //
    // BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
    // the MCU Control Register (MCUCR)
    MCUCR = bit (BODS) | bit (BODSE);

    // The BODS bit is automatically cleared after three clock cycles so we better get on with it
    MCUCR = bit (BODS);

    // Ensure we can wake up again by first disabling interupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(wakePin), sleepISR, RISING);

    // Send a message just to show we are about to sleep
    Serial.println("Good night!");
    Serial.flush();

    // Allow interrupts now
    interrupts();

    // And enter sleep mode as set above
    sleep_cpu();

    // --------------------------------------------------------
    // µController is now asleep until woken up by an interrupt
    // --------------------------------------------------------

    // Wakes up at this point when wakePin is RISING - interrupt routine is run first
    Serial.println("I'm awake!");

    // Reset the awake Counter so we stay awake for a minimum amount of time (unless it is light)
    awakeTimeMs = millis();

    // Reset beeper timer so we beep almost immediately on wakeup (if movement and dark)
    lastMillis = millis() + 500;

    // Re-enable ADC if it was previously running
    ADCSRA = prevADCSRA;
  }
}


// When wakePin is brought LOW this interrupt is triggered FIRST (even in PWR_DOWN sleep)
void sleepISR() {
  // Prevent sleep mode, so we don't enter it again, except deliberately, by code
  sleep_disable();

  // Detach the interrupt that brought us out of sleep
  detachInterrupt(digitalPinToInterrupt(wakePin));

  // Now we continue running the main Loop() just after we went to sleep
}


// Double blink and beep just to show we are running. Note that we do NOT
// use the delay for final delay here, this is done by checking
// millis instead (non-blocking)
void doBlink() {
  if (millis() > lastMillis + 1000) {
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    lastMillis = millis();
  }
}
