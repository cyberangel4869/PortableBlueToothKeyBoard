#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

// ========== 编码器引脚配置（硬件固定，直接宏定义）==========
#define ENC_A    6
#define ENC_B    5
#define ENC_SW   4

class Encoder {
private:
    // 中断共享变量
    volatile static bool _turned;
    volatile static int8_t _direction;
    volatile static bool _pressed;
    
    // 静态中断回调函数
    static void onEncoderA() {
        // 根据B相电平判断方向
        if (digitalRead(ENC_B) == HIGH) {
            _direction = -1;   
        } else {
            _direction = 1;  
        }
        _turned = true;
    }
    
    static void onEncoderSW() {
        _pressed = true;
    }
    
public:
    // 初始化函数
    void begin() {
        // 配置引脚模式
        pinMode(ENC_A, INPUT);//低电平有效
        pinMode(ENC_B, INPUT);//低电平有效
        pinMode(ENC_SW, INPUT_PULLDOWN);//没有外部电阻，必须下拉，高电平有效
        
        // 注册中断
        attachInterrupt(digitalPinToInterrupt(ENC_A), onEncoderA, RISING);
        attachInterrupt(digitalPinToInterrupt(ENC_SW), onEncoderSW, FALLING);
        
        // 初始化标志
        _turned = false;
        _direction = 0;
        _pressed = false;
    }
    
    // 检查是否有转动事件
    bool turned() {
        return _turned;
    }
    
    // 获取转动方向（1=正转，-1=反转）
    int8_t getDirection() {
        return _direction;
    }
    
    // 检查按键是否按下
    bool pressed() {
        return _pressed;
    }
    
    // 清除转动标志
    void clearTurned() {
        _turned = false;
    }
    
    // 清除方向标志
    void clearDirection() {
        _direction = 0;
    }
    
    // 清除按键标志
    void clearPressed() {
        _pressed = false;
    }
    
    // 清除所有标志
    void clearAll() {
        _turned = false;
        _direction = 0;
        _pressed = false;
    }
};

// 静态成员初始化
volatile bool Encoder::_turned = false;
volatile int8_t Encoder::_direction = 0;
volatile bool Encoder::_pressed = false;

#endif // ENCODER_H