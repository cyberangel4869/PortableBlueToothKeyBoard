#ifndef BOARD_MAP_MEM_H
#define BOARD_MAP_MEM_H

#include <Arduino.h>
#include <Preferences.h>

struct KeyMap{
    uint8_t KeyCode;
    uint8_t FuctCode;
};

struct EncoderMap{
    uint8_t ClockWise;
    uint8_t AntiClockWise;
};

struct BoardMap{
    uint8_t ScreenBrit;
    uint8_t RGBbrit;
    uint8_t R;
    uint8_t G;
    uint8_t B;
    KeyMap KeyCodes[10];
    EncoderMap EncoderCode;
    char Name[20];
};

class BoardMapMem {
public:
    BoardMapMem();

    // 初始化：加载 MODE0，失败则回退默认
    void begin();

    // 读取/保存指定模式的配置，失败返回 false 并回退默认
    bool Load(uint8_t index);
    bool Save(uint8_t index);

    // 只读获取整个配置
    const BoardMap* Get() const;

    // 手动设置当前配置（供 WebUI 等使用）
    void Set(const BoardMap* map);

    // 以下接口仅读取参数，硬件设置由调用方完成
    uint8_t GetScreenBrightness();   // 屏幕亮度
    uint8_t GetRGBBrightness();      // RGB 亮度
    uint8_t GetR();                  // 红色分量
    uint8_t GetG();                  // 绿色分量
    uint8_t GetB();                  // 蓝色分量
    char* GetName();                  // 自定义名称

private:
    static const uint8_t MAP_COUNT = 5;
    static const char* NAMESPACE;

    Preferences prefs;
    BoardMap currentMap;
    BoardMap defaultMap;

    void makeKey(uint8_t index, char* key, size_t keyLen) const;
    void buildDefault();
    void fallback();
};

#endif