/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NAFE113X8_REGISTER_H_
#define NAFE113X8_REGISTER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Register{
	uint16_t Address;
	uint8_t Bytes;
}Register_t;

typedef struct Field{
	const Register_t *refRegister;
	uint8_t Offset;
	uint16_t mask;
	uint8_t r_w;
}Field_t;

enum RegisterAcces{
	Write     = 0,
	Read      = 1,
	ReadWrite = 2,
};

enum CommonCommandID{
	CMD_CLEAR_ALARM = (uint16_t)0x0012,
	CMD_RESET       = (uint16_t)0x0014,
	CMD_CLEAR_REG   = (uint16_t)0x0015,
	CMD_RELOAD      = (uint16_t)0x0016
};

enum ADCCommandID{
	CMD_CH0        = (uint16_t)0x0000,
	CMD_CH1        = (uint16_t)0x0001,
	CMD_CH2		   = (uint16_t)0x0002,
	CMD_CH3        = (uint16_t)0x0003,
	CMD_CH4        = (uint16_t)0x0004,
	CMD_CH5        = (uint16_t)0x0005,
	CMD_CH6        = (uint16_t)0x0006,
	CMD_CH7        = (uint16_t)0x0007,
	CMD_CH8        = (uint16_t)0x0008,
	CMD_CH9        = (uint16_t)0x0009,
	CMD_CH10	   = (uint16_t)0x000A,
	CMD_CH11       = (uint16_t)0x000B,
	CMD_CH12       = (uint16_t)0x000C,
	CMD_CH13       = (uint16_t)0x000D,
	CMD_CH14       = (uint16_t)0x000E,
	CMD_CH15       = (uint16_t)0x000F,
	CMD_ADC_ABORT  = (uint16_t)0x0010,
	CMD_ADC_END    = (uint16_t)0x0011,
	CMD_CLEAR_DATA = (uint16_t)0x0013,
	CMD_SS         = (uint16_t)0x2000,
	CMD_SC         = (uint16_t)0x2001,
	CMD_MM         = (uint16_t)0x2002,
	CMD_MC         = (uint16_t)0x2003,
	CMD_MS         = (uint16_t)0x2004
};

extern const Register_t ChConfig0;
extern const Register_t ChConfig1;
extern const Register_t ChConfig2;
extern const Register_t ChConfig3;
extern const Register_t ChConfig4;

extern const Register_t ChConfig5_0;
extern const Register_t ChConfig5_1;
extern const Register_t ChConfig5_2;
extern const Register_t ChConfig5_3;
extern const Register_t ChConfig5_4;
extern const Register_t ChConfig5_5;
extern const Register_t ChConfig5_6;
extern const Register_t ChConfig5_7;
extern const Register_t ChConfig5_8;
extern const Register_t ChConfig5_9;
extern const Register_t ChConfig5_10;
extern const Register_t ChConfig5_11;
extern const Register_t ChConfig5_12;
extern const Register_t ChConfig5_13;
extern const Register_t ChConfig5_14;
extern const Register_t ChConfig5_15;

extern const Register_t ChConfig6_0;
extern const Register_t ChConfig6_1;
extern const Register_t ChConfig6_2;
extern const Register_t ChConfig6_3;
extern const Register_t ChConfig6_4;
extern const Register_t ChConfig6_5;
extern const Register_t ChConfig6_6;
extern const Register_t ChConfig6_7;
extern const Register_t ChConfig6_8;
extern const Register_t ChConfig6_9;
extern const Register_t ChConfig6_10;
extern const Register_t ChConfig6_11;
extern const Register_t ChConfig6_12;
extern const Register_t ChConfig6_13;
extern const Register_t ChConfig6_14;
extern const Register_t ChConfig6_15;

extern const Register_t ChData0;
extern const Register_t ChData1;
extern const Register_t ChData2;
extern const Register_t ChData3;
extern const Register_t ChData4;
extern const Register_t ChData5;
extern const Register_t ChData6;
extern const Register_t ChData7;
extern const Register_t ChData8;
extern const Register_t ChData9;
extern const Register_t ChData10;
extern const Register_t ChData11;
extern const Register_t ChData12;
extern const Register_t ChData13;
extern const Register_t ChData14;
extern const Register_t ChData15;

extern const Register_t GainCoeff0;
extern const Register_t GainCoeff1;
extern const Register_t GainCoeff2;
extern const Register_t GainCoeff3;
extern const Register_t GainCoeff4;
extern const Register_t GainCoeff5;
extern const Register_t GainCoeff6;
extern const Register_t GainCoeff7;
extern const Register_t GainCoeff8;
extern const Register_t GainCoeff9;
extern const Register_t GainCoeff10;
extern const Register_t GainCoeff11;
extern const Register_t GainCoeff12;
extern const Register_t GainCoeff13;
extern const Register_t GainCoeff14;
extern const Register_t GainCoeff15;

extern const Register_t OffCoeff0;
extern const Register_t OffCoeff1;
extern const Register_t OffCoeff2;
extern const Register_t OffCoeff3;
extern const Register_t OffCoeff4;
extern const Register_t OffCoeff5;
extern const Register_t OffCoeff6;
extern const Register_t OffCoeff7;
extern const Register_t OffCoeff8;
extern const Register_t OffCoeff9;
extern const Register_t OffCoeff10;
extern const Register_t OffCoeff11;
extern const Register_t OffCoeff12;
extern const Register_t OffCoeff13;
extern const Register_t OffCoeff14;
extern const Register_t OffCoeff15;

extern const Register_t OptCoeff0;
extern const Register_t OptCoeff1;
extern const Register_t OptCoeff2;
extern const Register_t OptCoeff3;
extern const Register_t OptCoeff4;
extern const Register_t OptCoeff5;
extern const Register_t OptCoeff6;
extern const Register_t OptCoeff7;
extern const Register_t OptCoeff8;
extern const Register_t OptCoeff9;
extern const Register_t OptCoeff10;
extern const Register_t OptCoeff11;
extern const Register_t OptCoeff12;
extern const Register_t OptCoeff13;

extern const Register_t SysConfig0;
extern const Register_t SysStatus0;
extern const Register_t GlobalAlarmE;
extern const Register_t GlobalAlarmI;

extern const Register_t GpiData;
extern const Register_t GpoData;
extern const Register_t GpioConfig0;
extern const Register_t GpioConfig1;
extern const Register_t GpioConfig2;
extern const Register_t GpiEdgePos;
extern const Register_t GpiEdgeNeg;

extern const Register_t PN0;
extern const Register_t PN1;
extern const Register_t PN2;

//ChConfig0 Fields
extern const Field_t ChConfig0_TccOff;
extern const Field_t ChConfig0_LvSignIn;
extern const Field_t ChConfig0_HvSel;
extern const Field_t ChConfig0_ChGain;
extern const Field_t ChConfig0_HvAin;
extern const Field_t ChConfig0_HvAip;

//ChConfig1 Fields
extern const Field_t ChConfig1_AdcSinc;
extern const Field_t ChConfig1_AdcDataRate;
extern const Field_t ChConfig1_ChThrs;
extern const Field_t ChConfig1_GainOffPntr;

//ChConfig2 Fields
extern const Field_t ChConfig2_ChChop;
extern const Field_t ChConfig2_AdcFilterRst;
extern const Field_t ChConfig2_AdcNormalSet;
extern const Field_t ChConfig2_ChDelay;

//ChConfig3 Fields
extern const Field_t ChConfig3_ViexVI;
extern const Field_t ChConfig3_ViexPol;
extern const Field_t ChConfig3_ViexMag;
extern const Field_t ChConfig3_VexEn;
extern const Field_t ChConfig3_Odc;
extern const Field_t ChConfig3_ViexChop;
extern const Field_t ChConfig3_ViexAip;
extern const Field_t ChConfig3_ViexAin;

//SysConfig0 Fields
extern const Field_t SysConfig0_CrcErrortoGPIO2;
extern const Field_t SysConfig0_IntbDriverType;
extern const Field_t SysConfig0_SpiDoutDrive;
extern const Field_t SysConfig0_GaiSticky;
extern const Field_t SysConfig0_DrdyPinEdge;
extern const Field_t SysConfig0_AdcSync;
extern const Field_t SysConfig0_StatusEn;
extern const Field_t SysConfig0_CrcEn;
extern const Field_t SysConfig0_CkSrcSel;
extern const Field_t SysConfig0_RefSel;
extern const Field_t SysConfig0_MclkOutEn;
extern const Field_t SysConfig0_StatusSticky;
extern const Field_t SysConfig0_AdcDataOut16;
extern const Field_t SysConfig0_DrdyPwdt;

//SysStatus Fields
extern const Field_t SysStatus0_AdcConvCh;
extern const Field_t SysStatus0_CfgChPtr;
extern const Field_t SysStatus0_CrcError;
extern const Field_t SysStatus0_ChipReady;
extern const Field_t SysStatus0_MCActive;
extern const Field_t SysStatus0_SCActive;

#define DRDYPINEDGE_EVERYCH 0
#define DRDYPINEDGE_LASTCH 1

#define HVAIP_GND 0
#define HVAIP_AI1P 1
#define HVAIP_AI2P 2
#define HVAIP_AI3P 3
#define HVAIP_AI4P 4
#define HVAIP_REFH 5
#define HVAIP_REFL 6
#define HVAIP_AICOM 7

#define HVAIN_GND 0
#define HVAIN_AI1N 1
#define HVAIN_AI2N 2
#define HVAIN_AI3N 3
#define HVAIN_AI4N 4
#define HVAIN_REFH 5
#define HVAIN_REFL 6
#define HVAIN_AICOM 7

#define LVSINGIN_REF2 0
#define LVSINGIN_GPIO01 1
#define LVSINGIN_VADD 3
#define LVSINGIN_VHDD 4
#define LVSINGIN_VHSS 5

#define GPIO9_MASK 0b1000000000000000
#define GPIO8_MASK 0b0100000000000000
#define GPIO7_MASK 0b0010000000000000
#define GPIO6_MASK 0b0001000000000000
#define GPIO5_MASK 0b0000100000000000
#define GPIO4_MASK 0b0000010000000000
#define GPIO3_MASK 0b0000001000000000
#define GPIO2_MASK 0b0000000100000000
#define GPIO1_MASK 0b0000000010000000
#define GPIO0_MASK 0b0000000001000000



#endif /* NAFE113X8_REGISTER_H_ */
