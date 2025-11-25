#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ===== LED STRIP SETUP (COPY FROM WORKING CODE) =====
#define LED_PIN 5
#define NUM_LEDS 150
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_BGR + NEO_KHZ400);

void setup() {
  // Initialize LED strip FIRST (like working code)
  strip.begin();
  strip.show();
  strip.setBrightness(255);

  // Initialize serial
  Serial.begin(115200);
  Serial.println("ESP32 LED Test - Copy from working code");
  
  // Test sequence
  Serial.println("Testing RED...");
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  strip.show();
  delay(2000);
  
  Serial.println("Testing GREEN...");
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  strip.show();
  delay(2000);
  
  Serial.println("Testing BLUE...");
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }
  strip.show();
  delay(2000);
  
  Serial.println("Testing OFF...");
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  
  Serial.println("Setup complete - starting loop test");
}

void loop() {
  static unsigned long lastChange = 0;
  static int phase = 0;
  
  if (millis() - lastChange >= 3000) {
    lastChange = millis();
    
    switch(phase % 4) {
      case 0:
        Serial.println("LOOP: ALL RED");
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.setPixelColor(i, strip.Color(255, 0, 0));
        }
        break;
      case 1:
        Serial.println("LOOP: ALL GREEN");
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.setPixelColor(i, strip.Color(0, 255, 0));
        }
        break;
      case 2:
        Serial.println("LOOP: ALL BLUE");
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.setPixelColor(i, strip.Color(0, 0, 255));
        }
        break;
      case 3:
        Serial.println("LOOP: ALL OFF");
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        break;
    }
    strip.show();
    phase++;
  }
  
  delay(10);
}