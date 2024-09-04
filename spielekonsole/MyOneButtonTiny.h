// -----
// MyOneButtonTiny.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button. This class is implemented for use with the
// Arduino environment. Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// 01.12.2023 created from MyOneButtonTiny to support tiny environments.
// -----

#ifndef MyOneButtonTiny_h
#define MyOneButtonTiny_h

#include "Arduino.h"

// ----- Callback function types -----
extern "C" {
  typedef void (*callbackFunction)(void);
}

class MyOneButtonTiny {
public:
  // ----- Constructor -----
  explicit MyOneButtonTiny(const int pin, const bool activeLow = true);

  // ----- Attach events functions -----
  void attachClick(callbackFunction newFunction);
  void attachLongPressStart(callbackFunction newFunction);

  // ----- State machine functions -----
  void tick(void);
  void tick(bool level);
  void reset(void);

  bool isIdle() const {
    return _state == OCS_INIT;
  }

private:
  int _pin = -1;                   // hardware pin number.
  unsigned int _click_ms = 400;    // number of msecs before a click is detected.
  unsigned int _press_ms = 800;    // number of msecs before a long button press is detected

  int _buttonPressed = 0;  // this is the level of the input pin when the button is pressed.

  // These variables will hold functions acting as event source.
  callbackFunction _clickFunc = NULL;
  callbackFunction _longPressStartFunc = NULL;

  // define FiniteStateMachine
  enum stateMachine_t : int {
    OCS_INIT = 0,
    OCS_DOWN = 1,  // button is down
    OCS_UP = 2,    // button is up
    OCS_COUNT = 3,
    OCS_PRESS = 6,  // button is hold down
    OCS_PRESSEND = 7,
  };

  void _fsm(bool activeLevel);
  void _newState(stateMachine_t nextState);

  stateMachine_t _state = OCS_INIT;

  unsigned long _startTime = 0;  // start of current input change to checking debouncing
  int _nClicks = 0;              // count the number of clicks with this variable

public:
  int pin() const {
    return _pin;
  };
  stateMachine_t state() const {
    return _state;
  };
};

#endif
