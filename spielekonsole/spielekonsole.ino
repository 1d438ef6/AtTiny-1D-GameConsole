#include "MyOneButtonTiny.h"
#include <tinyNeoPixel.h>
#include "Player.h"
#include "Mastermind.h"

#define randomizerPin 0

#define NUM_LEDS 8
#define LED_PIN PB1  

tinyNeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
volatile bool buttonPressed = false;

enum gamestate_t: int8_t {
  win = -2,
  loss = -1,
  Menu = 0,
  game1 = 1,
  game2 = 2,
  game3 = 3,
  snake = 4,
  pong1player = 5,
  pong2player = 6,
  pong2consoles = 7,
  lednumber = 8
};

gamestate_t gamestate = Menu;
gamestate_t prevstate = Menu;
Player player(1, 1, 0, 20, 230, 20);

#define key1_PIN PB2
#define key2_PIN PB3
#define key3_PIN PB4

MyOneButtonTiny key1(key1_PIN, true);  
MyOneButtonTiny key2(key2_PIN, true);
MyOneButtonTiny key3(key3_PIN, true);

void setup() {
  randomSeed(analogRead(randomizerPin));
  
  strip.begin();
  strip.show(); 
  
  strip.setBrightness(50);

  key1.attachClick(singleClickkey1);
  key1.attachLongPressStart(longPresskey1);
  key2.attachClick(singleClickkey2);
  key3.attachClick(singleClickkey3);
}

void loop() {
  key1.tick();
  key2.tick();
  key3.tick();
  handleGameState();
  player.update();
  strip.show();
}

// Function to set all LEDs to a specific color
void setAllLEDs(uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  //strip.show();
}

void setLEDColor(int led, uint8_t red, uint8_t green, uint8_t blue) {
  if (led >= 0 && led < NUM_LEDS) {
    strip.setPixelColor(led, strip.Color(red, green, blue));
  }
  //strip.show();
}

int8_t getRandomDirection() {
    int randomValue = random(2); // Generiert eine Zahl zwischen 0 und 1
    return randomValue == 0 ? -1 : 1; // Gibt -1 oder 1 zurück
}

void awaitButtonPress(){
  while(!buttonPressed){
    key1.tick();
    key2.tick();
    key3.tick();
  }
}

uint8_t getRandomFruitLocation() {
    uint8_t numPixels = strip.numPixels();
    uint8_t playerSize = player.getSize();
    uint8_t playerPos = player.getPosition();
    int8_t playerDir = player.getDirection();

    uint8_t snakeStart = playerPos;
    uint8_t snakeEnd = playerPos + (playerDir * (playerSize - 1));

    if (snakeStart < 1) snakeStart = 1;
    if (snakeEnd >= numPixels - 1) snakeEnd = numPixels - 2;

    uint8_t numValidPositions = numPixels - 2 - playerSize;
    
    uint8_t pos = random(1, numValidPositions);

    if (pos >= snakeStart && pos <= snakeEnd) {
        pos += (snakeEnd - snakeStart + 1);
    }
    if (pos >= numPixels - 1) pos = numPixels - 2;
    
    return pos;
}

void pongOutOfBounds(){
  if(player.getDirection() == -1){
    for(int i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i, strip.Color(0,150,0)); 
      strip.show();
      delay(50);
    }
    prevstate = loss;
  }
  if(player.getDirection() == 1){
    for(int i=strip.numPixels();i>=0;i--){
      strip.setPixelColor(i, strip.Color(0,150,0)); 
      strip.show();
      delay(50);
    }
    prevstate = loss;
  }
}

void configurePong(){
  player.setdisableMovementCheck(false);
  player.registerOutOfBoundsHandler(pongOutOfBounds);
  player.setMoveDirection(getRandomDirection());

  player.setUpdateInterval(200);
  player.setSize(1);
  player.setColor(20, 230, 20);
  player.setPosition((int)(strip.numPixels()-2)/2);
  setAllLEDs(0, 0, 0);
}

void drawPong(bool twoSides, uint8_t ledNumber = 3){
  if(twoSides){
    setLEDColor(0, 255, 165, 0);
    setLEDColor(1, 170, 110, 0);
    setLEDColor(2, 85, 85, 0);

    setLEDColor(strip.numPixels()-3, 85, 85, 0);
    setLEDColor(strip.numPixels()-2, 170, 110, 0);
    setLEDColor(strip.numPixels()-1, 255, 165, 0);
  } else {
    setLEDColor(0, 255, 165, 0);
    setLEDColor(1, 170, 110, 0);
    setLEDColor(2, 85, 85, 0);
  }
}

int intervalToSpeed(unsigned long interval) {
    return map(interval, 20, 750, 0, 10);
}

uint8_t aiDecision(int rally = 10) {
    uint8_t speed = intervalToSpeed(player.getUpdateInterval());  // Convert interval to speed
    uint8_t choice = 1;  // Default to center
    float errorChance = 0.1;
    float actualErrorChance = errorChance + (rally / 100.0);  // Increase error chance with rally length
    
    // Define the miss chance
    float missChance = 0.05;  
    if (speed > 7) {
        missChance += 0.05;  // Increase miss chance when the ball is fast
    }

    uint8_t randomValue = random(0, 100);

    if (randomValue < missChance * 100) {
        return 0;  
    }
    
    if (randomValue < (actualErrorChance + missChance) * 100) {
        choice = random(0, 3);  // Make an error: choose a random zone
    } else {
        // Otherwise, choose based on ball speed
        if (speed > 7) {
            choice = 3;  // Slow down by choosing left
        } else if (speed < 4) {
            choice = 1;  // Speed up by choosing right
        } else {
            choice = 2;  // Maintain speed by choosing center
        }
    }

    return choice;
}

void handleGameState() {
  switch (gamestate) {
    case win:
      gamestate = prevstate;
      prevstate = win;
      break;
    case loss:
      gamestate = prevstate;
      prevstate = loss;
      break;
    case Menu:
      if(prevstate!=Menu){
        player.setMoveDirection(0);
        player.setUpdateInterval(500);
        player.setSize(1);
        player.setPosition(0);
        player.unregisterOutOfBoundsHandler();
        player.setColor(20, 230, 20);
        
        prevstate=Menu;
        player.setPosition(1);
      }
      setAllLEDs(128, 0, 128);
      break;
    case game1:
      if(prevstate!=game1){
        prevstate=game1;
        player.unregisterOutOfBoundsHandler();
        mastermind_init();
      }
      mastermind_loop();
      if (mastermind_win){
        gamestate = win;
        mastermind_win = false;
      }
      break;
    case game2:
      if(prevstate!=game2){
        player.unregisterOutOfBoundsHandler();
        prevstate=game2;
      }
      setAllLEDs(0, 0, 0);
      break;
    case game3:
      if(prevstate!=game3){
        prevstate=game3;
        player.setdisableMovementCheck(false);
        player.unregisterOutOfBoundsHandler();
        player.setMoveDirection(-1);//getRandomDirection());
        player.setUpdateInterval(500);
        player.setSize(1);
        player.setPosition(5);
        player.setColor(20, 230, 20);
        setAllLEDs(0, 0, 0);
        awaitButtonPress();
      }
      setAllLEDs(0, 0, 0);
      if((player.getPosition()<=0)||(player.getPosition()>=strip.numPixels()-1)){
        player.setMoveDirection(player.getDirection()*-1);
      }
      break;
    case snake:
      static uint8_t fruit;
      if(prevstate!=snake){
        prevstate=snake;
        player.setdisableMovementCheck(false);
        player.unregisterOutOfBoundsHandler();
        player.setMoveDirection(getRandomDirection());
        player.setUpdateInterval(500);
        player.setSize(1);
        player.setColor(20, 230, 20);
        player.setPosition((int)(strip.numPixels()-2)/2);
        buttonPressed = false;
        setAllLEDs(0, 0, 0);
        awaitButtonPress();
        fruit = getRandomFruitLocation();
      }
      setAllLEDs(0, 0, 0);
      setLEDColor(0, 128, 0, 128);
      setLEDColor(strip.numPixels()-1, 128, 0, 128);
      setLEDColor(fruit, 255, 165, 0);

      if(player.getPosition() == fruit){
        player.incSize();
        fruit = getRandomFruitLocation();
      }
      
      if(player.getSize() >= strip.numPixels()-2){
        //gewonnen
        gamestate = win;
      }
      if((player.getPosition() <= 0)||(player.getPosition() >= strip.numPixels()-1)){
        //verloren
        gamestate = loss;
      }
      break;
    case pong1player:
      static uint8_t aiChose;
      if(prevstate!=pong1player){
        prevstate=pong1player;
        configurePong();
        drawPong(true);
        awaitButtonPress();
      }
      setAllLEDs(0, 0, 0);
      drawPong(true);

      if((player.getPosition()==strip.numPixels()-4) && (player.getDirection() == 1)){
          aiChose = aiDecision();
      }
      if((aiChose>0)&&(player.getPosition()==strip.numPixels()-aiChose)){
        player.setMoveDirection(-1);
        unsigned long s = player.getUpdateInterval();
        if((aiChose == 0)&&(s > 70)){
          player.setUpdateInterval(s - 50);
        } else if ((aiChose == 2) && (s < 350)){
          player.setUpdateInterval(s + 50);
        }
      }

      break;
    case pong2player:
      if(prevstate!=pong2player){
        prevstate=pong2player;
        configurePong();
        drawPong(true);
        awaitButtonPress();
      }
      setAllLEDs(0, 0, 0);
      drawPong(true);
      break;
    case pong2consoles:
      if(prevstate!=pong2consoles){
        prevstate=pong2consoles;
        configurePong();
        drawPong(false);
        awaitButtonPress();
      }
      setAllLEDs(0, 0, 0);
      drawPong(false);
      break;
    case lednumber:
      if(prevstate!=lednumber){
        prevstate=lednumber;
        player.setMoveDirection(0);
        player.setSize(1);
      }
      setAllLEDs(20, 230, 20);
      break;
    default:
      break;
  }
}

void pongPressPlayer1(){
  uint8_t p = player.getPosition();
  if((player.getDirection()==-1) && (p < 4)){
    player.setMoveDirection(1);
    unsigned long s = player.getUpdateInterval();
    if((p == 0) && (s > 70)){
      player.setUpdateInterval(s - 50);
    } else if ((p == 2) && (s < 350)){
      player.setUpdateInterval(s + 50);
    }
  }
}

void pongPressPlayer2(){
  uint8_t p = player.getPosition();
  if((player.getDirection()==1) && (p >= strip.numPixels() - 3)){
    player.setMoveDirection(-1);
    unsigned long s = player.getUpdateInterval();
    if((p == strip.numPixels()-1) && (s > 70)){
      player.setUpdateInterval(s - 50);
    } else if ((p == strip.numPixels()-2) && (s < 350)){
      player.setUpdateInterval(s+50);
    }
  }
}

// Callback function for a short button press on key 1
void singleClickkey1() {
  buttonPressed = true;
  switch (gamestate) {
    case Menu:
      gamestate  = static_cast<gamestate_t>(player.getPosition()+1);
      break;
    case game1:
      mastermind_button1();
      break;
    case game2:
      break;
    case game3:
      if(player.getPosition()==2){
        unsigned long newInterval = player.getUpdateInterval() - player.getUpdateInterval() / 10;
        player.setUpdateInterval(max(newInterval, 10UL)); // Ensure interval doesn't go below 10ms
      } else {
        gamestate = loss;
      }
      break;
    case snake:
      player.setMoveDirection(-1*player.getDirection());
      player.setPosition(player.getPosition() + (player.getDirection() * (player.getSize() - 1)));
      break;
    case pong1player:
      pongPressPlayer1();
      break;
    case pong2player:
      pongPressPlayer1();
      break;
    case pong2consoles:
      pongPressPlayer1();
      break;
    case lednumber:
      gamestate = Menu;
      break;
    default:
      break;
  }
}

// Callback function for a long button press on key 1
void longPresskey1() {
  buttonPressed = true;
  gamestate = Menu;
}

// Callback function for a short button press on key 2
void singleClickkey2() {
  buttonPressed = true;
  switch (gamestate) {
    case Menu:
      player.moveleft();
      break;
    case game1:
      mastermind_button2();
      break;
    case game2:
      break;
    case game3:
      break;
    case snake:
      break;
    case pong1player:
      break;
    case pong2player:
      break;
    case pong2consoles:
      break;
    case lednumber:
      if(player.getSize()>1){
        strip.updateLength(strip.numPixels()-1);
      }
      break;
    default:
      break;
  }
}

// Callback function for a long button press on key 2
void longPresskey2() {
  buttonPressed = true;
}

// Callback function for a short button press on key 3
void singleClickkey3() {
  buttonPressed = true;
  switch (gamestate) {
    case Menu:
      player.moveright();
      break;
    case game1:
      mastermind_button3();
      break;
    case game2:
      break;
    case game3:
      break;
    case snake:
      break;
    case pong1player:
      break;
    case pong2player:
      pongPressPlayer2();
      break;
    case pong2consoles:
      break;
    case lednumber:
      if(player.getSize()>=strip.numPixels()){
        strip.updateLength(strip.numPixels()+1);
      }
      break;
    default:
      break;
  }
}

// Callback function for a long button press on key 3
void longPresskey3() {
  buttonPressed = true;
}
