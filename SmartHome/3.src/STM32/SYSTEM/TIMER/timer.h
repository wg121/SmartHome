#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "oled.h"
#include "led.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

void TIM2_Int_Init(u16 arr,u16 psc); 
void TIM3_Int_Init(u16 arr,u16 psc); 
 
#endif
