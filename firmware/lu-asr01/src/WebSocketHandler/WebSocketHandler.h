#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <functional>

// 默认的缓冲区大小
#define DEFAULT_BUFFER_SIZE 512

class WebSocketHandler {
public:
    WebSocketHandler(const char* serverUrl, uint16_t port, const char* path);
    void begin();
    void loop();
    void setOnMessageCallback(std::function<void(WStype_t, uint8_t *, size_t)> callback);
    
    // 设置WebSocket缓冲区大小
    void setBufferSize(uint16_t size);

private:
    const char* serverUrl;
    uint16_t port;
    const char* path;
    WebSocketsClient webSocket;
    std::function<void(WStype_t, uint8_t *, size_t)> onMessageCallback;
    uint16_t bufferSize;

    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
};

#endif // WEBSOCKETHANDLER_H