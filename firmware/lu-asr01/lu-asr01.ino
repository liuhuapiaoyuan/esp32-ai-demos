#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
 

 
#define I2S_DOUT      27  // connect to DAC pin DIN
#define I2S_BCLK      26  // connect to DAC pin BCK
#define I2S_LRC       25  // connect to DAC pin LCK

#define LED_ONBOARD 32
const long interval = 1000;
int ledState = LOW; 
unsigned long previousMillis = 0; 

Audio audio;

String ssid =     "shixin-soft-com";
String password = "123456qq";
#define BUFFER_SIZE 64  // 缓冲区大小
char buffer[BUFFER_SIZE];  // 用于存储接收到的数据的数组
int bufferIndex = 0;  // 当前缓冲区中的位置

void setup() {
    
    // pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
    // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    // SD.begin(SD_CS);
    Serial.begin(115200);

    pinMode(LED_ONBOARD,OUTPUT);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // default 0...21
    audio.forceMono(true);

    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

}

void loop()
{

  while (Serial.available() > 0) {
    char receivedChar = Serial.read();  // 读取一个字符 

    if (receivedChar == '\n') {  // 如果接收到换行符，表示消息结束
      buffer[bufferIndex] = '\0';  // 在缓冲区末尾添加字符串终止符
      Serial.print("Received: ");  // 打印接收到的消息
      Serial.println(buffer);

      // 模拟处理消息完成后的行为，例如点亮LED
      digitalWrite(LED_ONBOARD, HIGH);
      //  *** web files ***
     audio.connecttohost("https://cdn.kedao.ggss.club/demo.mp3");

      delay(500);
      digitalWrite(LED_ONBOARD, LOW);

      // 清空缓冲区，准备接收下一条消息
      bufferIndex = 0;
    } else if (bufferIndex < BUFFER_SIZE - 1) {  // 防止缓冲区溢出
      buffer[bufferIndex] = receivedChar;
      bufferIndex++;
    }
  }
  audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}