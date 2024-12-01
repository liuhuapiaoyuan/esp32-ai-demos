#ifndef RECORDER_H
#define RECORDER_H

#include <Arduino.h>
#include <functional>
#include "driver/adc.h"
class Recorder {
public:
    typedef std::function<void(const uint8_t*, size_t)> RecordCallback;

    Recorder(uint8_t pin = 35);
    void begin();
    void start();
    void stop();
    void loop();
    void onRecord(RecordCallback callback);

private:
    static const int ADC_BUFFER_SIZE = 512;  // 采样缓冲区大小
    static const int SAMPLE_RATE = 16000;    // 采样率 16KHz
    static const int DC_FILTER_SIZE = 64;    // DC偏置滤波器大小

    struct ADCSample {
        int16_t value;
        unsigned long timestamp;
    };
    uint8_t _pin;
    bool _isRecording;
    RecordCallback _callback;
    
    // 采样相关变量
    ADCSample _adcBuffer[ADC_BUFFER_SIZE];
    int _adcBufferIndex;
    unsigned long _lastSampleTime;
    uint16_t _lastRawValue;

    // DC偏置相关
    int32_t _dcSum;
    int32_t _dcOffset;
    int _dcCount;

    // 调试统计
    int16_t _maxValue;
    int16_t _minValue;
    uint32_t _totalSamples;
    unsigned long _lastDebugTime;

    void initADC();
    void updateDCOffset(uint16_t value);
    int16_t convertToAudio(uint16_t adcValue);
    void processSample();
    void sendBuffer();
};

#endif // RECORDER_H
