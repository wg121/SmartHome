#include "sys.h"

u8 humidityH;	  		//湿度整数部分
u8 humidityL;	  		//湿度小数部分
u8 temperatureH;   	//温度整数部分
u8 temperatureL;   	//温度小数部分

float Light; 				//光照度

u8 Led_Status = 0;	//获取的LED灯的状态

char PUB_BUF[256];//上传数据的buf
//设备上行数据devPubTopic: /mysmarthome/pub  --上传消息的topic
//设备订阅命令devSubTopic: /mysmarthome/sub
const char *devSubTopic[] = {"/mysmarthome121/sub"};		//订阅消息的topic
const char devPubTopic[] = "/mysmarthome121/pub";				//上传消息的topic

u8 alarmFlag;				//是否报警的标志
u8 alarm_is_free;		//报警器是否被手动操作，如果被手动操作即设置为0 	  

u8 ESP8266_INIT_OK = 0;	//esp8266初始化完成标志

int main(void)
{	
	unsigned short timeCount = 0;	  //发送间隔变量
	unsigned char *dataPtr = NULL;  //接收数据指针
	
	Usart1_Init(115200);	//串口初始化为115200 -- 用于通过上位机输出信息
		DEBUG_LOG("\r\n");
		DEBUG_LOG("UART1初始化			[OK]");
	delay_init();	    	 //延时函数初始化
		DEBUG_LOG("延时函数初始化			[OK]");
	
	
	OLED_Init();					//初始化oled
	OLED_ColorTurn(0);		//0正常显示，1 反色显示
  OLED_DisplayTurn(0);	//0正常显示 1 屏幕翻转显示
	OLED_Clear();	
		DEBUG_LOG("OLED1初始化			[OK]");
		OLED_Refresh_Line("OLED [OK]");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		DEBUG_LOG("中断级别初始化			[OK]");
		OLED_Refresh_Line("OLED [OK]");
	
	LED_Init();			     //LED端口初始化
		DEBUG_LOG("LED初始化			[OK]");
		OLED_Refresh_Line("LED [OK]");
	KEY_Init();          	//初始化与按键连接的硬件接口
		DEBUG_LOG("按键初始化			[OK]");
		OLED_Refresh_Line("Key");
	EXTIX_Init();					//初始化外部中断--用于按键检测
		DEBUG_LOG("外部中断初始化			[OK]");
		OLED_Refresh_Line("EXIT");
	BEEP_Init();         	//初始化蜂鸣器端口
		DEBUG_LOG("蜂鸣器初始化			[OK]");
		OLED_Refresh_Line("Beep");
	BH1750_Init();				//初始化BH1750
		DEBUG_LOG("BH1750初始化			[OK]");
		OLED_Refresh_Line("BH1750");
	DHT11_Init();					//初始化dht11
		DEBUG_LOG("DHT11初始化			[OK]");
		OLED_Refresh_Line("DHT11");
	Usart2_Init(115200);	//初始化串口 -- stm32-8266通讯串口
		DEBUG_LOG("UART2初始化			[OK]");
		OLED_Refresh_Line("Uart2");
		
	DEBUG_LOG("硬件初始化			[OK]");		//硬件初始化完成，串口一输出打印信息：Hardware init OK
	delay_ms(1000);	
	DEBUG_LOG("初始化ESP8266 WIFI模块...");
	
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		OLED_ShowString(0,0,(u8*)"WiFi",16,1);
		OLED_ShowChinese(32,0,8,16,1);//连
		OLED_ShowChinese(48,0,9,16,1);//接
		OLED_ShowChinese(64,0,10,16,1);//中
		OLED_ShowString(80,0,(u8*)"...",16,1);
		OLED_Refresh();
	}
	ESP8266_Init();							//初始化ESP8266
	
	OLED_Clear();
	OLED_ShowChinese(0,0,4,16,1); //服
	OLED_ShowChinese(16,0,5,16,1);//务
	OLED_ShowChinese(32,0,6,16,1);//器
	OLED_ShowChinese(48,0,8,16,1);//连
	OLED_ShowChinese(64,0,9,16,1);//接
	OLED_ShowChinese(80,0,10,16,1);//中
	OLED_ShowString(96,0,(u8*)"...",16,1);
	OLED_Refresh();	

	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	
	OLED_Clear();
	
	TIM2_Int_Init(4999,7199);	//初始化定时器2，在oled显示温、湿度，光照
	TIM3_Int_Init(2499,7199);	//初始化定时器3，用于检测报警

	LED0 = 0;		//LED0亮起代表硬件初始化完成
	
	OneNet_Subscribe(devSubTopic, 1);	//订阅想要订阅的topics
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 一秒执行一次     //获取一次传感器获得数据
		{
			/********** 温湿度传感器获取数据**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** 光照度传感器获取数据**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** 读取LED1的状态 **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5);
			
			/********** 报警逻辑 **************/
			if(alarm_is_free == 10)//报警器控制权是否空闲 alarm_is_free == 10 初始值为10
			{
				if((humidityH < 80) && (temperatureH < 30) && (Light < 1000))alarmFlag = 0;
				else alarmFlag = 1;
			}
			if(alarm_is_free < 10)alarm_is_free++;

			
			/**********  从串口一输出相关信息打印到XCOM  **********/
			DEBUG_LOG(" | 湿度:%d.%d %% | 温度:%d.%d C | 光照度:%.1f lx ",humidityH,humidityL,temperatureH,temperatureL,Light);
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200 发送间隔5000ms  //每隔5秒上传一次数据
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5);//读取LED1的状态
			DEBUG_LOG(">>>>>> push massge start: >>>>>>");
			DEBUG_LOG("发布数据 ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?0:1,alarmFlag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG(">>>>>> push massge end :  >>>>>>");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		//检查有没有下发指令，也是接受mqtt发送来的数据，数据的地址保存在dataPtr指针里面
		dataPtr = ESP8266_GetIPD(3);		//需要15ms
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	}
}
