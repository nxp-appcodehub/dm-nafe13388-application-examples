/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    TwoWireRTD.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "systick_utils.h"
#include "NAFE113x8_SDK.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
static void waitReady();
static void doGpiosBlink();

static void delay(uint32_t ms);
static uint8_t twoWireRTD();
/*
 * @brief   Application entry point.
 */

int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_SystickEnable();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    NAFE113x8_InitNafe();

    NAFE113x8_WriteCommand(CMD_RESET,0);

    delay(10);
    waitReady();
    doGpiosBlink();

    PRINTF("\033[0;36m");
	PRINTF("----------------WELCOME TO NAFE MCXN DEMO---------------\r\n");
	PRINTF("\033[0m");
    while(1) {

		PRINTF("\r\n\033[35mTwo Wire RTD\033[37m\r\n");
		twoWireRTD();

    }
    return 0 ;
}

static uint8_t twoWireRTD(){

	const float R0=100;
	const float A=0.003908;
	const float ISource=0.000765;

    	float voltage;
    	float voltageSum;

	float temperatureResult;
	int status;

	// RTD Sensing and Forcing
	NAFE113x8_WriteCommand(CMD_CH0,0); // Set Pointer to ch0

	// AI1P to AICOM - GAIN 0.8x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI1P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AICOM);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,4);

	// SINC4+1 - 562.50Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,1);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,16);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,4);

	// Delay 16.5us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,1);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);

	// Current - Positive Pol - 750uA - AI1P
	NAFE113x8_WriteField(&ChConfig3_ViexVI,0,1);
	NAFE113x8_WriteField(&ChConfig3_ViexPol,0,0);
	NAFE113x8_WriteField(&ChConfig3_ViexMag,0,12);
	NAFE113x8_WriteField(&ChConfig3_ViexAip,0,1);
	NAFE113x8_WriteField(&ChConfig3_ViexAin,0,0);
	NAFE113x8_WriteField(&ChConfig3_ViexChop,0,0);

	status = NAFE113x8_doSCCR(0,0,20);
	if(status)
		return TIMEOUT;

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	PRINTF("\033[0;32m");
	PRINTF("\r\nVoltage\t\t\t\tTemperature \r\n");
	PRINTF("\033[0m");

	voltageSum = 0;
	for(int i = 0; i<20; i++){
		voltage = NAFE113x8_VoltageTranslation(NAFE113x8_getSample(i),HV,0,2);
		voltageSum += voltage;
		PRINTF("Sample %d: Voltage RTD=%6fV, Temperature RTD=%3fC\r\n", i,voltage,((voltage/ISource)-(R0+2398))/(A*R0));

	}
	PRINTF("-----------------------------------");
	voltage = voltageSum/20;
	temperatureResult = ((voltage/ISource)-(R0+2398))/(A*R0);

	PRINTF("\r\n------------Average Conversion-------------");
	PRINTF("\r\n\033[32mVoltage RTD=%6fV, Temperature RTD=%3fC\033[37m", voltage,temperatureResult);
	PRINTF("\r\n---------------------------\r\n");
	return 0;
}

static void waitReady(){
	uint16_t ChipReady=0;
	NAFE113x8_ReadField(&SysStatus0_ChipReady,0,&ChipReady);
	while (ChipReady==0)
		NAFE113x8_ReadField(&SysStatus0_ChipReady,0,&ChipReady);
}

static void doGpiosBlink(){
    NAFE113x8_WriteRegister(&GpioConfig0,0,0b1111111111<<6);
    NAFE113x8_WriteRegister(&GpioConfig1,0,0b1111111111<<6);
	for(int i=0; i<5; i++){
		NAFE113x8_WriteRegister(&GpoData,0,GPIO9_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO0_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO2_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO1_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO3_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO4_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO5_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO6_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO7_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO8_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO8_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO7_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO6_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO5_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO4_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO3_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO1_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO2_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO0_MASK);
		delay(10);
		NAFE113x8_WriteRegister(&GpoData,0,GPIO8_MASK);
	}
	for(int i=0; i<5; i++){
		delay(100);
		NAFE113x8_WriteRegister(&GpoData,0,0b1111111111<<6);
		delay(100);
		NAFE113x8_WriteRegister(&GpoData,0,0);
	}

    NAFE113x8_WriteRegister(&GpioConfig0,0,0);
    NAFE113x8_WriteRegister(&GpioConfig1,0,0);
}

static void delay(uint32_t ms)
{
    volatile int i,j;
    for(i=0; i<ms; i++)
    {
        for(j=0; j<15000; j++)
        {
            __NOP();
        }
    }
}





