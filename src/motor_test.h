
#ifndef __MOTOR_TEST_H__
#define __MOTOR_TEST_H__

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "config.h"

/*
 * list all test results of each channel
 * 
 */

#define IDLE    0
#define TESTING 1
#define END     2
#define FAIL    3

#define CS5463SCK_L (GPIOA->BRR  = GPIO_Pin_5)
#define CS5463SCK_H (GPIOA->BSRR = GPIO_Pin_5)

#define CS5463MOSI_L (GPIOA->BRR  = GPIO_Pin_7)
#define CS5463MOSI_H (GPIOA->BSRR = GPIO_Pin_7)

#define CS5463_MISO (GPIOA->IDR & GPIO_Pin_6)

#define CS5463Reset_L (GPIOC->BRR  = GPIO_Pin_4)
#define CS5463Reset_H (GPIOC->BSRR = GPIO_Pin_4)

#define CS5463_INT_V (GPIOC->IDR & GPIO_Pin_5)
#define CS5463_INT_I (GPIOB->IDR & GPIO_Pin_3)

#define CS5463CS_I_L (GPIOB->BRR = GPIO_Pin_1)
#define CS5463CS_I_H (GPIOB->BSRR = GPIO_Pin_1)

#define CS5463CS_V_L (GPIOA->BRR = GPIO_Pin_4)
#define CS5463CS_V_H (GPIOA->BSRR = GPIO_Pin_4)

#define FLASH_CS_L (GPIOB->BRR = GPIO_Pin_2)
#define FLASH_CS_H (GPIOB->BSRR = GPIO_Pin_2)

#define V_IMPROVE_RESOLUTION 1000000
#define I_IMPROVE_RESOLUTION 10000

union u32_bytes
{
	uint8 _byte[4];
	uint32 _u32;		
}__attribute__ ((packed));


typedef struct __attribute__ ((packed))
{
	union __attribute__ ((packed))
	{
		struct __attribute__ ((packed))
		{
				volatile uint8 PE0:1; 
				volatile uint8 PE1:1; 
				volatile uint8 PE2:1; 
				volatile uint8 PE3:1; 
				volatile uint8 PE4:1; 
				volatile uint8 PE5:1; 
				volatile uint8 PB8:1; 
				volatile uint8 PB9:1; 
		} __attribute__ ((packed)) bits;
		volatile uint8 uint8;
	} __attribute__ ((packed)) io_set;
	
	uint16_t sample_cnt;
	
}__attribute__ ((packed)) SYS_CONFIG;


typedef  struct __attribute__ ((packed))
{
	volatile tMEM32 motor_speed;
	volatile tMEM32 motor_dir; 
}__attribute__ ((packed)) RESULT;

typedef  struct __attribute__ ((packed))
{
	volatile uint32 V_rms_coeff_k;
	volatile uint32 V_rms_coeff_b;
	volatile uint32 V_ripple_coeff_k;
	volatile uint32 V_ripple_coeff_b;
	volatile uint32 I_rms_coeff_k[3];
	volatile uint32 I_rms_coeff_b[3];
	volatile uint32 I_ripple_coeff_k[3];	
	volatile uint32 I_ripple_coeff_b[3];	
	
}__attribute__ ((packed)) CALIBRATION;


typedef union 
{
	struct __attribute__ ((packed))
	{
		volatile uint8 sys_state:3; 
		volatile uint8 start:1; 
		volatile uint8 speed_flg:1; 
		volatile uint8 cs5460a_flg:1;
		volatile uint8 divider_sel:2; 
	} __attribute__ ((packed)) bits;
	uint8 uint8;
}__attribute__ ((packed)) SYS_STATUS;


extern SYS_CONFIG sys_config;
extern RESULT result;
extern SYS_STATUS sys_status;
extern CALIBRATION sys_coeff;



#endif


