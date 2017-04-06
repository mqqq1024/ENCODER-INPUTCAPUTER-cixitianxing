
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "config.h"
#include "public.h"

/* Library includes. */
#include "stm32f10x_conf.h"
#include "serial.h"
#include "motor_test.h"
#include <string.h>
#include <stdlib.h>
#include "AT45DB041E.h"
#include "delay.h"


#define CMD_START          0x00
#define CMD_END            0x01
#define CMD_RD_STATUS      0x02
#define CMD_RD_RESULT      0x03
#define CMD_SET            0x04
#define CMD_RD_DETAIL      0x05
#define CMD_RD_COEFF       0x07

volatile uint8 version_send;
extern const uint8 version[40];
extern volatile uint32_t encoder_a[1100];
extern volatile uint32_t encoder_b[1100];

uint8 get_check_sum(uint8* buf, uint32 cnt)
{
	uint32 i;
	uint8 check_sum=0;
	
	for(i=0;i<cnt;i++)
	{
		check_sum += buf[i];
	}
	return check_sum;
}

uint8 validate_check_rx_data(uint8* rx_buf, uint8* tx_buf)
{
	uint8 check_sum=0;

	check_sum = get_check_sum(rx_buf+1, *(rx_buf+1)-3); 
	if((check_sum == rx_buf[*(rx_buf+1)-2]) && (rx_buf[*(rx_buf+1)-1] == 0x7D))
	{
		if((*(rx_buf+3) == CMD_START) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_END) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_RD_STATUS) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_RD_RESULT) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_RD_RESULT) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_RD_DETAIL) && (*(rx_buf+1) == 0x06))
			return 1;
		else if((*(rx_buf+3) == CMD_SET) && (*(rx_buf+1) == 0x09))
			return 1;
		else if((*(rx_buf+3) == CMD_RD_COEFF) && (*(rx_buf+1) == 0x06))
			return 1;
		else
			return 0;		
	}
	else
		return 0;
}

void update_sys_status(uint8 cmd,uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	if(cmd == CMD_START)
	{
		sys_status.bits.sys_state = TESTING;
		sys_status.bits.start = 1;
//		sys_status.bits.cs5460a_flg = 0;
//		sys_status.bits.speed_flg = 0;

	
	}
	else if(cmd == CMD_END)
	{
		sys_status.bits.sys_state = IDLE;	
		sys_status.bits.start = 0;		
	}
	memcpy((void *)tx_buf, (void *)rx_buf, *(rx_buf+1));
	*len = *(rx_buf+1);
}

void read_sys_status(uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	*tx_buf = 0x7B;
	*(tx_buf+1) = 0x07;
	*(tx_buf+2) = 0x00;
	*(tx_buf+3) = *(rx_buf+3);
	*(tx_buf+4) = sys_status.uint8;
	*(tx_buf+5) = get_check_sum((tx_buf+1), *(tx_buf+1)-3);
	*(tx_buf+6) = 0x7D;
	*len = *(tx_buf+1);	
}

void read_result(uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	*tx_buf = 0x7B;
	*(tx_buf+1) = 6+8;
	*(tx_buf+2) = 0x00;
	*(tx_buf+3) = *(rx_buf+3);
	*(tx_buf+4) = result.motor_speed.uint8._0;
	*(tx_buf+5) = result.motor_speed.uint8._1;
	*(tx_buf+6) = result.motor_speed.uint8._2;
	*(tx_buf+7) = result.motor_speed.uint8._3;
	*(tx_buf+8) = result.motor_dir.uint8._0;
	*(tx_buf+9) = result.motor_dir.uint8._1;
	*(tx_buf+10) = result.motor_dir.uint8._2;
	*(tx_buf+11) = result.motor_dir.uint8._3;
	*(tx_buf+12) = get_check_sum((tx_buf+1), *(tx_buf+1)-3);
	*(tx_buf+13) = 0x7D;
	*len = *(tx_buf+1);			
}


void read_detail(uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	*tx_buf = 0x7B;
	*(tx_buf+1) = 6+24;
	*(tx_buf+2) = 0x00;
	*(tx_buf+3) = *(rx_buf+3);
	*(tx_buf+4) = encoder_a[2]&0xFF;
	*(tx_buf+5) = (encoder_a[2]>>8)&0xFF;
	*(tx_buf+6) = (encoder_a[2]>>16)&0xFF;
	*(tx_buf+7) = (encoder_a[2]>>24)&0xFF;
	*(tx_buf+8) = encoder_a[3]&0xFF;
	*(tx_buf+9) = (encoder_a[3]>>8)&0xFF;
	*(tx_buf+10) = (encoder_a[3]>>16)&0xFF;
	*(tx_buf+11) = (encoder_a[3]>>24)&0xFF;
	*(tx_buf+12) = encoder_a[4]&0xFF;
	*(tx_buf+13) = (encoder_a[4]>>8)&0xFF;
	*(tx_buf+14) = (encoder_a[4]>>16)&0xFF;
	*(tx_buf+15) = (encoder_a[4]>>24)&0xFF;
	*(tx_buf+16) = encoder_b[2]&0xFF;
	*(tx_buf+17) = (encoder_b[2]>>8)&0xFF;
	*(tx_buf+18) = (encoder_b[2]>>16)&0xFF;
	*(tx_buf+19) = (encoder_b[2]>>24)&0xFF;
	*(tx_buf+20) = encoder_b[3]&0xFF;
	*(tx_buf+21) = (encoder_b[3]>>8)&0xFF;
	*(tx_buf+22) = (encoder_b[3]>>16)&0xFF;
	*(tx_buf+23) = (encoder_b[3]>>24)&0xFF;
	*(tx_buf+24) = encoder_b[4]&0xFF;
	*(tx_buf+25) = (encoder_b[4]>>8)&0xFF;
	*(tx_buf+26) = (encoder_b[4]>>16)&0xFF;
	*(tx_buf+27) = (encoder_b[4]>>24)&0xFF;
	*(tx_buf+28) = get_check_sum((tx_buf+1), *(tx_buf+1)-3);
	*(tx_buf+29) = 0x7D;
	*len = *(tx_buf+1);			
}


void set_filter_divider(uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	memcpy((void *)&sys_config, (void *)(rx_buf+4), sizeof(sys_config));
	AT45DB041E_Write(0x300, (uint8_t *)&sys_config, sizeof(sys_config));
		
	memcpy((void *)tx_buf, (void *)rx_buf, *(rx_buf+1));
	*len = *(tx_buf+1);
}



void read_coeff(uint8* rx_buf, uint8* tx_buf, uint32* len)
{
	*tx_buf = 0x7B;
	*(tx_buf+1) = 6 + sizeof(sys_coeff) + sizeof(sys_config);
	*(tx_buf+2) = 0x00;
	*(tx_buf+3) = *(rx_buf+3);
	memcpy((tx_buf+4),(uint8_t *)&sys_coeff, sizeof(sys_coeff));
	memcpy((tx_buf+4+sizeof(sys_coeff)),(uint8_t *)&sys_config, sizeof(sys_config));
	*(tx_buf+*(tx_buf+1)-2) = get_check_sum((tx_buf+1),*(tx_buf+1)-3);
	*(tx_buf+*(tx_buf+1)-1) = 0x7D;
	*len = *(tx_buf+1);			
}

void process_ack_frame(uint8* rx_buf, uint8* tx_buf)
{
	uint32 len;
	
	switch(rx_buf[3])
	{
		case CMD_START: 
							update_sys_status(CMD_START,rx_buf, tx_buf, &len);						
							break;
		case CMD_END:
							update_sys_status(CMD_END,rx_buf, tx_buf, &len);
							break;
		case CMD_RD_STATUS: 
							read_sys_status(rx_buf, tx_buf, &len);
							break;
		case CMD_RD_RESULT:
							read_result(rx_buf, tx_buf, &len);
							break;
		case CMD_RD_DETAIL:
							read_detail(rx_buf, tx_buf, &len);
							break;
		case CMD_SET: 
							set_filter_divider(rx_buf, tx_buf, &len);
							break;
		case CMD_RD_COEFF: 
							read_coeff(rx_buf, tx_buf, &len);
							break;
		default: break;		
	}
	usart_send_frame(1, tx_buf, len);
		
}

void send_version(void)
{
	usart_send_frame(1, (void *)version, sizeof(version));
}

/*-----------------------------------------------------------------------------------*/

/**
 * 下位机通讯任务.
 */
/*-----------------------------------------------------------------------------------*/
static portTASK_FUNCTION( uart_com_task, pvParameters )
{
	/* The parameters are not used. */
	( void ) pvParameters; 
	
	uint8 rx_buf[50];
	uint8 tx_buf[200];
	while(1)
	{	
		if(version_send == 0)
		{
			version_send = 1;
			send_version();			
		}
		
		if(1 == usart_recv_frame(1, rx_buf))
		{
			if(1 == validate_check_rx_data(rx_buf, tx_buf))
			{
				process_ack_frame(rx_buf, tx_buf);				
			}
		}
	}
}

/*-----------------------------------------------------------------------------------*/
/**
 * 启动下位通讯任务函数.
 *
 * 
 *
 */
/*-----------------------------------------------------------------------------------*/
void start_uart_com_task( unsigned portBASE_TYPE uxPriority )
{
    
	/* Spawn the task. */
	xTaskCreate( uart_com_task, ( const signed portCHAR * const ) "UART_COM_TASK", UART_COM_STACK_SIZE, NULL, uxPriority, ( xTaskHandle * ) NULL );
}
//end of file
