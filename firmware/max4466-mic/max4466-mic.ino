#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>


// 替换为你的Wi-Fi和WebSocket服务器信息
const char* ssid = "li";
const char* password = "12345678@@";
String ai_server = "192.168.31.100";
int ai_server_port = 8080;
WebSocketsClient webSocket;
bool isWebSocketConnected = false;
bool uploading = false;
unsigned long uploadStartTime = 0;

// 定义 I2S 配置
#define I2S_SAMPLE_RATE 8000  // 采样率
#define I2S_BUFFER_SIZE 1024   // 缓冲区大小
#define I2S_PORT I2S_NUM_0     // I2S 端口号
#define I2S_MIC_PIN 35 // 麦克风输入引脚



 
void setup() {
  Serial.begin(115200);

  // 连接到Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


// 连接到WebSocket服务器
  webSocket.begin(ai_server.c_str(), ai_server_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);


// 配置 I2S 引脚
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = I2S_BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  // 配置 I2S 引脚
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = I2S_PIN_NO_CHANGE,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_PIN
  };

  // 安装 I2S 驱动
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);

  // 启动 I2S
  i2s_start(I2S_PORT);

}

void loop() {
  webSocket.loop();
 
  if (isWebSocketConnected && uploading) {
      int32_t samples[I2S_BUFFER_SIZE];
      size_t bytes_read;

      // 从 I2S 读取数据
      i2s_read(I2S_PORT, samples, sizeof(samples), &bytes_read, portMAX_DELAY);
    // 发送音频数据
      webSocket.sendBIN((uint8_t*)samples, bytes_read);
    // 检查是否已经上传了20秒
    if (millis() - uploadStartTime >= 10000) {
      webSocket.sendTXT("-1"); // 发送文本“-1”
      uploading = false;
    }
  }
} 

// WebSocket事件处理
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("Connected to WebSocket\n");
      isWebSocketConnected = true;
      webSocket.sendTXT("1"); // 发送文本“1”
      uploadStartTime = millis();
      uploading = true;
      break;
    case WStype_DISCONNECTED:
      Serial.printf("Disconnected from WebSocket\n");
      isWebSocketConnected = false;
      uploading = false;
      break;
    case WStype_ERROR:
      Serial.printf("WebSocket error\n");
      isWebSocketConnected = false;
      uploading = false;
      break;
    case WStype_TEXT:
      // 处理收到的文本消息
      break;
    case WStype_BIN:
      // 处理收到的二进制消息
      break;
  }
}

 

