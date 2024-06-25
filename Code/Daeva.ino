/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/Daeva
   ________________________________________ */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "icon.h"
#include "scanner.h"
#include "replay_attack.h"


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); // [full framebuffer, size = 1024 bytes]
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_NeoPixel pixels(1, 16, NEO_GRB + NEO_KHZ800);

RCSwitch mySwitch = RCSwitch();

 
const unsigned char* bitmap_icons[8] = {
  bitmap_icon_scanner,
  bitmap_icon_replay_attack,
  bitmap_icon_about
};


const int NUM_ITEMS = 3; 
const int MAX_ITEM_LENGTH = 20; 

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  
  { "Scanner" }, 
  { "Replay Attack" }, 
  { "About" }
 };
 

#define BUTTON_UP_PIN 17 
#define BUTTON_SELECT_PIN 25
#define BUTTON_DOWN_PIN 27 


int button_up_clicked = 0; 
int button_select_clicked = 0; 
int button_down_clicked = 0; 

int item_selected = 0; 

int item_sel_previous; 
int item_sel_next; 

int current_screen = 0;  


void about() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(7, 15, "CiferTech@gmail.com");
  u8g2.drawStr(12, 35, "GitHub/cifertech");
  u8g2.drawStr(7, 55, "instagram/cifertech");
  u8g2.sendBuffer();
}


void setup() {

  u8g2.begin();
  u8g2.setBitmapMode(1);

  pixels.begin();

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); 
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); 
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); 

}


void loop() {

  if (current_screen == 0) { // MENU SCREEN
    
      if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { 
        item_selected = item_selected - 1; 
        button_up_clicked = 1; 
        if (item_selected < 0) { 
          item_selected = NUM_ITEMS-1;
        }
      }
      else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { 
        item_selected = item_selected + 1; 
        button_down_clicked = 1; 
        if (item_selected >= NUM_ITEMS) { 
          item_selected = 0;
          }
      } 

      if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { 
        button_up_clicked = 0;
      }
      if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { 
        button_down_clicked = 0;
      }
  }


  bool callAbout = true;

  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { 
     button_select_clicked = 1; 
 
if (current_screen == 0 && item_selected == 1) {
  rpattackSetup();
    while (item_selected == 1) {
        if (digitalRead(BUTTON_SELECT_PIN) == HIGH) { 
          rpattackLoop();     
            if (callAbout) {                
                callAbout = false;  // Toggle the state to not call about()
            } else {
                break;  // Toggle the state to break the loop
                callAbout = true;  // Reset the state for the next cycle
            }

            while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
                // Wait for the button to be released
                
                if (callAbout = true){
                  break;
                }
            }
        }
    }
}    
   

if (current_screen == 0 && item_selected == 0) {
  scannerSetup();
    while (item_selected == 0) {
        if (digitalRead(BUTTON_SELECT_PIN) == HIGH) {       
            if (callAbout) {
                scannerLoop();   
                callAbout = false;  // Toggle the state to not call about()
            } else {
                break;  // Toggle the state to break the loop
                callAbout = true;  // Reset the state for the next cycle
            }

            while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
                // Wait for the button to be released
                if (callAbout = true){
                  break;
                }
            }
        }
    }
}  

  

if (current_screen == 0 && item_selected == 2) {
    while (item_selected == 2) {
        if (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
            if (callAbout) {
                about();
                callAbout = false;  // Toggle the state to not call about()
            } else {
                break;  // Toggle the state to break the loop
                callAbout = true;  // Reset the state for the next cycle
            }

            while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
                // Wait for the button to be released
                if (callAbout = true){
                  break;
                }
            }
        }
    }
  }
}  

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { 
    button_select_clicked = 0;
  }


  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} 
  item_sel_next = item_selected + 1;  
  if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} 



  u8g2.clearBuffer();  

    if (current_screen == 0) { 
      
      u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

      u8g2.setFont(u8g_font_7x14);
      u8g2.drawStr(25, 15, menu_items[item_sel_previous]); 
      u8g2.drawXBMP( 4, 2, 16, 16, bitmap_icons[item_sel_previous]);          

      u8g2.setFont(u8g_font_7x14B);    
      u8g2.drawStr(25, 15+20+2, menu_items[item_selected]);   
      u8g2.drawXBMP( 4, 24, 16, 16, bitmap_icons[item_selected]);     

      u8g2.setFont(u8g_font_7x14);     
      u8g2.drawStr(25, 15+20+20+2+2, menu_items[item_sel_next]);   
      u8g2.drawXBMP( 4, 46, 16, 16, bitmap_icons[item_sel_next]);  

      u8g2.drawXBMP(128-8, 0, 8, 64, bitmap_scrollbar_background);

      u8g2.drawBox(125, 64/NUM_ITEMS * item_selected, 3, 64/NUM_ITEMS);             
    } 
    
  u8g2.sendBuffer(); 

}
