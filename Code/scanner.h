/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/Daeva
   ________________________________________ */

#ifndef scanner_H
#define scanner_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <Adafruit_NeoPixel.h>
#include <RCSwitch.h>
#include <U8g2lib.h>
#include <Wire.h>

void scannerSetup();
void scannerLoop();

#endif
