#include <Arduino.h>
#include "USB.h"

// #include "USBHIDKeyboard.h"  // 注释掉USB键盘
// #include "USBHIDConsumerControl.h"  // 注释掉USB消费者控制
#include "OLED_Driver.h"
#include "WS2812Driver.h"
#include "KeyBoard.h"
#include "Encoder.h"
#include <BleKeyboard.h>
#include <BLESecurity.h>
#include <esp_sleep.h>

//#define DEBUG 1
//#define DEBUGKEYS 1
//#define DEBUGLOWPOWER 1

OLED_Driver oled;
WS2812Driver led;
Key keyboard;
Encoder encoder;
// USBHIDKeyboard USBkeyboard;  // 注释掉
// USBHIDConsumerControl USBconsumer;  // 注释掉
BleKeyboard bleKeyboard("BLE Keyboard", "Maker", 100);

#define MODE_PIN 12

struct KeyMap{
    uint8_t KeyCode;
    uint8_t FuctCode;
};

// 使用键值
KeyMap KeyCodes[10]={
    {0,0},
    {0xD4,0},  // KEY_DELETE
    {0,0},
    {0xB0,0},  // KEY_RETURN
    {'c',0x80}, // KEY_LEFT_CTRL_C
    {0xDA,0},  // KEY_UP_ARROW
    {'v',0x80}, // KEY_LEFT_CTRL_V
    {0xD8,0},  // KEY_LEFT_ARROW
    {0xD9,0},  // KEY_DOWN_ARROW
    {0xD7,0}   // KEY_RIGHT_ARROW
};

int8_t mode=0;
// bool usbMode = true;  // 注释掉模式选择
bool bluetoothOnly = true;  // 固定为蓝牙模式
uint32_t Cycle=0;//轮询时间，毫秒
unsigned long LastPressTime=0;//上一次按下按键的系统时间，毫秒

#define KEY_BOARD 0
#define VOLUME_ADJ 1
#define SCREEN_BRICHT_ADJ 2
#define RGB_BRIGHT_ADJ 3
void sendKeyPress(uint8_t keyCode, uint8_t modifier = 0);
void sendMediaKey(uint8_t keyCode);
void globalInit();


void setup() {
    globalInit();
}

void loop(){
    if(keyboard.pressed()){
        uint8_t KeyID = keyboard.getKeyNum();
        LastPressTime=millis();//记录系统时间
        Cycle=0;//轮询周期5ms
        led.clear();
        if(KeyID==0){
            sendKeyPress('3');
            sendKeyPress('1');
        }
        else{
        if(KeyCodes[KeyID].FuctCode == 0){
            sendKeyPress(KeyCodes[KeyID].KeyCode);
        } else {
            sendKeyPress(KeyCodes[KeyID].KeyCode, KeyCodes[KeyID].FuctCode);
        }
    }
        led.setPixelColor(led.num(KeyID), 0x66, 0xcc, 0xff);
        keyboard.clear();
    }
    if(encoder.turned()){
        Serial0.println("encoder turned");
        switch (mode)
        {
        case KEY_BOARD:
            break;
        case VOLUME_ADJ:
            if(encoder.getDirection() > 0){
                sendMediaKey(0x00);
                encoder.clearAll();
            } else {
                sendMediaKey(0x00);
                encoder.clearAll();
            }
            break;
        case SCREEN_BRICHT_ADJ:
            encoder.clearAll();
            break;
        case RGB_BRIGHT_ADJ:
            uint8_t brit = led.getBrightness();            
            if(encoder.getDirection()>0){
                brit=brit+10;
            }
            else{
                brit=brit-10;
            }
            if(brit<=255&&brit>=10){
                led.begin((uint8_t)brit);
            }
            encoder.clearAll();
            delay(50);
            break;
        }
    }
    if(encoder.pressed()){
        switch (mode)
        {
            case KEY_BOARD:
                mode = VOLUME_ADJ;
                oled.clear();
                oled.printCenter(20, "Volume");
                delay(50);
                encoder.clearPressed();
                break;
            case VOLUME_ADJ:
                mode = SCREEN_BRICHT_ADJ;
                oled.clear();
                oled.printCenter(12, "Screen");
                oled.printCenterNoClear(25, "Brightness");
                delay(50);
                encoder.clearPressed();
                break;
            case SCREEN_BRICHT_ADJ:
                mode = RGB_BRIGHT_ADJ;
                oled.clear();
                oled.printCenter(20, "RGB brightness");
                delay(50);
                encoder.clearPressed();
                break;
            case RGB_BRIGHT_ADJ:
                mode = KEY_BOARD;
                oled.clear();
                // 显示蓝牙连接状态
                if (bleKeyboard.isConnected()) {
                    oled.printCenter(20, "BT Connected");
                } else {
                    oled.printCenter(20, "BT Waiting");
                }
                delay(50);
                encoder.clearPressed();
                break;
        }
        
    }
    //一分钟内没有按下新按键，降低轮询频率
    if(millis()-LastPressTime>60000){
        led.clear();
        Cycle=500;//0.5秒轮询一次
    }
    //10分钟内没有新的按键按下，进入休眠模式
    if(millis()-LastPressTime>600000){
        oled.clear();
        led.clear();
        bleKeyboard.end();
        delay(500);
        keyboard.beginDeepSleep();
    }
    delay(Cycle);//轮询周期
}


void sendKeyPress(uint8_t keyCode, uint8_t modifier) {
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
}

void sendMediaKey(uint8_t keyCode) {
    // 只保留蓝牙模式
    if (bleKeyboard.isConnected()) {
        // 蓝牙模式下的多媒体键处理
        bleKeyboard.write(keyCode);
    }
}

void globalInit(){
    LastPressTime=millis();
    oled.begin();
    keyboard.beginNormal();
    led.begin(100);
    encoder.begin();
    bleKeyboard.begin();
    oled.clearBuffer();
    oled.setFont(u8g2_font_7x14_tr);
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
}

