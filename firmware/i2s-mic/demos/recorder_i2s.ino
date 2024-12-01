#include <SPI.h>
#include "SdFat.h"
#include "MyI2S.h"
#include "wave.h"

SdFs sd;      // sd卡
FsFile file;  // 录音文件

MyI2S mi;
const int record_time = 60;  // second
const char filename[] = "/我的录音.wav";

const int waveDataSize = record_time * 88200;
int32_t communicationData[1024];     //接收缓冲区
char partWavData[1024];


void setup() {
  Serial.begin(115200);
  delay(500);

  // 初始化SD卡
  if(!sd.begin(SdSpiConfig(5, DEDICATED_SPI, 18000000)))
  {
    Serial.println("init sd card error");
    return;
  }

  //删除并创建文件
  sd.remove(filename);
  file = sd.open(filename, O_WRITE|O_CREAT);
  if(!file)
  {
    Serial.println("crate file error");
    return;
  }

  auto header = CreateWaveHeader(1, 44100, 16);
  header.riffSize = waveDataSize + 44 - 8;
  header.dataSize = waveDataSize;
  file.write(&header, 44);

  if(!mi.InitInput(I2S_BITS_PER_SAMPLE_32BIT, 17, 21, 4))
  {
    Serial.println("init i2s error");
    return;
  }

  Serial.println("start");

  for (int j = 0; j < waveDataSize/1024; ++j) {
    auto sz = mi.Read((char*)communicationData, 4096);
    char*p =(char*)(communicationData);
    for(int i=0;i<sz/4;i++)
    {
      communicationData[i] *= 20;  //提高声音
      if(i%2 == 0)   //这里获取到的数据第一个Int32是右声道
      {
          partWavData[i] = p[4*i + 2];
          partWavData[i + 1] = p[4*i + 3];
      }
    }
    file.write((const byte*)partWavData, 1024);
  }
  file.close();
  Serial.println("finish");
}

void loop() {
  // put your main code here, to run repeatedly:

}
