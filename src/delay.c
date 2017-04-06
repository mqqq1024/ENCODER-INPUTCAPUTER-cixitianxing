
#include "main.h"
#include "config.h"
#include "user_api.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "stm32f10x_conf.h"


void delay_ms( uint32 nms )
{
	volatile u32 i, j;			//"volatile" is essential for these two variables to avoid being optized by compiler
	for(i=0;i<nms;i++)
		for(j=0;j<5980;j++)		//5538 for "volatile u32 nmS"
		;	
}

void delay_1us( void )		 //1.432us-0.293us=	1.139us
{
	volatile u16 i;			//"volatile" is essential for these two variables to avoid being optized by compiler
		for(i=0;i<4;i++)		//
		;	
}

void delay_3us( void )
{
	volatile u16 i;			//"volatile" is essential for these two variables to avoid being optized by compiler
		for(i=0;i<12;i++)		//
		;	
}
void delay_4us( void )
{
	volatile u16 i;			//"volatile" is essential for these two variables to avoid being optized by compiler
		for(i=0;i<17;i++)		//
		;	
}
void delay_5us( void )
{
	volatile u16 i;			//"volatile" is essential for these two variables to avoid being optized by compiler
		for(i=0;i<22;i++)		//
		;	
}
void delay_7us( void )
{
	volatile u16 i;			//"volatile" is essential for these two variables to avoid being optized by compiler
		for(i=0;i<30;i++)		//
		;	
}
void delay_10us( u16 nus )
{
	volatile u16 i, j;			//"volatile" is essential for these two variables to avoid being optized by compiler
	for(i=0;i<nus;i++)
		for(j=0;j<41;j++)		//
		;	
}
void DelayF ( uint8 nus )
{
	volatile u16 i;	
		for(i=0;i<4*nus;i++)		//
		;	
	
}