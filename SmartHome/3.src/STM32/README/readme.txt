GPIO引脚的接口定义
LED0: PB5
LED1: PE5

//按键采取外部中断
KEY0: PE4
KEY1: PA0 (WK_UP)

BEEP: PB8

DHT11:PA8

BH1750:
地址：0x46
SCL: PD6
SDA:PD7

OLED:
SCL: PA5
SDA:PA7

串口一：
TX:PA9
RX:PA10

串口二：
TX:PA2
RX:PA3

onenet开发板资料地址：				https://open.iot.10086.cn/bbs/thread-863-1-1.html
MQTT官网：							https://www.emqx.io/zh
MQTT免费服务器url：					https://www.emqx.com/zh/mqtt/public-mqtt5-broker
MQTT浏览器客户端，在线测试的url：	https://mqttx.app/zh/web
或者有MQTT fx软件

在客户端需要建立connect（注意host（ip）），然后comfirm（注意topic）

完成ESP8266 连接 MQTT 上云

利用cJSON可以实现MQTT发送给的下位机数据的接受

完成MQTT给下位机下发消息，下位机得到响应

从云服务sub数据开灯:
{
"target":"LED",
"value":"1"
}

从云服务sub数据关灯:
{
"target":"LED",
"value":"0"
}

从云服务sub数据开蜂鸣器:
{
"target":"BEEP",
"value":"1"
}

从云服务sub数据关蜂鸣器:
{
"target":"BEEP",
"value":"0"
}