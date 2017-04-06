
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "serial.h"
/* Demo program include files. */
#include "config.h" //
#include "public.h" //

#include "user_api.h" //
#include "stm32f10x.h"
#include <stdlib.h>
#include <math.h>
#include "led.h"
#include "AT45DB041E.h"
#include "stm32f10x_conf.h"
#include "motor_test.h"


void flash_gpio_init(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;
 
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
 	
// //     GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
// //     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
// //     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// //     GPIO_Init(GPIOD, &GPIO_InitStructure);

// // 	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
// //     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
// //     GPIO_Init(GPIOD, &GPIO_InitStructure);
 
//   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOB, &GPIO_InitStructure);
// 		GPIO_SetBits(GPIOB, GPIO_Pin_2);
// 	
// 	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	
// 	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static inline void delay(void)
{
	__DSB();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

uint8_t SPI_SendReceiveByte(uint8_t dat)
{
	uint8_t tmp = 0;
	int i;
	for(i = 0; i < 8; i++)
	{
		CS5463SCK_L;////GPIOA->BRR = 1<<5;//GPIOD->BRR = 2;
		if(dat & 0x80)
		{
			CS5463MOSI_H;////GPIOA->BSRR = 1<<7;//GPIOD->BSRR = 1;
		}
		else
		{
			CS5463MOSI_L;////GPIOA->BRR = 1<<7;//GPIOD->BRR = 1;
		}
		delay();

		CS5463SCK_H;////GPIOA->BSRR = 1<<5;//GPIOD->BSRR = 2;
		tmp <<= 1;
		tmp |= ((GPIOA->IDR & 0x40ul) >> 6);//(CS5463_MISO>>6);////GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);////((GPIOA->IDR & 0x40ul) >> 6);//((GPIOD->IDR & 0x08ul) >> 3);
		dat <<= 1;
		delay();
	}
	CS5463SCK_L;////GPIOA->BRR = 1<<5;;//GPIOD->BRR = 2;
	delay();

	return tmp;
}

void AT45DB041E_CheckBusy(void)
{
	uint32_t cnt = 0;
	uint8_t tmp = 0;

	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	do{
		SPI_SendReceiveByte(0xd7);
		tmp = SPI_SendReceiveByte(0xff);
		delay();
		cnt++;
	}while((cnt < 0xffff) && (tmp & 0x80));

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();
}

void AT45DB041E_Read(uint32_t addr, uint8_t *buf, uint32_t size)
{
	int i;
	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	SPI_SendReceiveByte(0xe8);
	SPI_SendReceiveByte((addr >> 16) & 0xfful);
	SPI_SendReceiveByte((addr >> 8) & 0xfful);
	SPI_SendReceiveByte(addr & 0xfful);
	SPI_SendReceiveByte(0xff);
	SPI_SendReceiveByte(0xff);
	SPI_SendReceiveByte(0xff);
	SPI_SendReceiveByte(0xff);

	for(i = 0; i < size; i++)
	{
		*buf++ = SPI_SendReceiveByte(0xff);
	}

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();
}

void AT45DB041E_Write(uint32_t addr, uint8_t *buf, uint32_t size)
{
	int i ;
	
	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	SPI_SendReceiveByte(0x84);
	SPI_SendReceiveByte(0);
	SPI_SendReceiveByte(0);
	SPI_SendReceiveByte(0);

	for(i= 0; i < size; i++)
	{
		SPI_SendReceiveByte(*buf++);
	}

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();

	AT45DB041E_CheckBusy();

	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	SPI_SendReceiveByte(0x83);
	SPI_SendReceiveByte((addr >> 16) & 0xfful);
	SPI_SendReceiveByte((addr >> 8) & 0xfful);
	SPI_SendReceiveByte(addr & 0xfful);

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();
}

void init_at45db041e(void)
{	
	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	SPI_SendReceiveByte(0x3d);
	SPI_SendReceiveByte(0x2a);
	SPI_SendReceiveByte(0x80);
	SPI_SendReceiveByte(0xa6);

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();
	AT45DB041E_CheckBusy();

	FLASH_CS_L;////GPIOB->BRR = 1 << 2;//GPIOD->BRR = 1 << 2;
	delay();

	SPI_SendReceiveByte(0x3d);
	SPI_SendReceiveByte(0x2a);
	SPI_SendReceiveByte(0x7f);
	SPI_SendReceiveByte(0x9a);

	FLASH_CS_H;////GPIOB->BSRR = 1 << 2;//GPIOD->BSRR = 1 << 2;
	delay();
	AT45DB041E_CheckBusy();
}


