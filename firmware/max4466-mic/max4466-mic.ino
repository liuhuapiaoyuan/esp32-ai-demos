#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/adc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi 配置
const char* ssid = "li";       // 替换为你的 WiFi SSID
const char* password = "12345678@@"; // 替换为你的 WiFi 密码

// WebSocket 服务器配置
const char* wsServer = "192.168.31.98"; // 替换为 WebSocket 服务器的 IP 地址
const int wsPort = 8080;                     // WebSocket 服务器端口
const char* wsPath = "/";                  // WebSocket 服务器路径

WebSocketsClient webSocket;

// 采样配置
#define SAMPLE_RATE 8000  // 目标采样率：8 kHz
#define SAMPLE_INTERVAL (1000000 / SAMPLE_RATE)  // 采样间隔（微秒）

// 缓冲区配置
#define BUFFER_SIZE 2000   // 缓冲区大小：8 kHz

volatile bool sampleReady = false;
uint16_t adcBuffer[BUFFER_SIZE];  // 存储 8 kHz 的采样数据
int sampleIndex = 0;

// 录制时间配置
#define RECORD_TIME 10  // 录制时间（秒）
unsigned long startTime = 0;

// 定时器线程
void timerThread(void* arg) {
  while (1) {
    if (webSocket.isConnected()) {  // 只有在 WebSocket 连接时才采集数据
      sampleReady = true;
    }
    vTaskDelay(SAMPLE_INTERVAL / 1000 / portTICK_PERIOD_MS);  // 延迟采样间隔
  }
}

// WebSocket 事件处理
void onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WebSocket connected to server");
      break;
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected from server");
      break;
    case WStype_TEXT:
      Serial.printf("Received text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("Received binary data, length: %u\n", length);
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // 配置 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // 配置 WebSocket 客户端
  webSocket.begin(wsServer, wsPort, wsPath);
  webSocket.onEvent(onWebSocketEvent);

  // 配置 ADC
  adc1_config_width(ADC_WIDTH_BIT_12);  // 设置 ADC 分辨率为 12 位
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_12);  // 替换为 ADC_ATTEN_DB_12

  // 创建定时器线程
  xTaskCreatePinnedToCore(
    timerThread,    // 任务函数
    "TimerThread",  // 任务名称
    4096,           // 堆栈大小
    NULL,           // 任务参数
    1,              // 任务优先级
    NULL,           // 任务句柄
    1);             // 核心 ID

  // 记录开始时间
  startTime = millis();
}

void loop() {
  // 处理 WebSocket 事件
  webSocket.loop();

  if (sampleReady && webSocket.isConnected()) {  // 只有在 WebSocket 连接时才处理数据
    sampleReady = false;

    // 读取 ADC 值
    adcBuffer[sampleIndex] = adc1_get_raw(ADC1_CHANNEL_7);  // 使用 adc1_get_raw 读取 ADC 值
    sampleIndex++;

    // 如果缓冲区满 8 kHz 的数据，发送数据到 WebSocket 服务器
    if (sampleIndex >= BUFFER_SIZE) {
      // 将 16 位数据转换为 8 位数据（PCM 格式）
      uint8_t pcmBuffer[BUFFER_SIZE * 2];
      for (int i = 0; i < BUFFER_SIZE; i++) {
        pcmBuffer[i * 2] = adcBuffer[i] & 0xFF;       // 低字节
        pcmBuffer[i * 2 + 1] = (adcBuffer[i] >> 8) & 0xFF; // 高字节
      }

      // 通过 WebSocket 发送二进制数据
      webSocket.sendBIN(pcmBuffer, BUFFER_SIZE * 2);

      sampleIndex = 0;

      // 检查录制时间是否超过 10 秒
      if (millis() - startTime >= RECORD_TIME * 1000) {
        // 发送 -1 表示录制结束
        String endSignal = "-1";  // 将 -1 转换为 String 类型
        webSocket.sendTXT(endSignal);  // 传递 String 变量
        Serial.println("Recording finished, sent -1 to server.");

        // 重置录制时间
        startTime = millis();
      }
    }
  }
}