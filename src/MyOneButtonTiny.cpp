/**
 * @file OneButton.cpp
 *
 * @brief Library for detecting button clicks, doubleclicks and long press
 * pattern on a single button.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *                          Ihor Nehrutsa, Ihor.Nehrutsa@gmail.com
 *
 * This work is licensed under a BSD style license. See
 * http://www.mathertel.de/License.aspx
 *
 * More information on: https://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 *
 * Changelog: see MyOneButtonTiny.h
 */

#include "MyOneButtonTiny.h"

// ----- Initialization and Default Values -----
MyOneButtonTiny::MyOneButtonTiny(const int pin, const bool activeLow) {
  _pin = pin;
  _buttonPressed = activeLow ? LOW : HIGH;
  pinMode(pin, INPUT_PULLUP);
}

// save function for click event
void MyOneButtonTiny::attachClick(callbackFunction newFunction) {
  _clickFunc = newFunction;
}

// save function for longPressStart event
void MyOneButtonTiny::attachLongPressStart(callbackFunction newFunction) {
  _longPressStartFunc = newFunction;
}

void MyOneButtonTiny::reset(void) {
  _state = MyOneButtonTiny::OCS_INIT;
  _nClicks = 0;
  _startTime = 0;
}

void MyOneButtonTiny::tick(void) {
  if (_pin >= 0) {
    _fsm(digitalRead(_pin) == _buttonPressed);
  }
}

void MyOneButtonTiny::tick(bool activeLevel) {
  _fsm(activeLevel);
}

void MyOneButtonTiny::_newState(stateMachine_t nextState) {
  _state = nextState;
}

void MyOneButtonTiny::_fsm(bool activeLevel) {
  unsigned long waitTime = (millis() - _startTime);

  switch (_state) {
    case MyOneButtonTiny::OCS_INIT:
      if (activeLevel) {
        _newState(MyOneButtonTiny::OCS_DOWN);
        _startTime = millis();  // remember starting time
        _nClicks = 0;
      }
      break;

    case MyOneButtonTiny::OCS_DOWN:
      if (!activeLevel) {
        _newState(MyOneButtonTiny::OCS_UP);
        _startTime = millis();  // remember starting time

      } else if ((activeLevel) && (waitTime > _press_ms)) {
        if (_longPressStartFunc) _longPressStartFunc();
        _newState(MyOneButtonTiny::OCS_PRESS);
      }
      break;

    case MyOneButtonTiny::OCS_UP:
      _nClicks++;
      _newState(MyOneButtonTiny::OCS_COUNT);
      break;

    case MyOneButtonTiny::OCS_COUNT:
      if (activeLevel) {
        _newState(MyOneButtonTiny::OCS_DOWN);
        _startTime = millis();  // remember starting time

      } else if ((waitTime >= _click_ms) || (_nClicks == 2)) {
        if (_clickFunc) _clickFunc();

        reset();
      }
      break;

    case MyOneButtonTiny::OCS_PRESS:
      if (!activeLevel) {
        _newState(MyOneButtonTiny::OCS_PRESSEND);
        _startTime = millis();
      }
      break;

    case MyOneButtonTiny::OCS_PRESSEND:
      reset();
      break;

    default:
      _newState(MyOneButtonTiny::OCS_INIT);
      break;
  }
}
