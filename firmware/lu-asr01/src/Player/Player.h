#ifndef PALYER_H
#define PALYER_H

#include <Arduino.h>
#include "driver/i2s.h"

#define BUFFER_SIZE 1024  // 音频缓冲区大小
#define SAMPLE_RATE 44100 // 采样率
#define DMA_BUFF_COUNT 8  // DMA缓冲区数量
#define DMA_BUFF_LEN 1024 // DMA缓冲区长度
#ifndef I2S_GPIO_UNUSED
  #define I2S_GPIO_UNUSED -1 // = I2S_PIN_NO_CHANGE in IDF < 5
#endif
class Player {
public:
    // 修改构造函数参数顺序以匹配主程序定义：
    // I2S_BCLK (BCK), I2S_LRC (LCK), I2S_DOUT (DIN)
    Player(int bclkPin, int lrcPin, int doutPin);
    ~Player();

    // 初始化I2S
    bool begin();
    // 写入音频数据
    size_t writeData(const int16_t* data, size_t length);
    // 停止播放
    void stop();
    // 设置音量 (0-100)
    void setVolume(uint8_t vol);
    // 获取当前缓冲区中的数据量
    size_t available();
    // 清空缓冲区
    void clear();

private:
    // I2S配置
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;
    
    // GPIO引脚
    int _bclkPin;
    int _wsPin;
    int _doutPin;
    
    // 音量控制 (0-100)
    uint8_t _volume;
    
    // 音频缓冲区
    int16_t* _audioBuffer;
    size_t _bufferSize;
    size_t _writeIndex;
    size_t _readIndex;
    
    // 内部方法
    inline int16_t applyVolume(int16_t sample);
};

#endif // PALYER_H