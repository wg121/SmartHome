#include "led.h"

// LED0  PB5
// LED1  PE5	

/*
************************************************************
*	函数名称：	LED_Init
*
*	函数功能：  初始化LED的GPIO
*
*	入口参数：	void
*
*	返回参数：	无
*
*	说明：		  初始化PB5和PE5为输出口.并使能这两个口的时钟	
************************************************************
*/
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 			//LED0 --> PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);											 	//PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 		//LED1 --> PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 			//推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 						//PE.5 输出高 
}
 
