const int buttonPin = 10;  // 按钮连接到 GPIO10
int buttonState = 0;       // 用于存储按钮状态的变量
int count = 0;             // 用于记录按钮按下次数的变量

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // 将按钮引脚设置为输入，并启用内部上拉电阻
  Serial.begin(115200);              // 初始化串口通信，波特率为 115200
}

void loop() {
  buttonState = digitalRead(buttonPin);  // 读取按钮状态

  if (buttonState == LOW) {  // 如果按钮被按下（由于上拉电阻，按钮未按下时为 HIGH）
    count++;                 // 计数器加 1
    Serial.print("Button Pressed: ");
    Serial.println(count);   // 打印当前计数
    delay(200);              // 简单的去抖动延迟
    while (digitalRead(buttonPin) == LOW) {
      // 等待按钮释放，避免多次计数
    }
  }
}