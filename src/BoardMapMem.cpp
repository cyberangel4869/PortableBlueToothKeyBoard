#include "BoardMapMem.h"

// 代码中的初始按键映射（回退用）
static const KeyMap DefaultKeyCodes[10] = {
    {0x00, 0},          // 无按键
    {0x4A, 0},          // KEY_HOME → 0x4A
    {0x4B, 0},          // KEY_PAGE_UP → 0x4B
    {0x4E, 0},          // KEY_PAGE_DOWN → 0x4E
    {0x4C, 0},          // KEY_DELETE → 0x4C
    {0x52, 0},          // KEY_UP_ARROW → 0x52
    {0x28, 0},          // KEY_RETURN → 0x28
    {0x50, 0},          // KEY_LEFT_ARROW → 0x50
    {0x51, 0},          // KEY_DOWN_ARROW → 0x51
    {0x4F, 0}           // KEY_RIGHT_ARROW → 0x4F
};

const char* BoardMapMem::NAMESPACE = "boardmap";

BoardMapMem::BoardMapMem() {
    buildDefault();
    currentMap = defaultMap;
}

void BoardMapMem::begin() {
    Load(0);   // 失败会自动 fallback
}

void BoardMapMem::buildDefault() {
    for (int i = 0; i < 10; i++) {
        defaultMap.KeyCodes[i] = DefaultKeyCodes[i];
    }
    defaultMap.EncoderCode.ClockWise     = 0;
    defaultMap.EncoderCode.AntiClockWise = 0;
    defaultMap.ScreenBrit = 200;
    defaultMap.RGBbrit    = 200;
    defaultMap.R = 0x66;
    defaultMap.G = 0xcc;
    defaultMap.B = 0xff;
    strncpy(defaultMap.Name,"Default",sizeof(defaultMap.Name));
}

void BoardMapMem::makeKey(uint8_t index, char* key, size_t keyLen) const {
    snprintf(key, keyLen, "m%d", index);
}

bool BoardMapMem::Load(uint8_t index) {
    if (index >= MAP_COUNT) {
        fallback();
        return false;
    }

    char key[8];
    makeKey(index, key, sizeof(key));

    prefs.begin(NAMESPACE, true);
    if (!prefs.isKey(key)) {
        prefs.end();
        fallback();
        return false;
    }
    size_t len = prefs.getBytes(key, &currentMap, sizeof(BoardMap));
    prefs.end();

    if (len != sizeof(BoardMap)) {
        fallback();
        return false;
    }
    return true;
}

bool BoardMapMem::Save(uint8_t index) {
    if (index >= MAP_COUNT) return false;

    char key[8];
    makeKey(index, key, sizeof(key));

    prefs.begin(NAMESPACE, false);
    size_t written = prefs.putBytes(key, &currentMap, sizeof(BoardMap));
    prefs.end();
    return (written == sizeof(BoardMap));
}

const BoardMap* BoardMapMem::Get() const {
    return &currentMap;
}

void BoardMapMem::Set(const BoardMap* map) {
    if (map == nullptr) return;
    currentMap = *map;
}

uint8_t BoardMapMem::GetScreenBrightness() {
    return currentMap.ScreenBrit;
}

uint8_t BoardMapMem::GetRGBBrightness(){
    return currentMap.RGBbrit;
}

uint8_t BoardMapMem::GetR(){
    return currentMap.R;
}

uint8_t BoardMapMem::GetG(){
    return currentMap.G;
}

uint8_t BoardMapMem::GetB(){
    return currentMap.B;
}

char* BoardMapMem::GetName(){
    return currentMap.Name;
}

void BoardMapMem::fallback() {
    currentMap = defaultMap;
}

void BoardMapMem::SetScreenBrightness(uint8_t value) {
    currentMap.ScreenBrit = value;
}

void BoardMapMem::SetRGBBrightness(uint8_t value) {
    currentMap.RGBbrit = value;
}

void BoardMapMem::SetR(uint8_t value) {
    currentMap.R = value;
}

void BoardMapMem::SetG(uint8_t value) {
    currentMap.G = value;
}

void BoardMapMem::SetB(uint8_t value) {
    currentMap.B = value;
}

void BoardMapMem::SetRGB(uint8_t r, uint8_t g, uint8_t b) {
    currentMap.R = r;
    currentMap.G = g;
    currentMap.B = b;
}

void BoardMapMem::SetName(const char* name) {
    if (name == nullptr) return;
    strncpy(currentMap.Name, name, sizeof(currentMap.Name));
}