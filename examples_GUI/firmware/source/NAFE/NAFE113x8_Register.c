/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include "NAFE113x8_Register.h"


const Register_t ChConfig0    = {0x20,2};
const Register_t ChConfig1    = {0x21,2};
const Register_t ChConfig2    = {0x22,2};
const Register_t ChConfig3    = {0x23,2};
const Register_t ChConfig4    = {0x24,2};

const Register_t ChConfig5_0  = {0x50,3};
const Register_t ChConfig5_1  = {0x51,3};
const Register_t ChConfig5_2  = {0x52,3};
const Register_t ChConfig5_3  = {0x53,3};
const Register_t ChConfig5_4  = {0x54,3};
const Register_t ChConfig5_5  = {0x55,3};
const Register_t ChConfig5_6  = {0x56,3};
const Register_t ChConfig5_7  = {0x57,3};
const Register_t ChConfig5_8  = {0x58,3};
const Register_t ChConfig5_9  = {0x59,3};
const Register_t ChConfig5_10 = {0x5A,3};
const Register_t ChConfig5_11 = {0x5B,3};
const Register_t ChConfig5_12 = {0x5C,3};
const Register_t ChConfig5_13 = {0x5D,3};
const Register_t ChConfig5_14 = {0x5E,3};
const Register_t ChConfig5_15 = {0x5F,3};

const Register_t ChConfig6_0  = {0x60,3};
const Register_t ChConfig6_1  = {0x61,3};
const Register_t ChConfig6_2  = {0x62,3};
const Register_t ChConfig6_3  = {0x63,3};
const Register_t ChConfig6_4  = {0x64,3};
const Register_t ChConfig6_5  = {0x65,3};
const Register_t ChConfig6_6  = {0x66,3};
const Register_t ChConfig6_7  = {0x67,3};
const Register_t ChConfig6_8  = {0x68,3};
const Register_t ChConfig6_9  = {0x69,3};
const Register_t ChConfig6_10 = {0x6A,3};
const Register_t ChConfig6_11 = {0x6B,3};
const Register_t ChConfig6_12 = {0x6C,3};
const Register_t ChConfig6_13 = {0x6D,3};
const Register_t ChConfig6_14 = {0x6E,3};
const Register_t ChConfig6_15 = {0x6F,3};

const Register_t ChData0      = {0x40,3};
const Register_t ChData1      = {0x41,3};
const Register_t ChData2      = {0x42,3};
const Register_t ChData3      = {0x43,3};
const Register_t ChData4      = {0x44,3};
const Register_t ChData5      = {0x45,3};
const Register_t ChData6      = {0x46,3};
const Register_t ChData7      = {0x47,3};
const Register_t ChData8      = {0x48,3};
const Register_t ChData9      = {0x49,3};
const Register_t ChData10     = {0x4A,3};
const Register_t ChData11     = {0x4B,3};
const Register_t ChData12     = {0x4C,3};
const Register_t ChData13     = {0x4D,3};
const Register_t ChData14     = {0x4E,3};
const Register_t ChData15     = {0x4F,3};

const Register_t GainCoeff0   = {0x80,3};
const Register_t GainCoeff1   = {0x81,3};
const Register_t GainCoeff2   = {0x82,3};
const Register_t GainCoeff3   = {0x83,3};
const Register_t GainCoeff4   = {0x84,3};
const Register_t GainCoeff5   = {0x85,3};
const Register_t GainCoeff6   = {0x86,3};
const Register_t GainCoeff7   = {0x87,3};
const Register_t GainCoeff8   = {0x88,3};
const Register_t GainCoeff9   = {0x89,3};
const Register_t GainCoeff10  = {0x8A,3};
const Register_t GainCoeff11  = {0x8B,3};
const Register_t GainCoeff12  = {0x8C,3};
const Register_t GainCoeff13  = {0x8D,3};
const Register_t GainCoeff14  = {0x8E,3};
const Register_t GainCoeff15  = {0x8F,3};

const Register_t OffCoeff0   = {0x90,3};
const Register_t OffCoeff1   = {0x91,3};
const Register_t OffCoeff2   = {0x92,3};
const Register_t OffCoeff3   = {0x93,3};
const Register_t OffCoeff4   = {0x94,3};
const Register_t OffCoeff5   = {0x95,3};
const Register_t OffCoeff6   = {0x96,3};
const Register_t OffCoeff7   = {0x97,3};
const Register_t OffCoeff8   = {0x98,3};
const Register_t OffCoeff9   = {0x99,3};
const Register_t OffCoeff10  = {0x9A,3};
const Register_t OffCoeff11  = {0x9B,3};
const Register_t OffCoeff12  = {0x9C,3};
const Register_t OffCoeff13  = {0x9D,3};
const Register_t OffCoeff14  = {0x9E,3};
const Register_t OffCoeff15  = {0x9F,3};

const Register_t OptCoeff0   = {0xA0,3};
const Register_t OptCoeff1   = {0xA1,3};
const Register_t OptCoeff2   = {0xA2,3};
const Register_t OptCoeff3   = {0xA3,3};
const Register_t OptCoeff4   = {0xA4,3};
const Register_t OptCoeff5   = {0xA5,3};
const Register_t OptCoeff6   = {0xA6,3};
const Register_t OptCoeff7   = {0xA7,3};
const Register_t OptCoeff8   = {0xA8,3};
const Register_t OptCoeff9   = {0xA9,3};
const Register_t OptCoeff10  = {0xAA,3};
const Register_t OptCoeff11  = {0xAB,3};
const Register_t OptCoeff12  = {0xAC,3};
const Register_t OptCoeff13  = {0xAD,3};

const Register_t SysConfig0   = {0x30,2};
const Register_t SysStatus0   = {0x31,2};
const Register_t GlobalAlarmE = {0x32,2};
const Register_t GlobalAlarmI = {0x33,2};

const Register_t GpiData      = {0x29,2};
const Register_t GpoData      = {0x2F,2};
const Register_t GpioConfig0  = {0x2A,2};
const Register_t GpioConfig1  = {0x2B,2};
const Register_t GpioConfig2  = {0x2C,2};
const Register_t GpiEdgePos   = {0x2D,2};
const Register_t GpiEdgeNeg   = {0x2E,2};

const Register_t PN0          = {0x7E,2};
const Register_t PN1          = {0x7D,2};
const Register_t PN2          = {0x7C,2};

//ChConfig0 Fields
const Field_t ChConfig0_TccOff           = {&ChConfig0,0,0x1,ReadWrite};
const Field_t ChConfig0_LvSignIn         = {&ChConfig0,1,0x7,ReadWrite};
const Field_t ChConfig0_HvSel            = {&ChConfig0,4,0x1,ReadWrite};
const Field_t ChConfig0_ChGain           = {&ChConfig0,5,0x7,ReadWrite};
const Field_t ChConfig0_HvAin            = {&ChConfig0,8,0xF,ReadWrite};
const Field_t ChConfig0_HvAip            = {&ChConfig0,12,0xF,ReadWrite};

//ChConfig1 Fields
const Field_t ChConfig1_AdcSinc          = {&ChConfig1,0,0x7,ReadWrite};
const Field_t ChConfig1_AdcDataRate      = {&ChConfig1,3,0x1F,ReadWrite};
const Field_t ChConfig1_ChThrs           = {&ChConfig1,8,0xF,ReadWrite};
const Field_t ChConfig1_GainOffPntr      = {&ChConfig1,12,0xF,ReadWrite};

//ChConfig2 Fields
const Field_t ChConfig2_ChChop           = {&ChConfig2,7,0x1,ReadWrite};
const Field_t ChConfig2_AdcFilterRst     = {&ChConfig2,8,0x1,ReadWrite};
const Field_t ChConfig2_AdcNormalSet     = {&ChConfig2,9,0x1,ReadWrite};
const Field_t ChConfig2_ChDelay          = {&ChConfig2,10,0x3F,ReadWrite};

//ChConfig3 Fields
const Field_t ChConfig3_ViexVI           = {&ChConfig3,15,0x1,ReadWrite};
const Field_t ChConfig3_ViexPol          = {&ChConfig3,14,0x1,ReadWrite};
const Field_t ChConfig3_ViexMag          = {&ChConfig3,10,0xF,ReadWrite};
const Field_t ChConfig3_VexEn            = {&ChConfig3,9,0x1,ReadWrite};
const Field_t ChConfig3_Odc              = {&ChConfig3,8,0x1,ReadWrite};
const Field_t ChConfig3_ViexChop         = {&ChConfig3,6,0x1,ReadWrite};
const Field_t ChConfig3_ViexAip          = {&ChConfig3,3,0x7,ReadWrite};
const Field_t ChConfig3_ViexAin          = {&ChConfig3,0,0x7,ReadWrite};

//SysConfig0 Fields
const Field_t SysConfig0_CrcErrortoGPIO2 = {&SysConfig0,0,0x1,ReadWrite};
const Field_t SysConfig0_IntbDriverType  = {&SysConfig0,1,0x1,ReadWrite};
const Field_t SysConfig0_SpiDoutDrive    = {&SysConfig0,2,0x1,ReadWrite};
const Field_t SysConfig0_GaiSticky       = {&SysConfig0,3,0x1,ReadWrite};
const Field_t SysConfig0_DrdyPinEdge     = {&SysConfig0,4,0x1,ReadWrite};
const Field_t SysConfig0_AdcSync         = {&SysConfig0,5,0x1,ReadWrite};
const Field_t SysConfig0_StatusEn        = {&SysConfig0,6,0x1,ReadWrite};
const Field_t SysConfig0_CrcEn           = {&SysConfig0,7,0x1,ReadWrite};
const Field_t SysConfig0_CkSrcSel        = {&SysConfig0,8,0x3,ReadWrite};
const Field_t SysConfig0_RefSel          = {&SysConfig0,10,0x3,ReadWrite};
const Field_t SysConfig0_MclkOutEn       = {&SysConfig0,12,0x1,ReadWrite};
const Field_t SysConfig0_StatusSticky    = {&SysConfig0,13,0x1,ReadWrite};
const Field_t SysConfig0_AdcDataOut16    = {&SysConfig0,14,0x1,ReadWrite};
const Field_t SysConfig0_DrdyPwdt        = {&SysConfig0,15,0x1,ReadWrite};

//SysStatus Fields
const Field_t SysStatus0_AdcConvCh       = {&SysStatus0,4,0xF,Read};
const Field_t SysStatus0_CfgChPtr        = {&SysStatus0,8,0xF,Read};
const Field_t SysStatus0_CrcError        = {&SysStatus0,12,0x1,Read};
const Field_t SysStatus0_ChipReady       = {&SysStatus0,13,0x1,Read};
const Field_t SysStatus0_MCActive        = {&SysStatus0,14,0x1,Read};
const Field_t SysStatus0_SCActive        = {&SysStatus0,15,0x1,Read};
