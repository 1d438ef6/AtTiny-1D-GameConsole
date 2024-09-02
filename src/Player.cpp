#include "Player.h"

// Assume NUM_LEDS and LED_PIN are defined somewhere in your main file or in a config file
extern tinyNeoPixel strip;

Player::Player(uint8_t startX, uint8_t startsize, int8_t startdirection,uint8_t r, uint8_t g, uint8_t b)
  : x(startX), playersize(startsize), move_direction(startdirection), colorR(r), colorG(g), colorB(b) {
    this->unregisterOutOfBoundsHandler();
  }

void Player::setColor(uint8_t r, uint8_t g, uint8_t b) {
  this->colorR = r;
  this->colorG = g;
  this->colorB = b;
  updateLEDs();
}

void Player::setSize(uint8_t newSize) {
  this->playersize = newSize;
  updateLEDs();
}

uint8_t Player::getSize(){
  return this->playersize;
}

void Player::incSize(){
  this->setSize(playersize+1);
}

void Player::decSize(){
  if (this->playersize>1){
    this->setSize(playersize-1);
  }
}

void Player::setPosition(uint8_t newPosition){
  this->x = newPosition;
}

void Player::move(int8_t dx) {
  this->x += dx;
  if(!this->disableMovementCheck){
    if(this->x<0){
      if(this->outOfBoundsHandler != nullptr){
        this->outOfBoundsHandler();
      }
      this->x = 0; 
    } else if(this->x>=strip.numPixels()){
      if(this->outOfBoundsHandler != nullptr){
        this->outOfBoundsHandler();
      }
      this->x = strip.numPixels()-1;
    }
  }
  this->updateLEDs();
}

void Player::move_noCheck(int8_t dx) {
  this->x += dx;
  //updateLEDs();
}

void Player::moveleft(){
  this->move(-1);
}

void Player::moveright(){
  this->move(1);
}

void Player::setdisableMovementCheck(bool movementCheck){
  this->disableMovementCheck = movementCheck;
}

void Player::setAfterglow(bool afterglow){
  this->afterGlow = afterglow;
}

void Player::updateLEDs() {
  if ((this->move_direction == 0) || (this->move_direction == -1)){
    for (int i = 0; i < this->playersize; i++){
      strip.setPixelColor(x+i, strip.Color(this->colorR, this->colorG, this->colorB));
    }
  } else {
    for (int i = this->playersize-1; i > 0; i--){
      strip.setPixelColor(x-i, strip.Color(this->colorR, this->colorG, this->colorB));
    }
  }
  strip.setPixelColor(this->x, strip.Color(this->colorR, this->colorG, this->colorB));
}

uint8_t Player::getPosition(){
  return this->x;
}

int8_t Player::getDirection(){
  return this->move_direction;
}

void Player::setMoveDirection(int8_t direction) {
  this->move_direction = direction;
}

void Player::registerOutOfBoundsHandler(void (*func)()){
  this->outOfBoundsHandler = func;
}

void Player::unregisterOutOfBoundsHandler(){
  this->outOfBoundsHandler = nullptr;
}

void Player::setUpdateInterval(unsigned long interval) {
  this->updateInterval = interval;
}

unsigned long Player::getUpdateInterval(){
  return this->updateInterval;
}

void Player::update() {
  unsigned long currentTime = millis();
  if (currentTime - this->lastUpdateTime >= this->updateInterval) {
    if (this->move_direction == -1) {
      this->moveleft();
    } else if (this->move_direction == 1) {
      this->moveright();
    }
    this->lastUpdateTime = currentTime;
  }
  this->updateLEDs();
}
