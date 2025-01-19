# 小熊夜灯控制板
![](/readme/2025-01-19%20145228.png)
使用点灯科技APP控制，界面配置如下
```
{¨config¨{¨headerColor¨¨transparent¨¨headerStyle¨¨light¨¨background¨{¨img¨¨assets/img/headerbg.jpg¨¨isFull¨«}}¨dashboard¨|{¨type¨¨deb¨¨mode¨É¨bg¨É¨cols¨Ñ¨rows¨Ì¨key¨¨debug¨´x´É´y´¤C}{ßA¨btn¨¨ico¨¨fal fa-power-off¨ßCÊ¨t0¨¨开关灯¨¨t1¨¨文本2¨ßDÉßEËßFËßG¨PWM1¨´x´É´y´Î¨speech¨|÷¨lstyle¨É}{ßA¨tex¨ßL´´ßN¨开关状态¨¨size¨¤EßDÉßJ¨fal fa-lightbulb¨ßEÍßFËßG¨T1¨´x´Ë´y´ÌßQ|÷ßRÌ¨clr¨¨#076EEF¨}{ßA¨inp¨ßDÉßEÑßFËßG¨inp-6yh¨´x´É´y´É}{ßAßIßJßKßCÉßL¨呼吸¨ßNßOßDÉßEËßFËßG¨POWER¨´x´É´y´ÌßQ|÷ßX¨#EA0909¨}{ßA¨col¨ßL¨颜色拾取¨ßX¨#389BEE¨ßDÉßEÏßFÏßGße´x´Ë´y´ÎßQ|÷ßRÉ}÷¨actions¨|¦¨cmd¨¦¨switch¨‡¨text¨‡¨on¨¨打开?name¨¨off¨¨关闭?name¨—÷¨triggers¨|{¨source¨ßj¨source_zh¨ßT¨state¨|ßlßn÷¨state_zh¨|¨打开¨¨关闭¨÷}÷}
```
## 使用说明
1.打开点灯科技app，添加一个独立设备，将以上界面配置粘贴至该设备的界面配置中，点击更新配置即可自动添加控制按钮等界面，复制该设备的秘钥以备接下来使用。
2.连接名为小熊夜灯的wifi热点，自动弹出网络配置界面，点击要连接的wifi名称，输入密码，将秘钥填入auth框内，点击确认，等待蓝色板载指示灯灭（呼吸效果出现）表示已经连接至wifi网络。
3.打开点灯APP，点击按钮即可控制呼吸效果和开关灯光。
## 硬件说明
flash按键为程序烧录按键，同时为配网过程中网络密码输入正确但是auth输错的清除密码信息按键。
## 程序编译
### 需要修改Servo.h下的定义，改变默认的驱动舵机的pwm占空比
```
#define DEFAULT_MIN_PULSE_WIDTH      0 // uncalibrated default,the shortest duty cycle sent to a servo
#define DEFAULT_MAX_PULSE_WIDTH      20000 // uncalibrated default, the longest duty cycle sent to a servo 
#define DEFAULT_NEUTRAL_PULSE_WIDTH  1500 // default duty cycle when servo is attached
#define REFRESH_INTERVAL            20000 // classic default period to refresh servos in microseconds 

```
# 清除配网信息
## 1.长按flash按键3秒以上
## 2.在可以通过app控制的情况下，在APP输入框输入“1”，即可。