
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "config.h"
//#include "platform_config.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"
#include "public.h"
#include "user_api.h"
#include "serial.h"
#include "uart_com.h"
#include "AT45DB041E.h"
#include "led.h"
#include "globalfunc.h"
#include "motor_test.h"
#include "timer2.h"
#include <string.h> 

//#define TEST 
#ifdef TEST
void start_test_task( unsigned portBASE_TYPE uxPriority );
#endif

SYS_CONFIG sys_config;
RESULT result;
SYS_STATUS sys_status;
CALIBRATION sys_coeff;
extern volatile uint8 version_send;
const uint8 version[40]="AIP DC PARAMETER V1.02 2017.02.14";

void NVIC_PriorityGroupConfiguration(void);
void module_init(void);
void iwdg_init(void);
void version_init(void);

int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
    */        
    /* System Priority Group Configuration */
//  SCB->VTOR = FLASH_BASE | 0x4000;    
		NVIC_PriorityGroupConfiguration();
    module_init();   

#ifdef TEST//#if TEST==1
    start_test_task(TEST_PRIORITY);
#else
		start_uart_com_task( UART_COM_PRIORITY );		
#endif   
    /* Now all the tasks have been started - start the scheduler.

		NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
		The processor MUST be in supervisor mode when vTaskStartScheduler is 
		called.  The demo applications included in the FreeRTOS.org download switch
		to supervisor mode prior to main being called.  If you are not using one of
		these demo application projects then ensure Supervisor mode is used here. */
		vTaskStartScheduler();
    
    return 0;
}

void version_init(void)
{

	uint8 versioncheck[100];
	AT45DB041E_Read(0, (uint8_t*)versioncheck, sizeof(version));
	AT45DB041E_Read(0x100, (uint8_t*)&sys_coeff, sizeof(sys_coeff));
	AT45DB041E_Read(0x300,(uint8_t *)&sys_config, sizeof(sys_config));

	if(strcmp((void *)version, (void *)versioncheck))
	{
		AT45DB041E_Write(0, (uint8_t*)version, sizeof(version));
		delay_ms(10);//indispensible delay
	
		sys_coeff.V_rms_coeff_k = 1;//0x54f4;//0x52EA;//0x5474;//V
		sys_coeff.V_rms_coeff_b = 0;//0x356A8;
		sys_coeff.V_ripple_coeff_k = 1;//0x5606;
		sys_coeff.V_ripple_coeff_b = 0;//0x2094;		
		sys_coeff.I_rms_coeff_k[0] = 1;//0xf7325;//0xee607;//0x1118EE;//5A
		sys_coeff.I_rms_coeff_b[0] = 0;//0x2ca51;
		sys_coeff.I_ripple_coeff_k[0] = 1;//0xf90c3;
		sys_coeff.I_ripple_coeff_b[0] = 0;//0x2094;
		sys_coeff.I_rms_coeff_k[1] = 1;//0x1751c;//0x1B5B1;//50A
		sys_coeff.I_rms_coeff_b[1] = 0;//0x16AFA;
		sys_coeff.I_ripple_coeff_k[1] = 1;//0x188c8;
		sys_coeff.I_ripple_coeff_b[1] = 0;//0x2094;		
		sys_coeff.I_rms_coeff_k[2] = 1;//0xc2f7;//0XDAD8;//100A
		sys_coeff.I_rms_coeff_b[2] = 0;//0x1768;
		sys_coeff.I_ripple_coeff_k[2] = 1;//0xc44c;
		sys_coeff.I_ripple_coeff_b[2] = 0;//0x2094;			
		AT45DB041E_Write(0x100, (uint8_t*)&sys_coeff, sizeof(sys_coeff));
		delay_ms(10);//indispensible delay
				
		sys_config.io_set.uint8 = 0x30;
		sys_config.sample_cnt = 1000;
		AT45DB041E_Write(0x300,(uint8_t *)&sys_config, sizeof(sys_config));
 		delay_ms(10);//indispensible delay	
	}
}

/**
  * @brief  init module 
  * @param  None
  * @retval None
  */
void module_init(void)
{
		led_init();
		timer2_init(50);
#if WATCH_DOG==1
    iwdg_init();
#endif
    usart_init(1, 4800);
		flash_gpio_init();
		init_at45db041e();
		version_init();

		sys_status.bits.sys_state = IDLE;	
		version_send = 0;
}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void NVIC_PriorityGroupConfiguration(void)
{
     NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); /* group 4*/
}

/**
  * @brief  idle task hook.
  * @param  None
  * @retval None
  */
void vApplicationIdleHook( void )
{
    portTickType tick;
    static portTickType prev_tick = 0;
#if WATCH_DOG==1
    IWDG_ReloadCounter();
#endif    
    tick = xTaskGetTickCount();
    
    if(get_elapsed_ticks(prev_tick, tick )>1000/portTICK_RATE_MS)
    {
//			toggle_run_led();
      prev_tick = xTaskGetTickCount();
    }
}

#ifdef TEST//#if TEST==1

static portTASK_FUNCTION( test_task, pvParameters )
{
    while(1)
    {    		 
//    vTaskDelay(500/portTICK_RATE_MS);
//		set_run_led_on_core();
			vTaskDelay(2/portTICK_RATE_MS);
//		set_run_led_off_core();
    }    
}

void start_test_task( unsigned portBASE_TYPE uxPriority )
{    
	/* Spawn the task. */
	xTaskCreate( test_task, ( const signed portCHAR * const ) "TEST", TEST_STACK_SIZE, NULL, uxPriority, ( xTaskHandle * ) NULL );
}


#endif


void iwdg_init(void)
{
    /* 写入0x5555,用于允许狗狗寄存器写入功能 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    /* 喂狗时间 20s/6.4MS=3125 4s/6.4MS=625.注意不能大于0xfff*/
    IWDG_SetReload(3125);

    /* 喂狗*/
    IWDG_ReloadCounter();

    /* 使能狗狗*/
    IWDG_Enable();
}

