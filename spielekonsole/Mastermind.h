#ifndef MASTERMIND_H
#define MASTERMIND_H

#include "Player.h"
#include <tinyNeoPixel.h>

extern tinyNeoPixel strip;
extern Player player;

uint8_t correct[4];
uint8_t guessed[4];
uint32_t hints[4];
const uint32_t colors[] = {
  strip.Color(255, 0, 0),
  strip.Color(0, 255, 0),
  strip.Color(0, 0, 255),
  strip.Color(128, 0, 128)
};
const uint8_t NUM_COLORS = sizeof(colors)/sizeof(colors[0]);

bool mastermind_win = false;

void generate_code(){
  for (int i = 0; i < 4; i++) {
    correct[i] = random(NUM_COLORS);
  }
}


void checkCode(){
  uint8_t samePositioncounter = 0;
  uint8_t differentPositionCounter = 0;
  bool used[4] = {false, false, false, false}; // Array, um verwendete Positionen zu markieren
  
  for (int i = 0; i < 4; i++) {
    if (guessed[i] == correct[i]) {
      samePositioncounter++;
      used[i] = true; // Markiere die Position als verwendet
    } 
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      // Nur prüfen, wenn Positionen unterschiedlich sind und die Position nicht bereits verwendet wurde
      if (i != j && guessed[i] == correct[j] && !used[j]) {
        differentPositionCounter++;
        used[j] = true; // Markiere die Position als verwendet
        break;  // Aus der inneren Schleife aussteigen, um Doppelzählung zu vermeiden
      }
    }
  }

  for (int i = 0; i<4; i++){
    hints[i] = strip.Color(0, 0, 0);
  }

  if(samePositioncounter>=4){
    mastermind_win = true;
  }
  
  uint8_t pos = 0;
  for (int i = 0; i<samePositioncounter; i++){
    hints[pos] = strip.Color(255, 255, 255);
    pos++;
  }
  for (int i = 0; i<differentPositionCounter; i++){
    hints[pos] = strip.Color(255, 165, 0);
    pos++;
  }
}

void mastermind_init(uint8_t nPlayer = 1){
  for (int i = 0; i < 4; i++) {
    hints[i] = strip.Color(0, 0, 0);
    guessed[i] = 0;
  }
  generate_code();
  player.setColor(165, 150, 10);
  player.setPosition(0);
  player.setSize(1);
}

void mastermind_button1(){
  checkCode();
}

void mastermind_button3(){
  player.moveright();
  if(player.getPosition()>=4){
    player.setPosition(0);
  }
  player.setColor(165, 150, 10);
}

void mastermind_button2(){
  uint8_t p = player.getPosition();
  guessed[p]++;
  if(guessed[p]>=NUM_COLORS){
    guessed[p] = 0;
  }
  player.setColor(colors[guessed[p]]);
}

void mastermind_loop(){
  for (int i = 0; i < 4; i++) {
      strip.setPixelColor(i, colors[guessed[i]]);
  }
  for (int i = 0; i < 4; i++) {
      strip.setPixelColor(i+4, hints[i]);
  }
}



#endif 
