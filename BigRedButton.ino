#include "Keyboard.h"
#include "Mouse.h"
#include <FastLED.h>

#define LED_PIN 9    
#define NUM_LEDS 16
#define BUTTON_PIN 10

#define RANDOM_FLASHES_TIMEOUT_MS 10000
#define WAITING_MODE_MOVE_TIME 50

CRGB leds[NUM_LEDS];

uint32_t last_button_press_ms;

void setup() {
  last_button_press_ms = millis();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
  delay(1); 
  Mouse.begin();
  Keyboard.begin();
  randomSeed(analogRead(0));
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW) {
    last_button_press_ms = millis();
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255,0,0);
    }
    FastLED.show();
    delay(10);
    Mouse.click(MOUSE_LEFT);
    delay(1);
    Keyboard.write('\n');
    delay(300);
  } else {
    if(millis() - last_button_press_ms > RANDOM_FLASHES_TIMEOUT_MS) {
      RandomFlashesMode();
    } else {
      WaitingMode();
    }
  }
  delay(1);
}

void WaitingMode() {
  static uint32_t timer = millis();
  static int8_t current_led = 0;
  if(millis() - timer > WAITING_MODE_MOVE_TIME) {
    FastLED.clear();
    timer = millis();
    current_led++;
    if(current_led > NUM_LEDS-1) {
      current_led = 0;
    }
  }
  leds[current_led] = CRGB::Yellow;
  FastLED.show();
  delay(1);
}

#define LED_SELECT_PERIOD 70
#define FADE_DELAY 5

void RandomFlashesMode() {
  static uint8_t brightness[NUM_LEDS] = {0};
  static int8_t dirs[NUM_LEDS] = {1};
  static uint8_t colors[NUM_LEDS] = {0};
  static uint32_t led_select_timer = millis();
  if(millis() - led_select_timer > LED_SELECT_PERIOD) {
    led_select_timer = millis();
    if(random(3) == 0) {
      int8_t it = random(NUM_LEDS);
      if(brightness[it] == 0) {
        colors[it] = random(256);
        leds[it] = CHSV(colors[it],255, 1);
        brightness[it] = 1;
      }
    }
  }
  static uint32_t fade_timer = millis();
  if(millis() - fade_timer > FADE_DELAY) {
    fade_timer = millis();
    for(int i = 0; i < NUM_LEDS; i++) {
      if(brightness[i] > 0) {
        brightness[i] += dirs[i];
      }
      leds[i] = CHSV(colors[i],255, brightness[i]);
      if(brightness[i] == 255) {
        dirs[i] = -1;
      }
      if(brightness[i] == 0) {
        dirs[i] = 1;
      }
    }
  }
  FastLED.show();
}