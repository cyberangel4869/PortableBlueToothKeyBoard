#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// ========== 引脚配置 ==========
#define LED_PIN     7
#define LED_COUNT   9

class WS2812Driver {
private:
    Adafruit_NeoPixel strip;
    int ledCount;
    int ledPin;

    const int IDtoLEDnum[9]={6,5,0,7,4,1,8,3,2};//按键编号和对应的LED号
    
    
public:
    // 构造函数
    WS2812Driver(int pin=LED_PIN, int count=LED_COUNT, uint16_t pixelType = NEO_GRB + NEO_KHZ800)
        : strip(count, pin, pixelType), ledCount(count), ledPin(pin) {
    }
    
    // 初始化函数
    void begin(uint8_t brightness = 50) {
        strip.begin();
        strip.setBrightness(brightness);
        strip.show();  // 初始化为全灭状态
    }

    //将按键编码转换为该按键下的LED的编码
    int num(uint8_t KeyID){
        return IDtoLEDnum[KeyID-1];
    }
    
    // 基础控制函数
    void setPixelColor(int index, uint32_t color) {
        if (index >= 0 && index < ledCount) {
            strip.setPixelColor(index, color);
        }
        strip.show();
    }
    
    void setPixelColor(int index, uint8_t r, uint8_t g, uint8_t b) {
        setPixelColor(index, strip.Color(r, g, b));
    }
    
/**
 * 将所有LED设置为同一颜色
 * @param color 颜色值（使用 strip.Color(R,G,B) 生成）
 */
    void fillColor(uint32_t color) {
        for (int i = 0; i < ledCount; i++) {
            strip.setPixelColor(i, color);
        }
        strip.show();
    }
    
    void fillColor(uint8_t r, uint8_t g, uint8_t b) {
        fillColor(strip.Color(r, g, b));
    }
    
    void clear() {
        fillColor(0, 0, 0);
    }
    
    void show() {
        strip.show();
    }
    
    void setBrightness(uint8_t brightness) {
        strip.setBrightness(brightness);
    }
    
    uint8_t getBrightness() {
        return strip.getBrightness();
    }
    
    int getLedCount() {
        return ledCount;
    }
    
    // 颜色转换辅助函数
    uint32_t rgbToColor(uint8_t r, uint8_t g, uint8_t b) {
        return strip.Color(r, g, b);
    }
    
    uint32_t hsvToColor(uint16_t hue, uint8_t saturation = 255, uint8_t value = 255) {
        return strip.ColorHSV(hue, saturation, value);
    }
    
/**
 * 流光溢彩效果（逐个点亮）
 * @param color 颜色值
 * @param wait  每步延迟时间（毫秒）
 */
    void colorWipe(uint32_t color, int waitMs = 50) {
        for (int i = 0; i < ledCount; i++) {
            strip.setPixelColor(i, color);
            strip.show();
            delay(waitMs);
        }
    }
    
    void colorWipe(uint8_t r, uint8_t g, uint8_t b, int waitMs = 50) {
        colorWipe(strip.Color(r, g, b), waitMs);
    }
    
/**
 * 彩虹循环效果（所有LED颜色渐变）
 * @param wait 每步延迟时间（毫秒）
 */
    void rainbowCycle(int waitMs = 10) {
        static uint16_t j = 0;
        uint8_t saturation = 255;
        uint8_t brightness = 128;
        
        for (int i = 0; i < ledCount; i++) {
            uint16_t hue = (i * 65536 / ledCount + j) % 65536;
            strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue, saturation, brightness)));
        }
        strip.show();
        delay(waitMs);
        j += 256;
    }
    
/**
 * 跑马灯效果（三色追逐）
 * @param color 颜色值
 * @param wait  每步延迟时间（毫秒）
 */
    void theaterChase(uint32_t color, int waitMs = 100) {
        for (int j = 0; j < 10; j++) {
            for (int q = 0; q < 3; q++) {
                for (int i = 0; i < ledCount; i += 3) {
                    strip.setPixelColor(i + q, color);
                }
                strip.show();
                delay(waitMs);
                for (int i = 0; i < ledCount; i += 3) {
                    strip.setPixelColor(i + q, 0);
                }
            }
        }
    }
    
    void theaterChase(uint8_t r, uint8_t g, uint8_t b, int waitMs = 100) {
        theaterChase(strip.Color(r, g, b), waitMs);
    }
    
/**
 * 呼吸灯效果（渐亮渐灭）
 * @param color 颜色值
 * @param duration 一个完整呼吸周期的时长（秒）
 */
    void breathingLight(uint32_t color, float durationSec = 2.0, int cycles = 3) {
        float halfDuration = durationSec * 1000 / 2;
        
        for (int cycle = 0; cycle < cycles; cycle++) {
            unsigned long startCycle = millis();
            
            while ((millis() - startCycle) < durationSec * 1000) {
                unsigned long elapsed = millis() - startCycle;
                
                float brightness;
                if (elapsed < halfDuration) {
                    brightness = (elapsed / halfDuration) * 255;
                } else {
                    brightness = (1 - (elapsed - halfDuration) / halfDuration) * 255;
                }
                
                uint8_t r = (color >> 16) & 0xFF;
                uint8_t g = (color >> 8) & 0xFF;
                uint8_t b = color & 0xFF;
                
                uint8_t r_adj = r * brightness / 255;
                uint8_t g_adj = g * brightness / 255;
                uint8_t b_adj = b * brightness / 255;
                
                fillColor(strip.Color(r_adj, g_adj, b_adj));
                delay(10);
            }
        }
        
        clear();
    }
    
    void breathingLight(uint8_t r, uint8_t g, uint8_t b, float durationSec = 2.0, int cycles = 3) {
        breathingLight(strip.Color(r, g, b), durationSec, cycles);
    }
    

    // ========== 流水灯效果 ==========
    void runningLight(uint32_t color, int waitMs = 50, int loops = 3) {
        for (int loop = 0; loop < loops; loop++) {
            for (int i = 0; i < ledCount; i++) {
                clear();
                setPixelColor(i, color);
                show();
                delay(waitMs);
            }
        }
        clear();
    }
    
    // ========== 双色交替效果 ==========
    void alternateColors(uint32_t color1, uint32_t color2, int waitMs = 500, int cycles = 5) {
        for (int cycle = 0; cycle < cycles; cycle++) {
            fillColor(color1);
            delay(waitMs);
            fillColor(color2);
            delay(waitMs);
        }
        clear();
    }
    
    // ========== 渐变效果 ==========
    void fadeToColor(uint32_t targetColor, int steps = 50, int stepDelayMs = 20) {
        uint8_t currentR = 0, currentG = 0, currentB = 0;
        uint8_t targetR = (targetColor >> 16) & 0xFF;
        uint8_t targetG = (targetColor >> 8) & 0xFF;
        uint8_t targetB = targetColor & 0xFF;
        
        for (int step = 0; step <= steps; step++) {
            uint8_t r = currentR + (targetR - currentR) * step / steps;
            uint8_t g = currentG + (targetG - currentG) * step / steps;
            uint8_t b = currentB + (targetB - currentB) * step / steps;
            
            fillColor(r, g, b);
            delay(stepDelayMs);
        }
    }
};

#endif // WS2812_DRIVER_H