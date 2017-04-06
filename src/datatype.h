#include <stdint.h>

#ifndef		__DATATYPE_H__
#define		__DATATYPE_H__

typedef	struct	__SerialOut_t
{
	volatile uint8_t	length;
	volatile uint8_t	status;
	volatile uint8_t*	ptr;
}SerialOut, *LPSerialOut;

typedef struct	__SerialIn_t
{
	volatile uint8_t	count;
	volatile uint8_t	reserved;
	volatile uint8_t	buf[64];
}SerialIn, *LPSerialIn;

typedef	struct	__Status_t
{
	volatile uint8_t	Vauto;
	volatile uint8_t	Vsel;
	volatile uint8_t	Iauto;
	volatile uint8_t	Isel;
}Status, *LPStatus;

typedef	struct	__Result_t
{
	Status	StatusReg;
	volatile int32_t	BCVoltage;
	volatile int32_t	BCCurrent;
	volatile int32_t	Voltage;
	volatile int32_t	Current;
	volatile int32_t	Power;
}Result, *LPResult;

typedef	struct	__Coefficient_t
{
	volatile int32_t	k;
	volatile int32_t	b;
}Coefficient, *LPCoefficient;

typedef	struct	__SysCoeff_t
{
	Coefficient	Voltage[3];
	Coefficient Current[3];
}SysCoeff, *LPSysCoeff;

typedef	struct	__Setting_t
{
	volatile uint8_t	VChnMax;
	volatile uint8_t	IChnMax;
	volatile uint8_t	VGain;
	volatile uint8_t	IGain;
}Setting, *LPSetting;

typedef	struct	__Control_t
{
	Status	StatusReg;
	Setting	SettingReg;
}Control, *LPControl;

typedef	struct	__Communication_t
{
	volatile uint32_t	BaudRate;
}Communication, *LPCommunication;



#endif
