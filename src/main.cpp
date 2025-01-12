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
/* LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成*/
const uint16_t indexWave[] = {
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
    4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
    15, 17, 19, 22, 25, 28, 32, 36,
    41, 47, 53, 61, 69, 79, 89, 102,
    116, 131, 149, 170, 193, 219, 250,
    284, 323, 367, 417, 474, 539, 613,
    697, 792, 901, 1024, 1024, 901, 792,
    697, 613, 539, 474, 417, 367, 323,
    284, 250, 219, 193, 170, 149, 131,
    116, 102, 89, 79, 69, 61, 53, 47, 41,
    36, 32, 28, 25, 22, 19, 17, 15, 13,
    11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
    2, 2, 2, 2, 1, 1, 1, 1};

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
// void breath()
// {
//   if (j)
//   {
//     led.write(map(indexWave[i], 0, 1024, 0, 180));
//     pixels.setBrightness(map(indexWave[i], 0, 1024, 0, 255));
//     Serial.println(map(indexWave[i], 0, 1024, 0, 255));
//     i++;
//     pixels.show();
//     if (i == 110)
//     {
//       j = !j;
//     }
//   }
//   else
//   {
//     led.write(map(indexWave[i], 0, 1024, 0, 180));
//     pixels.setBrightness(map(indexWave[i], 0, 1024, 0, 255));
//     Serial.println(map(indexWave[i], 0, 1024, 0, 255));
//     i--;
//     pixels.show();
//     if (i == 0)
//     {
//       j = !j;
//     }
//   }
// }
void breath()
{
  if (j)
  {
    led.write(i);
    pixels.setBrightness((uint8_t)map(i, 0, 180, 0, 255));
    pixels.show();
    Serial.println((uint8_t)map(i, 0, 180, 0, 255));
    i++;
    if (i == 180)
    {
      j = !j;
    }
  }
  else
  {
    i--;
    led.write(i);
    pixels.setBrightness((uint8_t)map(i, 0, 180, 0, 255));
    pixels.show();
    Serial.println((uint8_t)map(i, 0, 180, 0, 255));
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
