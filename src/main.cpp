#include <Arduino.h>
#include <ESP8266WiFi.h>
#define BLINKER_WIFI
#include <Blinker.h>
#include <main.h>
#include <Adafruit_NeoPixel.h>
#include "OneButton.h"
#include <Servo.h>

#define PIN_INPUT D3
#define PIN 5 // ws2812b连接的io

#define NUMPIXELS 1   // Popular NeoPixel ring size
#define serveSwitch 4 // mos连接的io
Ticker timer2;        // 建立Ticker用于实现定时功能
Servo led;            // create servo object to control a led
OneButton button(PIN_INPUT, true);
uint8_t i = 0;
bool j = 1, breath_flag = 1;
const uint16_t indexWave[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10,
    11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17,
    17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35,
    36, 37, 38, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 68, 69, 70, 71, 72, 73, 75, 76, 77, 78, 80, 81,
    82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96, 97, 99, 100, 102,
    103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
    127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
    154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
    255};

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
StaticJsonDocument<200> doc;

#define TEXTE1 "T1"
BlinkerText Text1(TEXTE1);
#define BUTTON1 "PWM1"
BlinkerButton Button1(BUTTON1);
#define BUTTON2 "POWER"
BlinkerButton Button2(BUTTON2);

// this function will be called when the button started long pressed.
void LongPressStart(void *oneButton)
{
  Serial.print(((OneButton *)oneButton)->getPressedMs());
  Serial.println("\t - LongPressStart()");
}

// this function will be called when the button is released.
void LongPressStop(void *oneButton)
{
  Serial.print(((OneButton *)oneButton)->getPressedMs());
  Serial.println("\t - LongPressStop()\n");
}

// this function will be called when the button is held down.
void DuringLongPress(void *oneButton)
{
  wifiManager.resetSettings(); // reset saved settings
  ESP.reset();
}

// 呼吸效果
void breath()
{
  if (j)
  {
    led.write(map(indexWave[i], 0, 255, 0, 180));
    pixels.setBrightness(map(indexWave[i], 0, 255, 1, 255));
    pixels.show();
    Serial.println(map(indexWave[i], 0, 255, 1, 255));
    i++;
    if (i == 255)
    {
      j = !j;
    }
  }
  else
  {
    led.write(map(indexWave[i], 0, 255, 0, 180));
    pixels.setBrightness(map(indexWave[i], 0, 255, 1, 255));
    pixels.show();
    Serial.println(map(indexWave[i], 0, 255, 1, 255));
    i--;
    if (i == 0)
    {
      j = !j;
    }
  }
}

// ws2812换颜色
void color_change(uint8_t red, uint8_t green, uint8_t blue, uint8_t Brightness)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  pixels.clear(); // Set all pixel colors to 'off'
  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  // Here we're using a moderately bright green color:
  pixels.setBrightness(Brightness);
  pixels.setPixelColor(0, pixels.Color(red, green, blue)); // Pixel index, starting from 0.
  pixels.show();                                           // Send the updated pixel colors to the hardware.
}

// 打开mos
void serve_enable(bool state)
{
  digitalWrite(serveSwitch, state);
}

// 按钮1数据处理回调函数，开关状态取反
void Button1Callback(const String &state)
{
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("Toggle on!");
    Button1.icon("fas fa-lightbulb");
    Button1.print("on");
    led.write(180);
    // myservo.write(zero_angle - angle - 5); // tell servo to go to position in variable 'pos'
    // delay(300);
    // myservo.write(zero_angle - 1);
    // delay(100);
    // serve_enable(0);
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("Toggle off!");
    Button1.icon("far fa-lightbulb");
    Button1.print("off");
    // serve_enable(1);
    // myservo.write(zero_angle + angle + 5); // tell servo to go to position in variable 'pos'
    // delay(300);
    // myservo.write(zero_angle + 1);
    // delay(100);
    led.write(0);
  }
}

// 按钮2数据处理回调函数，断开总电源
void Button2Callback(const String &state)
{
  if (state == BLINKER_CMD_BUTTON_TAP)
  {
    BLINKER_LOG("Button tap!");
    Button1.text("呼吸");
    breath_flag = !breath_flag;
  }
}

// 自定义心跳函数，返回开关状态
void heartbeat()
{
  // if (myservo.read() > zero_angle)
  //   Text1.print("灯开");
  // else
  //   Text1.print("灯关");
}

// 检测收到未解析数据时的回调函数
void dataRead(const String &data)
{
  BLINKER_LOG("Blinker readString: ", data);
  deserializeJson(doc, data);
  int d_flag = data.toInt();
  if (d_flag == 1) // 发送数字为1，清除联网信息
  {
    wifiManager.resetSettings();
    Blinker.print("resetSettings and reset ESP");
    ESP.reset();
  }
  Blinker.vibrate();
  uint32_t BlinkerTime = millis();
  Blinker.print("millis", BlinkerTime);
  uint8_t red = doc["col"][0];
  uint8_t green = doc["col"][1];
  uint8_t blue = doc["col"][2];
  uint8_t Brightness = doc["col"][3];
  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);
  Serial.println(Brightness);
  color_change(red, green, blue, Brightness);
}

void setup()
{
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 250); // 设置睡眠模式（可选参数：WIFI_NONE_SLEEP、WIFI_LIGHT_SLEEP、WIFI_MODEM_SLEEP ）
  led.attach(serveSwitch);                  // PWM1
  led.write(0);
  Serial.begin(115200); // 初始化串口服务
  BLINKER_DEBUG.stream(Serial);
  pinMode(BUILTIN_LED, OUTPUT); // 指示灯初始状态
  digitalWrite(BUILTIN_LED, LOW);
  // pinMode(serveSwitch, OUTPUT); // 舵机初始状态
  // digitalWrite(serveSwitch, LOW);
  init_littlefs();
  init_wifiManager();

  // Binker设备配置
  Serial.println(auth);
  Serial.println(ssid);
  Serial.println(pswd);
  Blinker.begin(auth, ssid, pswd);

  // 注册Blinker APP命令的回调函数
  Blinker.attachData(dataRead);
  Button1.attach(Button1Callback); // 按钮1回调函数注册
  Button2.attach(Button2Callback); // 按钮2回调函数注册
  Blinker.attachHeartbeat(heartbeat);
  timer2.attach(30, flag_change);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  button.attachLongPressStart(LongPressStart, &button);
  button.attachDuringLongPress(DuringLongPress, &button);
  button.attachLongPressStop(LongPressStop, &button);

  button.setLongPressIntervalMs(3000); // 长按3s断网
}

void loop()
{
  if (flag)
  {
    Serial.println(auth);
    Serial.println(ssid);
    Serial.println(pswd);
    // 检测blinker的auth值是否正确，正确blinker初始化成功，Blinker.init()值不会在一个循环内刷新，故放置在10s循环内
    if (!Blinker.init())
    {
      // wifiManager.resetSettings(); // reset saved settings
      // ESP.reset();
    }
    // 检测运行是否断网，WiFi.status()值不会立即刷新
    if (WiFi.status() == WL_CONNECTED)
      digitalWrite(LED_BUILTIN, HIGH);
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      // wifiManager.resetSettings(); // reset saved settings
      // ESP.reset();
    }
    flag = 0;
  }
  Blinker.run();
  button.tick();
  if (breath_flag)
  {
    breath();
  }
}
