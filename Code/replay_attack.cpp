/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/Daeva
   ________________________________________ */

#include <Arduino.h> 
#include "replay_attack.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern Adafruit_NeoPixel pixels;

extern RCSwitch mySwitch;

#define RX_PIN 4         
#define TX_PIN 2        

#define BUTTON_PIN 27    

#define ROTARY_PIN_A 12     
#define ROTARY_PIN_B 15     
#define ROTARY_BUTTON_PIN 27 
  
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

RotaryEncoder encoder(ROTARY_PIN_A, ROTARY_PIN_B);

unsigned long receivedValue = 0; 
int receivedBitLength = 0;       
int receivedProtocol = 0;       
const int rssi_threshold = -75; 

static const uint32_t subghz_frequency_list[] = {
    300000000, 303875000, 304250000, 310000000, 315000000, 318000000,  //  300-348 MHz
    390000000, 418000000, 433075000, 433420000, 433920000, 434420000, 434775000, 438900000,  //  387-464 MHz
    868350000, 868000000, 915000000, 925000000  //  779-928 MHz
};

int currentFrequencyIndex = 0; 

void rpattackSetup(){
  
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
 
    u8g2.setCursor(0, 10);
    u8g2.print("Received:");
    u8g2.setCursor(70, 10);
    u8g2.print("000000");
    u8g2.setCursor(0, 30);
    u8g2.printf("Bit: -");
    u8g2.setCursor(40, 30);
    u8g2.printf("Ptc: -");
    u8g2.setCursor(0, 50);
    u8g2.print("Freq:");  
    u8g2.setCursor(40, 50);
    u8g2.print("0");
    u8g2.print(" MHz");
    u8g2.sendBuffer();
    delay(1000);
    u8g2.clearBuffer();

  ELECHOUSE_cc1101.Init(); 
  ELECHOUSE_cc1101.SetRx(); 

  mySwitch.enableReceive(RX_PIN); 
  mySwitch.enableTransmit(TX_PIN);   
}


void rpattackLoop(){
  
  static int lastPosition = -1;
  
  encoder.tick(); 
  int newPosition = encoder.getPosition();
  
  if (newPosition != lastPosition) {
    lastPosition = newPosition;
    currentFrequencyIndex = (newPosition + sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0])) % (sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]));
    ELECHOUSE_cc1101.setMHZ(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
    
      u8g2.clearBuffer();
      u8g2.setCursor(0, 10);
      u8g2.print("Received:");
      u8g2.setCursor(70, 10);
      u8g2.print(receivedValue);
      u8g2.setCursor(0, 30);
      u8g2.printf("Bit:%d ", receivedBitLength);
      u8g2.setCursor(40, 30);
      u8g2.printf("Ptc:%d ", receivedProtocol);
      u8g2.setCursor(0, 50);
      u8g2.print("Freq:");
      u8g2.setCursor(40, 50);
      u8g2.print(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
      u8g2.print(" MHz");
      u8g2.sendBuffer();
  }

  if (mySwitch.available()) { 
    receivedValue = mySwitch.getReceivedValue(); 
    receivedBitLength = mySwitch.getReceivedBitlength(); 
    unsigned int* rawSignal = mySwitch.getReceivedRawdata(); 
    receivedProtocol = mySwitch.getReceivedProtocol(); 

    if (receivedValue != 0) { 
      u8g2.clearBuffer();
      u8g2.setCursor(0, 10);
      u8g2.print("Received:");
      u8g2.setCursor(70, 10);
      u8g2.print(receivedValue);
      u8g2.setCursor(0, 30);
      u8g2.printf("Bit:%d ", receivedBitLength);
      u8g2.setCursor(40, 30);
      u8g2.printf("Ptc:%d ", receivedProtocol);    
      u8g2.setCursor(0, 50);
      u8g2.print("Freq:");
      u8g2.setCursor(40, 50);
      u8g2.print(subghz_frequency_list[currentFrequencyIndex] / 1000000.0);
      u8g2.print(" MHz");      
      u8g2.sendBuffer();

      mySwitch.resetAvailable(); 
    }
  }

  if (digitalRead(BUTTON_PIN) == LOW && receivedValue != 0) { 
    mySwitch.disableReceive(); 
    delay(100);
    mySwitch.enableTransmit(TX_PIN); 
    ELECHOUSE_cc1101.SetTx();

    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Value:");
    u8g2.setCursor(40, 10);
    u8g2.print(receivedValue);
    u8g2.setCursor(0, 30);
    u8g2.print("Sending...");
    u8g2.sendBuffer();

    mySwitch.setProtocol(receivedProtocol);
    mySwitch.send(receivedValue, receivedBitLength); 

    delay(500);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Value:");
    u8g2.setCursor(40, 10);
    u8g2.print(receivedValue);
    u8g2.setCursor(0, 30);
    u8g2.print("Done!");
    u8g2.sendBuffer();

    ELECHOUSE_cc1101.SetRx(); 
    mySwitch.disableTransmit(); 
    delay(100);
    mySwitch.enableReceive(RX_PIN); 
  }  
}
