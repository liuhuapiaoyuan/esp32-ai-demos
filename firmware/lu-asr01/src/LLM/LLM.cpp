#include "LLM.h"

// 定义标点符号列表
const char* LLM::DEFAULT_BREAK_POINTS[] = {
    "。", // 句号
    "，", // 逗号
    "！", // 感叹号
    "？", // 问号
    ".", // 英文句号
    ",", // 英文逗号
    "；", // 分号
    ";", // 英文分号
    "：", // 冒号
    ":", // 英文冒号
    "…", // 省略号
    "—"  // 破折号
};

// 定义数组大小
const int LLM::BREAK_POINTS_COUNT = sizeof(DEFAULT_BREAK_POINTS) / sizeof(DEFAULT_BREAK_POINTS[0]);

LLM::LLM() {
    _lastError = "";
    _textBuffer = "";
    _onTextChunk = nullptr;
    _apiEndpoint = "https://api.openai.com/v1/chat/completions";
}

void LLM::begin(const char* apiKey, const char* apiEndpoint) {
    _apiKey = String(apiKey);
    if (apiEndpoint != nullptr && strlen(apiEndpoint) > 0) {
        _apiEndpoint = String(apiEndpoint);
    }
}

String LLM::chatCompletion(const String& prompt) {
    return makeRequest(prompt);
}

String LLM::getLastError() const {
    return _lastError;
}

String LLM::makeRequest(const String& prompt) {
    if (_apiKey.isEmpty()) {
        handleError("API key not set");
        return "";
    }

    // 准备JSON请求体
    _jsonDoc.clear();
    _jsonDoc["model"] = "llama-3.1-70b-versatile";
    _jsonDoc["stream"] = true;  // 启用流式输出
    JsonArray messages = _jsonDoc.createNestedArray("messages");
    // JsonObject system = messages.createNestedObject();
    // system["role"] = "system";
    // system["content"] = "你的名字是小P，现在是一个对话场景，注意使用口语的方式回复";
    JsonObject message = messages.createNestedObject();
    message["role"] = "user";
    message["content"] = prompt;

    String jsonString;
    serializeJson(_jsonDoc, jsonString);

    // 设置HTTP请求
    _http.begin(_apiEndpoint);
    _http.addHeader("Content-Type", "application/json");
    _http.addHeader("Authorization", "Bearer " + _apiKey);
    _http.addHeader("Accept", "text/event-stream");

    // 发送请求
    int httpCode = _http.POST(jsonString);

    if (httpCode != HTTP_CODE_OK) {
        String error = "HTTP error: " + String(httpCode);
        handleError(error);
        _http.end();
        return "";
    }

    // 读取SSE流
    WiFiClient* stream = _http.getStreamPtr();
    String completeResponse;
    
    while (_http.connected()) {
        if (stream->available()) {
            String line = stream->readStringUntil('\n');
            if (line.startsWith("data: ")) {
                String data = line.substring(6); // 去掉 "data: " 前缀
                if (data == "[DONE]") {
                    // 处理剩余的文本
                    if (!_textBuffer.isEmpty() && _onTextChunk) {
                        _onTextChunk(_textBuffer);
                        _textBuffer = "";
                    }
                    break;
                }
                processSSEChunk(data);
            }
        }
    }

    _http.end();
    return completeResponse;
}

void LLM::processSSEChunk(const String& chunk) {
    _jsonDoc.clear();
    DeserializationError error = deserializeJson(_jsonDoc, chunk);
    
    if (error) {
        handleError("JSON parse error in chunk");
        return;
    }
    JsonVariantConst choices = _jsonDoc["choices"];

    // 提取文本内容
    if (choices!=nullptr && _jsonDoc["choices"].size() > 0) {
        const char* content = _jsonDoc["choices"][0]["delta"]["content"];
        if (content != nullptr) {
            String textChunk = String(content);
            _textBuffer += textChunk;

            // 查找最后一个分隔符的位置
            int lastDelimiterIndex = -1;
            for (int i = 0; i < BREAK_POINTS_COUNT; i++) {
                int index = _textBuffer.lastIndexOf(DEFAULT_BREAK_POINTS[i]);
                if (index > lastDelimiterIndex) {
                    lastDelimiterIndex = index;
                }
            }

            // 如果找到分隔符或缓冲区太长，则发送内容
            if ((lastDelimiterIndex != -1 && _textBuffer.length() < 15) || 
                _textBuffer.length() >= 32) {  // 设置最大缓冲区大小
                if (_onTextChunk) {
                    if (lastDelimiterIndex != -1) {
                        String outputChunk = _textBuffer.substring(0, lastDelimiterIndex + 1);
                        _onTextChunk(outputChunk);
                        _textBuffer = _textBuffer.substring(lastDelimiterIndex + 1);
                    } else {
                        // 如果没有分隔符但缓冲区已满，直接发送全部内容
                        _onTextChunk(_textBuffer);
                        _textBuffer = "";
                    }
                }
            }
        }
    }
}

bool LLM::containsBreakPoint(const String& text) const {
    for (int i = 0; i < BREAK_POINTS_COUNT; i++) {
        if (text.indexOf(DEFAULT_BREAK_POINTS[i]) != -1) {
            return true;
        }
    }
    return false;
}

void LLM::handleError(const String& error) {
    _lastError = error;
}