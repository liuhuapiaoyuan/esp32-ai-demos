#include "WebSocketHandler.h"

WebSocketHandler::WebSocketHandler(const char* serverUrl, uint16_t port, const char* path)
    : serverUrl(serverUrl), port(port), path(path), bufferSize(DEFAULT_BUFFER_SIZE) {}

void WebSocketHandler::begin() {
    // 设置缓冲区大小
    // webSocket.setExtraHeaders("Sec-WebSocket-Protocol: audio");
    // webSocket.setBuffer(bufferSize * 1024); // 转换为字节
    
    webSocket.begin(serverUrl, port, path);
    webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(type, payload, length);
    });
}

void WebSocketHandler::loop() {
    webSocket.loop();
}

void WebSocketHandler::setOnMessageCallback(std::function<void(WStype_t, uint8_t *, size_t)> callback) {
    onMessageCallback = callback;
}

void WebSocketHandler::setBufferSize(uint16_t size) {
    bufferSize = size;
    // if (webSocket.isConnected()) {
    //     webSocket.disconnect();
    //     webSocket.setBuffer(bufferSize * 1024);
    //     webSocket.begin(serverUrl, port, path);
    // }
}

void WebSocketHandler::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    if (onMessageCallback) {
        onMessageCallback(type, payload, length);
    }
}