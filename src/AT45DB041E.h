
#ifndef __AT45DB041E_H__
#define __AT45DB041E_H__

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "config.h"

void flash_gpio_init(void);
void init_at45db041e(void);
void AT45DB041E_Read(uint32_t addr, uint8_t *buf, uint32_t size);
void AT45DB041E_Write(uint32_t addr, uint8_t *buf, uint32_t size);


#endif

