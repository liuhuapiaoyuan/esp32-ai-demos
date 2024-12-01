#include "WiFi.h"
#include <WebSocketsClient.h> 
#include <driver/i2s.h>
#include <queue>
#include <string>
#include "Audio.h"


#define I2S_DOUT      25  // connect to DAC pin DIN
#define I2S_BCLK      26  // connect to DAC pin BCK
#define I2S_LRC       27  // connect to DAC pin LCK
#define ASR_RX        13  // D13用于l
#define LED_ONBOARD   23  // 状态灯

#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define MIC_i2s_num I2S_NUM_1
#define MIC_I2S_BCK  12    //SCK
#define MIC_I2S_WS   14   //WS
#define MIC_I2S_DIN   32  //SD

// Global queue to store TTS prompts
std::queue<String> ttsPromptQueue;
const int MAX_QUEUE_SIZE = 15;  // Maximum number of prompts to store

 

const long interval = 1000; 
unsigned long previousMillis = 0; 
 
String ssid =     "li";
String password = "12345678@@";
String ai_server = "192.168.31.100";
int ai_server_port = 8080;
int status = 0;
WebSocketsClient websocket; 


String urlEncode(const String &str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      // ASCII 字符直接添加
      encoded += c;
    } else {
      // 非ASCII字符先转成UTF-8
      char buffer[5] = {0};
      sprintf(buffer, "%%%02X", (unsigned char)c);
      encoded += buffer;
    }
  }
  return encoded;
}
 
// WebSocket事件处理
void onMessage(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n"); 
            digitalWrite(LED_ONBOARD, LOW);
            status = 0;
            break;
        case WStype_ERROR:
            Serial.printf("[WSc] Connect error!\n");
            status = 0;
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            break;
        case WStype_TEXT:
            
            if (strncmp((char *)payload, "LLM:", 4) == 0) {
              String text = String((char *)payload + 4);  
              String prompt = "http://" + ai_server  + ":" + String(ai_server_port) + "/tts.mp3?text=" + urlEncode(text);
              // Add prompt to queue if not full
              if (ttsPromptQueue.size() < MAX_QUEUE_SIZE) {
                  ttsPromptQueue.push(prompt);
                  Serial.println("Added prompt to queue. Queue size: " + String(ttsPromptQueue.size()));
              } else {
                  Serial.println("Queue is full, dropping prompt");
              } 
            }
            if (strncmp((char *)payload, "ASR:", 4) == 0) {
                String text = String((char *)payload + 4);  
                if(text=="START"){
                    status = 1;
                }
                if(text=="STOP"){
                    status = 0;
                }
            }
            break;
        default:
            break;
    }
}

Audio audio; 

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, ASR_RX);
    pinMode(LED_ONBOARD, OUTPUT);
    
    // 连接WiFi
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);
    
    // 配置WebSocket
    websocket.begin(ai_server.c_str(), ai_server_port, "/");
    websocket.onEvent(onMessage);
    websocket.setReconnectInterval(2000);
    
    // 初始化麦克风
    if (mic_i2s_init(16000))
    {
        Serial.println(("[mic Error] Failed to start I2S!"));
    }else{
       Serial.println("mic succewss");
    }

      audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // default 0...21
    audio.forceMono(true);


  //  *** web files ***
  audio.connecttohost("https://cdn.kedao.ggss.club/wx/hello.mp3");
   
}

void loop() {
    websocket.loop(); 
    
    // 串口命令处理
    if (Serial2.available()) {
        char receivedChar = Serial2.read();
        if (receivedChar == '\n') {
            digitalWrite(LED_ONBOARD, HIGH);
            websocket.sendTXT(status ==0 ? "1": "-1"); 
        }
    }
    if( status ==1){
        record_loop();
    }

    audio.loop();
    
    if (!audio.isRunning() && !ttsPromptQueue.empty()) {
      String prompt = ttsPromptQueue.front();
      ttsPromptQueue.pop();
      audio.connecttohost(prompt.c_str());
      //Serial.println("Playing next prompt from queue. Remaining: " + String(ttsPromptQueue.size()));
  }
}

int16_t diy_wakeup_sample_buffer[2048];
void record_loop() {
    size_t diy_wakeup_bytes_read;
    i2s_read(MIC_i2s_num, (void *)diy_wakeup_sample_buffer, sizeof(diy_wakeup_sample_buffer), &diy_wakeup_bytes_read, portMAX_DELAY);
    
    websocket.sendBIN((uint8_t *)diy_wakeup_sample_buffer, diy_wakeup_bytes_read);
    delay(3);
}


bool mic_i2s_init(uint32_t sampling_rate)
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = sampling_rate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_MIC_CHANNEL,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    i2s_pin_config_t i2s_mic_pins = {
        .bck_io_num = MIC_I2S_BCK,
        .ws_io_num = MIC_I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_I2S_DIN,
    };

    esp_err_t ret = 0;
    ret = i2s_driver_install(MIC_i2s_num, &i2s_config, 0, NULL);

    if (ret != ESP_OK)
    {
        Serial.println("[Error] Error in i2s_driver_install");
    }
    ret = i2s_set_pin(MIC_i2s_num, &i2s_mic_pins);

    if (ret != ESP_OK)
    {
        Serial.println("[Error] Error in i2s_set_pin");
    }

    ret = i2s_zero_dma_buffer(MIC_i2s_num);
    if (ret != ESP_OK)
    {
        Serial.println("[Error] Error in initializing dma buffer with 0");
    }

     
    return int(ret);
}

 