/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    VoltageSensingSCSR.c
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
static uint32_t tic(void);
static uint32_t toc(uint32_t tic_ticks);
static void delay(uint32_t ms);
static void VSenseSCSR();
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

		PRINTF("\r\n\033[35mVoltage Sensing using Single Channel Single Reading Conversion\033[37m\r\n"); // Single Channel Single Reading Conversion
		VSenseSCSR();

	}
	return 0 ;
}

static void VSenseSCSR(){

	uint32_t regValue;

	GPIO_SetPinInterruptConfig(BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_GPIO, BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_PIN, kGPIO_InterruptRisingEdge);
	EnableIRQ(GPIO00_IRQn);

	float conversionResult;
	// Gain settings for each logical channel
	float gains[7] = {0.2,0.4,0.2,0.4,0.4,2,16};

	uint32_t ticks[7];
	float time;
	float time_tot;
	time_tot = 0;

	Register_t temp;
	uint16_t offset;

	//Channel 0 AI1P-AICOM
	NAFE113x8_WriteCommand(CMD_CH0,0); // Set Pointer to ch0

	// AI1P to AICOM - GAIN 0.2x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI1P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AICOM);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,0);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 12000Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,5);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,0);

	// Delay 16.493us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);


	// Channel 1 AI1N-AICOM
	NAFE113x8_WriteCommand(CMD_CH1,0); // Set Pointer to ch1

	// AI1N to AICOM - GAIN 0.4x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AICOM);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI1N);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,1);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 12000Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,5);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,0);

	// Delay 16.493us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);


	// Channel 2 AI2P-AICOM
	NAFE113x8_WriteCommand(CMD_CH2,0); // Set Pointer to ch2

	// AI2P to AICOM - GAIN 0.2x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI2P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AICOM);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,0);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 24000Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,3);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,0);

	// Delay 16.493us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);


	// Channel 3 AI1N-AICOM
	NAFE113x8_WriteCommand(CMD_CH3,0); // Set Pointer to ch3

	// AI1N to AICOM - GAIN 0.4x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AICOM);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI1N);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,1);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 24000Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,3);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,1);

	// Delay 16.493us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);

	// Channel 4 AI3P-AICOM
	NAFE113x8_WriteCommand(CMD_CH4,0); // Set Pointer to ch4

	// AI3P to AICOM - GAIN 0.4x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI3P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AICOM);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,1);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 6000Sps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,7);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,1);

	// Delay 33.4us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,14);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);


	// Channel 5 AI3N-AICOM
	NAFE113x8_WriteCommand(CMD_CH5,0); // Set Pointer to ch5

	// AI3N to AICOM - GAIN 2x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AICOM);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI3N);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,4);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4 - 6000ps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,14);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,7);

	// Delay 33.4us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,14);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);


	// Channel 6 AI4P-AI4N
	NAFE113x8_WriteCommand(CMD_CH6,0); // Set Pointer to ch5

	// AI4P to AI4N - GAIN 16x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI4P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI4N);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,7);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,1);

	// SINC4+1 - 50ps
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,1);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,20);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,7);

	// Delay 16.493us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);

	PRINTF("\r\nConnections for Each Logical Channel\r\n");
	PRINTF("\r\n****************\r\n");
	PRINTF("\r\n 0 - AI1P-AICOM \r\n");
	PRINTF("\r\n 1 - AI1N-AICOM \r\n");
	PRINTF("\r\n 2 - AI2P-AICOM \r\n");
	PRINTF("\r\n 3 - AI1N-AICOM \r\n");
	PRINTF("\r\n 4 - AI3P-AICOM \r\n");
	PRINTF("\r\n 5 - AI3N-AICOM \r\n");
	PRINTF("\r\n 6 - AI4P-AI4N \r\n");
	PRINTF("\r\n****************\r\n");

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	offset = ChData0.Address;
	temp.Bytes = ChData0.Bytes;
	for(int i=0;i<7;i++) {
		temp.Address = offset;
		clearReceivedFlag();
		NAFE113x8_WriteCommand(CMD_CH0+i,0); // Set Pointer to ch i (0 -> 6)
		ticks[i] = tic(); // Start counter to monitor the duration for each conversion
		NAFE113x8_WriteCommand(CMD_SS,0); // Start a single channel conversion
		while(!getReceivedFlag());
		ticks[i] = toc(ticks[i]); // Stop counter to monitor the duration for each conversion
		NAFE113x8_ReadRegister(&temp, 0, &regValue);

		conversionResult = NAFE113x8_VoltageTranslation(regValue,HV,0,gains[i]);
		PRINTF("\r\n\033[32mChannel %d = %.6fV\033[37m\r\n",i,conversionResult);
		offset += 1;

		time = (ticks[i]) / (SystemCoreClock / 1000.F);
		PRINTF("\r\nElapsed Time for Channel %d = %3fms\r\n", i ,time);
		time_tot += time;

	}
	PRINTF("\r\n\033[33mTotal Elapsed Time = %3fms\033[37m\r\n", time_tot);
	PRINTF("\r\n-----------------------------------\r\n");

	GPIO_SetPinInterruptConfig(BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_GPIO, BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_PIN, kGPIO_DMARisingEdge);
	DisableIRQ(GPIO00_IRQn);
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

static uint32_t tic(void)
{
	SysTick_Config(0x00FFFFFF);
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return SysTick->VAL;
}

static uint32_t toc(uint32_t tic_ticks)
{
	return tic_ticks - SysTick->VAL;
}



