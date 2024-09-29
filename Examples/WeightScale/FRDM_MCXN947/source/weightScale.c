/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    weightScale.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "NAFE113x8_SDK.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
static void waitReady();
static void doGpiosBlink();

static void delay(uint32_t ms);
static void loadCellWeightScale();
/*
 * @brief   Application entry point.
 */

int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
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

		PRINTF("\r\n\033[35mWeight Scale (Load Cell)\033[37m\r\n");

		PRINTF("\r\n********************\r\n");
		PRINTF("\r\nThree Step Procedure:\r\n");
		PRINTF("\r\nStep1: Offset Calculation\r\n");
		PRINTF("\r\nStep2: Calibration Coefficient Calculation\r\n");
		PRINTF("\r\nStep3: Actual Weight Calculation\r\n");
		PRINTF("\r\n********************\r\n");

		PRINTF("\r\nPress any key to start the procedure\r\n");
		GETCHAR();

		loadCellWeightScale();
	}
	return 0 ;
}

static void loadCellWeightScale(){

	int knownWeight;
	char key;

	float offset;
	float voltage;
	float coefficient;
	float weight;
	bool confirmation = true;

	// Ch0 Force/Sense Configuration
	NAFE113x8_WriteCommand(CMD_CH0,0); // Set Pointer to ch0

	// AI1P to AI1N - GAIN 16x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI1P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI1N);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,7);

	// SINC4+1 - 100Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,1);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,20);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,7);

	// Delay 16.5us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,1);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);

	// Voltage - Positive Pol - 6V - AI2P
	NAFE113x8_WriteField(&ChConfig3_ViexVI,0,0);
	NAFE113x8_WriteField(&ChConfig3_ViexPol,0,0);
	NAFE113x8_WriteField(&ChConfig3_ViexMag,0,13);
	NAFE113x8_WriteField(&ChConfig3_ViexAip,0,2);
	NAFE113x8_WriteField(&ChConfig3_ViexAin,0,0);
	NAFE113x8_WriteField(&ChConfig3_ViexChop,0,0);

	PRINTF("\r\n********************\r\n");
	PRINTF("\r\n-> Calculation of Offset <-\r\n");
	PRINTF("\r\n-> Step 1:Offset Calculation <-\r\n");
	PRINTF("\r\nLeave the weight scale without weight and click any buttons. \r\n");
	GETCHAR(); // Wait a keyboard import to start a conversion sequence.

	NAFE113x8_doSCCR(0,0,50);

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	voltage = 0;
	for(int i = 0; i<50; i++){
		voltage += NAFE113x8_VoltageTranslation(NAFE113x8_getSample(i),HV,0,16);
	}
	PRINTF("-----------------------------------");
	offset = voltage/50;
	PRINTF("\r\n-> Step 1: Done <-\r\n");
	PRINTF("\r\n********************\r\n");

	PRINTF("\r\n-> Step 2 : Calibration Coefficient Calculation <-\r\n");
	PRINTF("\r\nFor First Calculation Put Known Weight for Calibration\r\n");

	while (confirmation){
		PRINTF("\r\nPut an object of known weight on the scale\r\n",knownWeight);
		PRINTF("\r\nPlease, digit the weight on the scale and click enter\r\n");
		SCANF("%i", &knownWeight); // Wait a keyboard import to start a conversion sequence.
		PRINTF("\r\nThe weight on the scale is %dg, do you confirm(y/n)?\r\n", knownWeight);
		key = GETCHAR();
		if (key == 'y'){confirmation = false;}
		else{confirmation = true;}
	}

	NAFE113x8_WriteCommand(CMD_CH0,0);
	NAFE113x8_doSCCR(0,0,50);
	voltage = 0;
	for(int i = 0; i<50; i++){
		voltage += NAFE113x8_VoltageTranslation(NAFE113x8_getSample(i),HV,0,16);
	}
	PRINTF("-----------------------------------");
	voltage = voltage/50;
	coefficient = knownWeight / (voltage - offset);

	PRINTF("\r\n-> Step 2: Done <-\r\n");
	PRINTF("\r\n********************\r\n");

	while(1)
	{
		PRINTF("\r\n->Click any button to start one Conversion sequences<-\r\n");
		GETCHAR(); // Wait a keyboard import to start a conversion sequence.

		NAFE113x8_WriteCommand(CMD_CH0,0);
		NAFE113x8_doSCCR(0,0,50);
		voltage = 0; // Initialize result variable
		for(int i = 0; i<50; i++){
			voltage += NAFE113x8_VoltageTranslation(NAFE113x8_getSample(i),HV,0,16);
		}
		voltage = voltage/50;
		weight = coefficient*(voltage-offset);
		PRINTF("\r\n\033[32mVoltage Load Cell=%6fmV, Weight=%1fg\033[37m\r\n", voltage*1000,weight);
		PRINTF("\r\n********************\r\n");

		PRINTF("\r\nPut any item to calculate its weight \r\n");

	}
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
