#include <stdint.h>

#ifndef		__GLOBALFUNC_H__
#define		__GLOBALFUNC_H__

uint8_t SPISendReceiveByte(uint8_t dat);// __attribute__((always_inline))

void SetCurrentGain(uint8_t ch, uint8_t g);
void SetVoltageGain(uint8_t g);
void ReadCurrent(void);
void ReadVoltage(void);
void CS5460A_ClearSR(void);
void receive_control(void *buf);
void receive_coeff(void *buf);
void CS5460_Start(void);
void init_cs5460a(void);

#endif
