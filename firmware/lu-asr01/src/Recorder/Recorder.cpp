#include "Recorder.h"

Recorder::Recorder(uint8_t pin) 
    : _pin(pin), _isRecording(false), _adcBufferIndex(0), _lastSampleTime(0),
      _lastRawValue(0), _dcSum(0), _dcOffset(0), _dcCount(0),
      _maxValue(-32768), _minValue(32767), _totalSamples(0), _lastDebugTime(0) {
}

void Recorder::begin() {
    analogReadResolution(10); // 设置ADC分辨率
  analogSetPinAttenuation(_pin, ADC_11db);
    // initADC();
}

void Recorder::initADC() {
    analogSetWidth(12);              // 设置ADC分辨率为12位
    analogSetAttenuation(ADC_2_5db); // 使用2.5dB衰减，提高灵敏度
    analogSetClockDiv(1);            // 设置ADC时钟分频
    
    // 计算初始DC偏置
    int32_t sum = 0;
    for(int i = 0; i < 100; i++) {
        sum += analogRead(_pin);
        delay(1);
    }
    _dcOffset = sum / 100;
    Serial.printf("Initial DC offset: %d\n", _dcOffset);
}

void Recorder::start() {
    _isRecording = true;
    _adcBufferIndex = 0;
    _lastSampleTime = micros();
    Serial.println("Recording started");
}

void Recorder::stop() {
    _isRecording = false;
    Serial.println("Recording stopped");
}

void Recorder::onRecord(RecordCallback callback) {
    _callback = callback;
}

void Recorder::updateDCOffset(uint16_t value) {
    _dcSum += value;
    _dcCount++;
    
    if (_dcCount >= DC_FILTER_SIZE) {
        _dcOffset = _dcSum / DC_FILTER_SIZE;
        _dcSum = 0;
        _dcCount = 0;
    }
}

int16_t Recorder::convertToAudio(uint16_t adcValue) {
    // 移除DC偏置
    int32_t centered = adcValue - _dcOffset;
    
    // 使用较大增益
    int32_t scaled = centered * 64;
    
    // 限制在16位范围内
    if (scaled > 32767) scaled = 32767;
    if (scaled < -32768) scaled = -32768;
    
    return (int16_t)scaled;
}

void Recorder::processSample() {
    if (!_isRecording) return;

    unsigned long currentTime = micros();
    const unsigned long sampleInterval = 1000000 / SAMPLE_RATE;
    
    if (currentTime - _lastSampleTime >= sampleInterval) {
        if (_adcBufferIndex < ADC_BUFFER_SIZE) {
            // 读取ADC
            uint16_t rawValue = analogRead(_pin);
            
            // 更新DC偏置
            updateDCOffset(rawValue);
            
            // 简单的一阶差分，强调变化
            int16_t diff = rawValue - _lastRawValue;
            _lastRawValue = rawValue;
            
            // 转换为音频样本
            int16_t audioValue = convertToAudio(abs(diff) > 5 ? rawValue : _dcOffset);
            
            // 更新统计
            if (audioValue > _maxValue) _maxValue = audioValue;
            if (audioValue < _minValue) _minValue = audioValue;
            
            _adcBuffer[_adcBufferIndex].value = audioValue;
            _adcBuffer[_adcBufferIndex].timestamp = currentTime;
            _adcBufferIndex++;
            _totalSamples++;
            _lastSampleTime += sampleInterval;
        }

        if (_adcBufferIndex >= ADC_BUFFER_SIZE) {
            sendBuffer();
        }
    }
}

void Recorder::sendBuffer() {
    if (!_callback) return;

    uint8_t sendBuffer[ADC_BUFFER_SIZE * 2];
    float rms = 0;
    int nonZeroCount = 0;
    
    for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
        sendBuffer[i * 2] = highByte(_adcBuffer[i].value);
        sendBuffer[i * 2 + 1] = lowByte(_adcBuffer[i].value);
        rms += (float)_adcBuffer[i].value * _adcBuffer[i].value;
        if (_adcBuffer[i].value != 0) nonZeroCount++;
    }
    rms = sqrt(rms / ADC_BUFFER_SIZE);
    
    // 调用回调函数发送数据
    _callback(sendBuffer, ADC_BUFFER_SIZE * 2);
    
    // 输出调试信息
    if (millis() - _lastDebugTime >= 1000) {
        Serial.printf("Audio Stats - DC: %d, Min: %d, Max: %d, Range: %d, RMS: %.2f, Active: %d%%\n", 
            _dcOffset, _minValue, _maxValue, _maxValue - _minValue, rms, 
            (nonZeroCount * 100) / ADC_BUFFER_SIZE);
        _lastDebugTime = millis();
        _maxValue = -32768;
        _minValue = 32767;
    }
    
    _adcBufferIndex = 0;
}

void Recorder::loop() {
        if (!_isRecording) return;

    if (!_callback) return;
    uint8_t sampleArray[1024];
     // 采集音频数据
  for (int i = 0; i < 1024; i++) {
    sampleArray[i] = analogRead(_pin);
    Serial.print(sampleArray[i]);
    Serial.print(" ");
    delayMicroseconds(1000000 / 1024);
  }
    _callback(sampleArray, 1024 * 2);
}
