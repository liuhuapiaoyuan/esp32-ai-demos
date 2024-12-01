#include <WiFi.h>
#include "../../src/LLM/LLM.h"

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_Password";

// OpenAI API credentials
const char* apiKey = "your-api-key";
const char* apiEndpoint = "https://api.openai.com/v1/chat/completions"; // 可选，默认为OpenAI endpoint

LLM llm;

// 回调函数，用于处理流式文本输出
void onTextChunk(const String& text) {
    Serial.print(text);
}

void setup() {
    Serial.begin(115200);
    
    // 连接WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    
    // 初始化LLM
    llm.begin(apiKey, apiEndpoint);
    llm.setTextChunkCallback(onTextChunk);
    
    // 发送测试问题
    Serial.println("\n发送问题到LLM...");
    String response = llm.chatCompletion("你好，请介绍一下你自己。");
    
    // 检查是否有错误
    if (llm.getLastError() != "") {
        Serial.println("错误: " + llm.getLastError());
    }
}

void loop() {
    // 在这里可以添加更多的交互逻辑
    delay(1000);
}
