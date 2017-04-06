
#include "config.h"
#include "user_api.h"
#include "public.h" //全局变量定义文件
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "stm32f10x_conf.h"

#include "led.h"
#include "serial.h"
#include "delay.h"
#include "motor_test.h"

volatile uint32_t encoder_a[1100];
volatile uint32_t encoder_b[1100];
volatile uint16_t tim2_update=0,encoder_a_cnt=0;

void timer2_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void timer2_rcc_init(void)
{
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void timer2_init( uint16_t time_period )
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	timer2_rcc_init();
	timer2_gpio_init();
	
	TIM_DeInit(TIM2);

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;	//0-72M/1-36M
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
	
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
}

void TIM2_IRQHandler(void)
{ 
 	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		tim2_update++;
		if((tim2_update==1) && (encoder_a_cnt<=1))
		{
			tim2_update = 0;
		}
	}

	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		encoder_a_cnt++;
		if(encoder_a_cnt == 1)
		{
			TIM2->CNT = 0;
			tim2_update = 0;
		}
		else
		{
			encoder_a[encoder_a_cnt] = tim2_update*65536 + TIM_GetCapture1(TIM2);
			if(encoder_a_cnt==1002)
			{
				result.motor_speed.uint32 = (uint64_t)60*72000000*2*10/(encoder_a[encoder_a_cnt]-encoder_a[2]);//	(encoder_a[encoder_a_cnt]-encoder_a[2])/1000;//
				if( (encoder_b[2]-encoder_a[2]) < (encoder_a[3]-encoder_b[2]) )
				{
					result.motor_dir.uint32 = 1;
				}
				else
				{
					result.motor_dir.uint32 = 2;
				}
				tim2_update = 0;
				encoder_a_cnt = 0;	
			}
		}		
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		encoder_b[encoder_a_cnt] = tim2_update*65536 + TIM_GetCapture2(TIM2);
	}

}


