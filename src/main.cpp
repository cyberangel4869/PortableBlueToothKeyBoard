#include <Arduino.h>
#include <Preferences.h>
#include "BoardMapMem.h"

#define USB_EN 1

#include "OLED_Driver.h"
#include "WS2812Driver.h"
#include "KeyBoard.h"
#include "Encoder.h"

#ifdef BT_EN
#include <BleKeyboard.h>
#include <BLESecurity.h>
#include <esp_sleep.h>
#endif

#ifdef USB_EN
#include "USBHIDKeyboard.h"  
#include "USBHIDConsumerControl.h"
#include "USB.h"
#endif



OLED_Driver oled;
WS2812Driver led;
Key keyboard;
Encoder encoder;
BoardMapMem mem;
#ifdef USB_EN
USBHIDKeyboard USBkeyboard;  
USBHIDConsumerControl USBconsumer;  
#endif
#ifdef BT_EN
BleKeyboard bleKeyboard("BLE Keyboard", "Maker", 100);
#endif



int8_t mode=0;
uint32_t Cycle=0;//轮询时间，毫秒
unsigned long LastPressTime=0;//上一次按下按键的系统时间，毫秒
Preferences prefs;
BoardMap* currentMap;   // 当前模式实际使用的映射

#define MODE0 0
#define MODE1 1
#define MODE2 2
#define MODE3 3
#define MODE4 4
#define WEBUI 5
void sendKeyPress(uint8_t keyCode, uint8_t modifier = 0);
void sendMediaKey(uint8_t keyCode);
void globalInit();


void setup() {
    globalInit();
}

void loop(){
    if(keyboard.pressed()){
        uint8_t KeyID = keyboard.getKeyNum();
        LastPressTime = millis();
        Cycle = 0;
        led.clear();

        if (KeyID < 10) {
            sendKeyPress(currentMap->KeyCodes[KeyID].KeyCode,
                         currentMap->KeyCodes[KeyID].FuctCode);
        }
        keyboard.clear();
    }

    if(encoder.turned()){
        
    }

    if(encoder.pressed()){
        uint8_t nextMode;
        switch (mode){
            case MODE0: nextMode = MODE1; break;
            case MODE1: nextMode = MODE2; break;
            case MODE2: nextMode = MODE3; break;
            case MODE3: nextMode = MODE4; break;
            case MODE4: nextMode = WEBUI; break;
            case WEBUI: nextMode = MODE0; break;
            default:    nextMode = MODE0; break;
        }

        if (nextMode == WEBUI) {
            oled.clear();
            oled.printCenter(10, "Web UI");
            oled.printCenter(20, "SSID:Keyboard");
            oled.printCenter(30, "PW:88888888");
            mode = nextMode;
        } 
        else {
            mem.Load(nextMode);
            led.setBrightness(mem.GetRGBBrightness());
            oled.setBrightness(mem.GetScreenBrightness());
            oled.clear();
            oled.printCenter(20,mem.GetName());
            mode = nextMode;
        }

        delay(50);
        encoder.clearPressed();
    }

    #ifdef BT_EN
    if(millis()-LastPressTime>60000){
        led.clear();
        Cycle=500;
    }
    if(millis()-LastPressTime>600000){
        oled.clear();
        led.clear();
        bleKeyboard.end();
        delay(500);
        keyboard.beginDeepSleep();
    }
    delay(Cycle);
    #endif
}


void sendKeyPress(uint8_t keyCode, uint8_t modifier) {
    #ifdef BT_EN
    if (bleKeyboard.isConnected()) {
        if (modifier == 0) {
            bleKeyboard.press(keyCode);
            bleKeyboard.releaseAll();
        } else {
            bleKeyboard.press(modifier);
            bleKeyboard.press(keyCode);
            bleKeyboard.releaseAll();
        }     
    }
    #endif
    #ifdef USB_EN
    if(modifier==0){
        USBkeyboard.press(keyCode);
        USBkeyboard.releaseAll();
    } else {
        USBkeyboard.press(modifier);
        USBkeyboard.press(keyCode);
        USBkeyboard.releaseAll();
    }
    #endif
}

void sendMediaKey(uint8_t keyCode) {
    #ifdef BT_EN
    if (bleKeyboard.isConnected()) {
        // 蓝牙模式下的多媒体键处理
        bleKeyboard.write(keyCode);
    }
    #endif
    #ifdef USB_EN
    USBconsumer.press(keyCode);
    USBconsumer.release();
    #endif
}

void globalInit(){
    LastPressTime=millis();
    oled.begin();
    keyboard.beginNormal();
    led.begin(50);
    encoder.begin();
    oled.clearBuffer();
    oled.setFont(u8g2_font_7x14_tr);
    #ifdef BT_EN
    bleKeyboard.begin();
    while(!bleKeyboard.isConnected()){
        led.fillColor(0xff0000);
        oled.printCenter(20,"Connecting...");
        delay(500);
        led.clear();
        delay(500);
    }
    led.fillColor(0x00ff00);
    oled.clear();
    oled.printCenter(20,"BT Keyboard");
    delay(1000);
    led.clear();
    #endif
    #ifdef USB_EN
    USB.begin();
    USBkeyboard.begin();
    USBconsumer.begin();
    oled.printCenter(20,"USB mode 0");
    #endif
}

