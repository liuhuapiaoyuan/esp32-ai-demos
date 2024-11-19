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


//  *** web files ***
 audio.connecttohost("https://cdn.kedao.ggss.club/demo.mp3");

}

void loop()
{

   unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(LED_ONBOARD, ledState);
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