#include "sys.h"

u8 humidityH;	  		//ʪ����������
u8 humidityL;	  		//ʪ��С������
u8 temperatureH;   	//�¶���������
u8 temperatureL;   	//�¶�С������

float Light; 				//���ն�

u8 Led_Status = 0;	//��ȡ��LED�Ƶ�״̬

char PUB_BUF[256];//�ϴ����ݵ�buf
//�豸��������devPubTopic: /mysmarthome/pub  --�ϴ���Ϣ��topic
//�豸��������devSubTopic: /mysmarthome/sub
const char *devSubTopic[] = {"/mysmarthome121/sub"};		//������Ϣ��topic
const char devPubTopic[] = "/mysmarthome121/pub";				//�ϴ���Ϣ��topic

u8 alarmFlag;				//�Ƿ񱨾��ı�־
u8 alarm_is_free;		//�������Ƿ��ֶ�������������ֶ�����������Ϊ0 	  

u8 ESP8266_INIT_OK = 0;	//esp8266��ʼ����ɱ�־

int main(void)
{	
	unsigned short timeCount = 0;	  //���ͼ������
	unsigned char *dataPtr = NULL;  //��������ָ��
	
	Usart1_Init(115200);	//���ڳ�ʼ��Ϊ115200 -- ����ͨ����λ�������Ϣ
		DEBUG_LOG("\r\n");
		DEBUG_LOG("UART1��ʼ��			[OK]");
	delay_init();	    	 //��ʱ������ʼ��
		DEBUG_LOG("��ʱ������ʼ��			[OK]");
	
	
	OLED_Init();					//��ʼ��oled
	OLED_ColorTurn(0);		//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);	//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();	
		DEBUG_LOG("OLED1��ʼ��			[OK]");
		OLED_Refresh_Line("OLED [OK]");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		DEBUG_LOG("�жϼ����ʼ��			[OK]");
		OLED_Refresh_Line("OLED [OK]");
	
	LED_Init();			     //LED�˿ڳ�ʼ��
		DEBUG_LOG("LED��ʼ��			[OK]");
		OLED_Refresh_Line("LED [OK]");
	KEY_Init();          	//��ʼ���밴�����ӵ�Ӳ���ӿ�
		DEBUG_LOG("������ʼ��			[OK]");
		OLED_Refresh_Line("Key");
	EXTIX_Init();					//��ʼ���ⲿ�ж�--���ڰ������
		DEBUG_LOG("�ⲿ�жϳ�ʼ��			[OK]");
		OLED_Refresh_Line("EXIT");
	BEEP_Init();         	//��ʼ���������˿�
		DEBUG_LOG("��������ʼ��			[OK]");
		OLED_Refresh_Line("Beep");
	BH1750_Init();				//��ʼ��BH1750
		DEBUG_LOG("BH1750��ʼ��			[OK]");
		OLED_Refresh_Line("BH1750");
	DHT11_Init();					//��ʼ��dht11
		DEBUG_LOG("DHT11��ʼ��			[OK]");
		OLED_Refresh_Line("DHT11");
	Usart2_Init(115200);	//��ʼ������ -- stm32-8266ͨѶ����
		DEBUG_LOG("UART2��ʼ��			[OK]");
		OLED_Refresh_Line("Uart2");
		
	DEBUG_LOG("Ӳ����ʼ��			[OK]");		//Ӳ����ʼ����ɣ�����һ�����ӡ��Ϣ��Hardware init OK
	delay_ms(1000);	
	DEBUG_LOG("��ʼ��ESP8266 WIFIģ��...");
	
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		OLED_ShowString(0,0,(u8*)"WiFi",16,1);
		OLED_ShowChinese(32,0,8,16,1);//��
		OLED_ShowChinese(48,0,9,16,1);//��
		OLED_ShowChinese(64,0,10,16,1);//��
		OLED_ShowString(80,0,(u8*)"...",16,1);
		OLED_Refresh();
	}
	ESP8266_Init();							//��ʼ��ESP8266
	
	OLED_Clear();
	OLED_ShowChinese(0,0,4,16,1); //��
	OLED_ShowChinese(16,0,5,16,1);//��
	OLED_ShowChinese(32,0,6,16,1);//��
	OLED_ShowChinese(48,0,8,16,1);//��
	OLED_ShowChinese(64,0,9,16,1);//��
	OLED_ShowChinese(80,0,10,16,1);//��
	OLED_ShowString(96,0,(u8*)"...",16,1);
	OLED_Refresh();	

	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	OLED_Clear();
	
	TIM2_Int_Init(4999,7199);	//��ʼ����ʱ��2����oled��ʾ�¡�ʪ�ȣ�����
	TIM3_Int_Init(2499,7199);	//��ʼ����ʱ��3�����ڼ�ⱨ��

	LED0 = 0;		//LED0�������Ӳ����ʼ�����
	
	OneNet_Subscribe(devSubTopic, 1);	//������Ҫ���ĵ�topics
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 һ��ִ��һ��     //��ȡһ�δ������������
		{
			/********** ��ʪ�ȴ�������ȡ����**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** ���նȴ�������ȡ����**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** ��ȡLED1��״̬ **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5);
			
			/********** �����߼� **************/
			if(alarm_is_free == 10)//����������Ȩ�Ƿ���� alarm_is_free == 10 ��ʼֵΪ10
			{
				if((humidityH < 80) && (temperatureH < 30) && (Light < 1000))alarmFlag = 0;
				else alarmFlag = 1;
			}
			if(alarm_is_free < 10)alarm_is_free++;

			
			/**********  �Ӵ���һ��������Ϣ��ӡ��XCOM  **********/
			DEBUG_LOG(" | ʪ��:%d.%d %% | �¶�:%d.%d C | ���ն�:%.1f lx ",humidityH,humidityL,temperatureH,temperatureL,Light);
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200 ���ͼ��5000ms  //ÿ��5���ϴ�һ������
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5);//��ȡLED1��״̬
			DEBUG_LOG(">>>>>> push massge start: >>>>>>");
			DEBUG_LOG("�������� ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?0:1,alarmFlag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG(">>>>>> push massge end :  >>>>>>");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		//�����û���·�ָ�Ҳ�ǽ���mqtt�����������ݣ����ݵĵ�ַ������dataPtrָ������
		dataPtr = ESP8266_GetIPD(3);		//��Ҫ15ms
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	}
}
