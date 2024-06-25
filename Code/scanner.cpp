/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/Daeva
   ________________________________________ */

#include <Arduino.h> 
#include "scanner.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern Adafruit_NeoPixel pixels;

extern RCSwitch mySwitch;

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

const int buttonPin = 27;
volatile bool taskState = false;


static const uint32_t subghz_frequency_list[] = {
    
    300000000, 303875000, 304250000, 310000000, 315000000, 318000000,  // 300-348 MHz
    390000000, 418000000, 433075000, 433420000, 433920000, 434420000, 434775000, 438900000,  // 387-464 MHz
    868350000, 915000000, 925000000  // 779-928 MHz
};

#define BAR_WIDTH 2
#define BAR_SPACING 1
#define NUM_FREQUENCIES (SCREEN_WIDTH / (BAR_WIDTH + BAR_SPACING))

int rssiValues[NUM_FREQUENCIES] = {0};

#define WAVEFORM_SAMPLES 128
int waveform[WAVEFORM_SAMPLES] = {0};
int waveformIndex = 0;

void IRAM_ATTR handleButtonPress() {
  taskState = !taskState; 
}

void drawHistogram() {

  float mhz = 0; 
  int maxHeight = SCREEN_HEIGHT - 40;
  u8g2.clearBuffer();  

  for (int i = 0; i < NUM_FREQUENCIES; i++) {

    int barHeight = map(rssiValues[i], -100, -40, 0, maxHeight);
    u8g2.drawBox(i * (BAR_WIDTH + BAR_SPACING), SCREEN_HEIGHT - barHeight, BAR_WIDTH, barHeight);

    ELECHOUSE_cc1101.setMHZ(subghz_frequency_list[i % (sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]))] / 1000000.0);
    ELECHOUSE_cc1101.SetRx();
    delay(2); 

    int rssi = ELECHOUSE_cc1101.getRssi();
    rssiValues[i] = rssi;

  if(rssi < -75){              
                         
     if(i  % 2 == 0){
       ELECHOUSE_cc1101.setMHZ(433.92);          
       ELECHOUSE_cc1101.SetRx();
       mhz = 433.92;
       u8g2.drawStr(30, 10, "433.92");
      }
       else{
       ELECHOUSE_cc1101.setMHZ(315);          
       ELECHOUSE_cc1101.SetRx();
       mhz = 315.00;
       u8g2.drawStr(30, 10, "315.00");
     }
   }

    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(0, 10, "Freq:");
    //u8g2.setCursor(30, 10);
    //u8g2.print(mhz);
    u8g2.drawStr(70, 10, "MHz");
    
    Serial.println(mhz);

  }
  
  u8g2.sendBuffer();
}

void axis(){

        float mhz = 0;   
        u8g2.clearBuffer();    

        for (int i = 1; i < SCREEN_WIDTH; i++)
        {
              pixels.setPixelColor(0, pixels.Color(0, 2, 2));
              pixels.show();  
                                                  
              int rssi = ELECHOUSE_cc1101.getRssi();
              waveform[i] = map(rssi, -100, -40, 0, 1023); 
              
             if(rssi < -75){              
                         
                if(i  % 2 == 0){
                  ELECHOUSE_cc1101.setMHZ(433.92);          
                  ELECHOUSE_cc1101.SetRx();
                  mhz = 433.92;
                }
                else{
                  ELECHOUSE_cc1101.setMHZ(315);          
                  ELECHOUSE_cc1101.SetRx();
                  mhz = 315.00;
                }
            }

    int prevY = map(waveform[i - 1], 0, 1023, SCREEN_HEIGHT - 1, 8); 
    int currY = map(waveform[i], 0, 1023, SCREEN_HEIGHT - 1, 8);  

    u8g2.drawLine(i - 1, prevY, i, currY); 

    u8g2.sendBuffer();
    delay(30); 

    Serial.println(mhz);
    Serial.println(prevY);
    Serial.println(currY);

    //delay(500); 
  }
}

void scannerSetup(){

  Serial.begin(115200);

  u8g2.begin();

  ELECHOUSE_cc1101.Init(); 
  ELECHOUSE_cc1101.SetRx(); 

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);  
  
}


void scannerLoop(){

  if (taskState) {
    u8g2.clearBuffer();    
    axis();
    u8g2.sendBuffer();
  } else {
    u8g2.clearBuffer();    
    drawHistogram();
    u8g2.sendBuffer();
  }  
  
}
