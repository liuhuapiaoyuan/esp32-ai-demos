#ifndef LLM_H
#define LLM_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <functional>
#include <vector>

class LLM {
public:
    // 定义回调函数类型
    using TextChunkCallback = std::function<void(const String&)>;

    LLM();
    void begin(const char* apiKey, const char* apiEndpoint);
    String chatCompletion(const String& prompt);
    String getLastError() const;
    
    // 设置回调函数
    void setTextChunkCallback(TextChunkCallback callback) { _onTextChunk = callback; }

private:
    String makeRequest(const String& prompt);
    void handleError(const String& error);
    void processSSEChunk(const String& chunk);
    bool containsBreakPoint(const String& text) const;

    static const char* DEFAULT_BREAK_POINTS[];
    static const int BREAK_POINTS_COUNT;

    String _apiKey;
    String _apiEndpoint;
    String _lastError;
    HTTPClient _http;
    
    // 文本块处理相关
    String _textBuffer;
    TextChunkCallback _onTextChunk;
    
    // JSON document for API requests and responses
    JsonDocument _jsonDoc;
};

#endif // LLM_H