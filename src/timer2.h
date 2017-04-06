
#ifndef _TIMER2_H__
#define _TIMER2_H__

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f10x_conf.h"
#include "config.h"

void timer2_init( uint16_t time_period );

#endif


