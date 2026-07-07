#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// ========== 引脚配置（可根据需要修改）==========
#ifndef OLED_SDA
#define OLED_SDA 40
#endif

#ifndef OLED_SCL
#define OLED_SCL 41
#endif

#ifndef OLED_RESET_PIN
#define OLED_RESET_PIN 42
#endif

#ifndef OLED_ADDRESS
#define OLED_ADDRESS 0x3C
#endif

// ========== 屏幕尺寸配置 ==========
#define OLED_WIDTH 128
#define OLED_HEIGHT 32

// ========== 旋转模式配置 ==========
// 可选值: U8G2_R0(0°), U8G2_R1(90°), U8G2_R2(180°), U8G2_R3(270°)
#ifndef OLED_ROTATION
#define OLED_ROTATION U8G2_R2
#endif

// ========== OLED 驱动类 ==========
class OLED_Driver {
private:
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display;
    int sdaPin;
    int sclPin;
    int resetPin;
    bool isInitialized;
    uint8_t currentBrightness;
    
    // 硬件复位
    void hardReset() {
        if (resetPin >= 0) {
            pinMode(resetPin, OUTPUT);
            digitalWrite(resetPin, LOW);
            delay(10);
            digitalWrite(resetPin, HIGH);
            delay(10);
        }
    }
    
public:
    // ========== 构造函数 ==========
    // 默认构造函数（使用宏定义引脚）
    OLED_Driver() 
        : display(OLED_ROTATION, U8X8_PIN_NONE), 
          sdaPin(OLED_SDA), 
          sclPin(OLED_SCL), 
          resetPin(OLED_RESET_PIN),
          isInitialized(false),
          currentBrightness(128) {}
    
    
    // ========== 初始化函数 ==========
    bool begin() {
        // 执行硬件复位（如果有复位引脚）
        if (resetPin >= 0) {
            hardReset();
        }
        
        // 初始化 I2C 总线
        Wire.begin(sdaPin, sclPin);
        Wire.setClock(400000);  // 400kHz I2C 时钟
        
        // 初始化显示
        display.begin();
        display.enableUTF8Print();     // 启用 UTF8 支持
        display.setFont(u8g2_font_6x13_tf);  // 默认字体
        display.setContrast(currentBrightness);
        
        isInitialized = true;
        
        // 清屏
        clear();
        
        return true;
    }
    
    // ========== 清屏 ==========
    void clear() {
        if (!isInitialized) return;
        display.clearBuffer();
        display.sendBuffer();
    }
    
    // ========== 更新显示（发送缓冲区到屏幕）==========
    void update() {
        if (!isInitialized) return;
        display.sendBuffer();
    }
    
    // ========== 清空缓冲区（不清屏）==========
    void clearBuffer() {
        if (!isInitialized) return;
        display.clearBuffer();
    }
    
    // ========== 设置光标位置 ==========
    void setCursor(int x, int y) {
        if (!isInitialized) return;
        display.setCursor(x, y);
    }
    
    // ========== 设置字体 ==========
    void setFont(const uint8_t* font) {
        if (!isInitialized) return;
        display.setFont(font);
    }
    
    // ========== 设置字体方向 ==========
    void setFontDirection(uint8_t dir) {
        if (!isInitialized) return;
        display.setFontDirection(dir);
    }
    
    // ========== 设置亮度/对比度 ==========
    void setBrightness(uint8_t brightness) {
        if (!isInitialized) return;
        currentBrightness = brightness;
        display.setContrast(brightness);
    }
    
    uint8_t getBrightness() {
        return currentBrightness;
    }
    
    // ========== 设置显示模式 ==========
    void setPowerSave(bool enable) {
        if (!isInitialized) return;
        if (enable) {
            display.setPowerSave(1);
        } else {
            display.setPowerSave(0);
        }
    }
    
    // ========== 打印字符串到当前光标的位置==========
    void print(const char* text) {
        if (!isInitialized) return;
        display.print(text);
    }
    
    void print(int number) {
        if (!isInitialized) return;
        display.print(number);
    }
    
    void print(float number, int decimalPlaces = 2) {
        if (!isInitialized) return;
        display.print(number, decimalPlaces);
    }

    
    // ========== 打印并立即显示==========
    void printAt(int x, int y, const char* text) {
        if (!isInitialized) return;
        setCursor(x, y);
        print(text);
        update();
    }

    void printAt(int x, int y, int number) {
        if (!isInitialized) return;
        setCursor(x, y);
        print(number);
        update();
    }
    
    // ========== 居中打印（自动计算 X 位置）==========
    void printCenter(int y, const char* text) {
        if (!isInitialized) return;
        int textWidth = display.getStrWidth(text);
        int x = (OLED_WIDTH - textWidth) / 2;
        printAt(x, y, text);
    }
    
    // ========== 居中打印（使用当前字体，不清屏）==========
    void printCenterNoClear(int y, const char* text) {
        if (!isInitialized) return;
        int textWidth = display.getStrWidth(text);
        int x = (OLED_WIDTH - textWidth) / 2;
        setCursor(x, y);
        print(text);
        update();
    }
    
    // ========== 右对齐打印 ==========
    void printRight(int y, const char* text) {
        if (!isInitialized) return;
        int textWidth = display.getStrWidth(text);
        int x = OLED_WIDTH - textWidth;
        printAt(x, y, text);
    }
    
    // ========== 获取字符串宽度（像素）==========
    int getTextWidth(const char* text) {
        if (!isInitialized) return 0;
        return display.getStrWidth(text);
    }
    
    // ========== 绘制像素点 ==========
    void drawPixel(int x, int y) {
        if (!isInitialized) return;
        display.drawPixel(x, y);
    }
    
    // ========== 绘制线条 ==========
    void drawLine(int x1, int y1, int x2, int y2) {
        if (!isInitialized) return;
        display.drawLine(x1, y1, x2, y2);
    }
    
    // ========== 绘制矩形框 ==========
    void drawFrame(int x, int y, int width, int height) {
        if (!isInitialized) return;
        display.drawFrame(x, y, width, height);
    }
    
    void drawBox(int x, int y, int width, int height) {
        if (!isInitialized) return;
        display.drawBox(x, y, width, height);
    }
    
    // ========== 绘制圆 ==========
    void drawCircle(int x, int y, int radius) {
        if (!isInitialized) return;
        display.drawCircle(x, y, radius);
    }
    
    void drawDisc(int x, int y, int radius) {
        if (!isInitialized) return;
        display.drawDisc(x, y, radius);
    }
    
    // ========== 显示图片（位图）==========
    void drawBitmap(int x, int y, int width, int height, const uint8_t* bitmap) {
        if (!isInitialized) return;
        display.drawXBMP(x, y, width, height, bitmap);
    }
    
    // ========== 获取屏幕尺寸 ==========
    int getWidth() { return OLED_WIDTH; }
    int getHeight() { return OLED_HEIGHT; }
    
    // ========== 检测 OLED 是否响应 ==========
    bool isConnected() {
        Wire.beginTransmission(OLED_ADDRESS);
        return Wire.endTransmission() == 0;
    }
    
    // ========== 软件复位（重新初始化）==========
    void softReset() {
        if (!isInitialized) return;
        clear();
        delay(10);
    }
    
    // ========== 完全复位（硬件+软件）==========
    void fullReset() {
        if (resetPin >= 0) {
            hardReset();
        }
        if (isInitialized) {
            display.begin();
            display.enableUTF8Print();
            display.setFont(u8g2_font_6x13_tf);
            display.setContrast(currentBrightness);
            clear();
        }
    }
    
    // ========== 获取 U8g2 对象（用于高级操作）==========
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C* getDisplay() {
        return &display;
    }
};

#endif // OLED_DRIVER_H