#include "Player.h"

Player::Player(int bclkPin, int lrcPin, int doutPin)
    : _bclkPin(bclkPin), _wsPin(lrcPin), _doutPin(doutPin), _volume(100) {
    _bufferSize = BUFFER_SIZE;
    _audioBuffer = new int16_t[_bufferSize];
    _writeIndex = 0;
    _readIndex = 0;
}

Player::~Player() {
    stop();
    delete[] _audioBuffer;
}

bool Player::begin() {
    // 配置I2S
    i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = DMA_BUFF_COUNT,
        .dma_buf_len = DMA_BUFF_LEN,
        .use_apll = true,  // 使用APLL以获得更好的音质
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    pin_config = {
        .bck_io_num = _bclkPin,
        .ws_io_num = _wsPin,
        .data_out_num = _doutPin,
        .data_in_num = I2S_GPIO_UNUSED
    };

    // 安装并启动I2S驱动
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.println("Failed to install i2s driver");
        return false;
    }

    err = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (err != ESP_OK) {
        Serial.println("Failed to set i2s pins");
        return false;
    }

    // 启动I2S
    err = i2s_start(I2S_NUM_0);
    if (err != ESP_OK) {
        Serial.println("Failed to start i2s");
        return false;
    }

    return true;
}

size_t Player::writeData(const int16_t* data, size_t length) {
    size_t written = 0;
    size_t space = _bufferSize - (_writeIndex - _readIndex);
    
    // 等待空间可用
    while (space == 0) {
        delay(3);
        space = _bufferSize - (_writeIndex - _readIndex);
    }
    
    // 计算实际可写入的数据量
    size_t toWrite = min(length, space);
    
    // 写入数据
    for (size_t i = 0; i < toWrite; i++) {
        size_t idx = _writeIndex % _bufferSize;
        _audioBuffer[idx] = applyVolume(data[i]);
        _writeIndex++;
        written++;
    }

    // 发送数据到I2S
    size_t bytesWritten = 0;
    while (_readIndex < _writeIndex) {
        size_t idx = _readIndex % _bufferSize;
        int16_t sample = _audioBuffer[idx];
        int16_t stereoData[2] = {sample, sample}; // 左右声道相同
        
        size_t bytes = 0;
        esp_err_t err = i2s_write(I2S_NUM_0, stereoData, sizeof(stereoData), &bytes, 100 / portTICK_PERIOD_MS);
        
        if (err != ESP_OK && err != ESP_ERR_TIMEOUT) {
            Serial.printf("I2S write error: %d\n", err);
        }
        
        if (bytes > 0) {
            _readIndex++;
            bytesWritten += bytes;
        } else {
            delay(1); // 如果写入失败，等待一小段时间
        }
    }

    return written;
}

void Player::stop() {
    i2s_stop(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_0);
}

void Player::setVolume(uint8_t vol) {
    _volume = min((uint8_t)100, vol);
}

int16_t Player::applyVolume(int16_t sample) {
    return (sample * _volume) / 100;
}

size_t Player::available() {
    return _writeIndex - _readIndex;
}

void Player::clear() {
    _writeIndex = 0;
    _readIndex = 0;
}