#ifndef PLAYER_H
#define PLAYER_H

#include <TinyNeoPixel.h>

class Player {
public:
  Player(uint8_t startX, uint8_t startsize, int8_t startdirction, uint8_t r, uint8_t g, uint8_t b);

  void setColor(uint8_t r, uint8_t g, uint8_t b);
  void setSize(uint8_t newSize);
  uint8_t getSize();
  void incSize();
  void decSize();
  void setPosition(uint8_t newPosition);
  void move(int8_t dx);
  void moveleft();
  void moveright();
  void move_noCheck(int8_t dx);
  uint8_t getPosition();
  int8_t getDirection();

  void setMoveDirection(int8_t direction); // Methode zum Setzen der Bewegungsrichtung
  void setUpdateInterval(unsigned long interval); // Methode zum Setzen des Zeitintervalls
  unsigned long getUpdateInterval();

  void setdisableMovementCheck(bool moveMentCheck);
  void registerOutOfBoundsHandler(void (*func)());
  void unregisterOutOfBoundsHandler();
  void setAfterglow(bool afterglow);

  void update();
  
  void updateLEDs();

private:
  uint8_t x;             // Position of the player
  uint8_t playersize;            // Size of the player (number of LEDs)
  uint8_t colorR, colorG, colorB; // Color of the player
  int8_t move_direction;

  bool disableMovementCheck;
  bool afterGlow;

  unsigned long lastUpdateTime; // Zeit der letzten Aktualisierung
  unsigned long updateInterval;

  void (*outOfBoundsHandler)();

};

#endif // PLAYER_H
