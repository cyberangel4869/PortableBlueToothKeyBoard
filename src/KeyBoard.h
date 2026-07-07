#ifndef KEY_H
#define KEY_H

#include <Arduino.h>
#include <map>
#include <esp_sleep.h>

class Key {
private:
    // 单例实例指针
    static Key* _instance;
    
    // 反向查找：IO引脚号 -> 按键编号
    const std::map<uint8_t, uint8_t> pinToKeyId = {
        {47, 1}, {9, 2}, {15, 3}, {21, 4},
        {10, 5}, {16, 6}, {14, 7}, {11, 8}, {17, 9}
    };
    
    // 中断共享变量
    volatile bool _pressed;//按键按下标志位
    volatile uint8_t _pendingPin;//按下按键的IO编号
    
    // 静态中断回调函数
    static void onKeyPressHandler(void* arg) {
        if (_instance) {
            //修改当前类的按键IO号
            _instance->_pendingPin = (uint8_t)(uintptr_t)arg;
            //修改标志位
            _instance->_pressed = true;
        }
    }
    
public:
    // 构造函数
    Key() : _pressed(false), _pendingPin(0) {
        _instance = this;//指向当前定义的Key类
    }
    
    // 初始化函数
    //正常模式
    void beginNormal() {
        for (auto it = pinToKeyId.begin(); it != pinToKeyId.end(); ++it) {//遍历按键编码和IO编码
            uint8_t pin = it->first;
            pinMode(pin, INPUT);//外部上拉
            attachInterruptArg(
                digitalPinToInterrupt(pin),//绑定IO号对应的中断
                onKeyPressHandler,//调用的中断处理函数
                (void*)(uintptr_t)pin,//将IO编号作为函数指针回传
                RISING//上升沿触发
            );
        }
    }
    //低功耗休眠模式
    void beginDeepSleep(){
        uint64_t IOmask=0;
        for(auto it = pinToKeyId.begin();it!=pinToKeyId.end();++it){
            uint8_t pin = it->first;
            pinMode(pin,INPUT);
            if(pin<=21){
                IOmask=IOmask|1ULL<<pin;
            }
        }
        if(IOmask!=0){
            esp_sleep_enable_ext1_wakeup(IOmask,ESP_EXT1_WAKEUP_ANY_LOW);
            esp_deep_sleep_start();
        }
    }
    
    // 检查是否有按键按下
    bool pressed() {
        return _pressed;
    }

    uint8_t getIoNum(){
        if(!_pressed) return 0;
        uint8_t pendingPin = _pendingPin;
        return pendingPin;
    }
    
    // 获取按键编号（在这里完成IO号到按键编号的查找）
    uint8_t getKeyNum() {
        if (!_pressed) return 0;
        uint8_t pendingPin=_pendingPin;
        // 耗时操作：查找键值对
        auto it = pinToKeyId.find(pendingPin);
        if (it != pinToKeyId.end()) {
            return it->second;
        }
        return 0;
    }
    
    // 清除标志
    void clear() {
        _pressed = false;
        _pendingPin = 0;
    }
};

// 静态成员初始化
Key* Key::_instance = nullptr;

#endif