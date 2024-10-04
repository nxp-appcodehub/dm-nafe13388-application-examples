/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    CurrentSensing.c
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
static void ISenseSCSR();
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

    	PRINTF("\r\n\033[35mCurrent Sensing using Single Channel Single Reading Conversion\033[37m\r\n");  // Single Channel Single Reading Conversion
		ISenseSCSR();
    }
    return 0 ;
}

static void ISenseSCSR(){

	GPIO_SetPinInterruptConfig(BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_GPIO, BOARD_INITDEBUG_UARTPINS_NAFE_DRDY_PIN, kGPIO_InterruptRisingEdge);
	EnableIRQ(GPIO00_IRQn);

	uint32_t regValue;
	float conversionResult;

    uint32_t ticks[1];
    float time;
    float time_tot;
	time_tot = 0;

	// Current Sensing (I = V/R)
	NAFE113x8_WriteCommand(CMD_CH0,0); // Set Pointer to ch0

	// AI4P to AI4N - GAIN 0.4x
	NAFE113x8_WriteField(&ChConfig0_HvAip,0,HVAIP_AI4P);
	NAFE113x8_WriteField(&ChConfig0_HvAin,0,HVAIN_AI4N);
	NAFE113x8_WriteField(&ChConfig0_ChGain,0,1);
	NAFE113x8_WriteField(&ChConfig0_HvSel,0,1);
	NAFE113x8_WriteField(&ChConfig0_LvSignIn,0,0);
	NAFE113x8_WriteField(&ChConfig0_TccOff,0,0);

	// SINC4 - 12000Sps, Single Cycle Settling Mode
	NAFE113x8_WriteField(&ChConfig1_AdcSinc,0,0);
	NAFE113x8_WriteField(&ChConfig1_ChThrs,0,0);
	NAFE113x8_WriteField(&ChConfig1_AdcDataRate,0,5);
	NAFE113x8_WriteField(&ChConfig1_GainOffPntr,0,1);

	// Delay 16.5us
	NAFE113x8_WriteField(&ChConfig2_AdcNormalSet,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChDelay,0,11);
	NAFE113x8_WriteField(&ChConfig2_AdcFilterRst,0,0);
	NAFE113x8_WriteField(&ChConfig2_ChChop,0,0);

	// Note: One can connect any resistance value, just change its value in conversion formula accordingly.
	PRINTF("\r\nConnections Guide\r\n");
	PRINTF("\r\n****************\r\n");
	PRINTF("\r\nProvide Voltage using Power Supply on AI4P-AI4N\r\n"); // Differential Signal (AIxP-AIxN)
	PRINTF("\r\nConnect 220 Ohms Resistance on AI4P-AI4N \r\n");
	PRINTF("\r\n****************\r\n");

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	for(int i=0;i<1;i++) {
	clearReceivedFlag();
	ticks[i] = tic();
	NAFE113x8_WriteCommand(CMD_CH0+i,0);
	NAFE113x8_WriteCommand(CMD_SS,0);
	while(!getReceivedFlag());
	ticks[i] = toc(ticks[i]);
	NAFE113x8_ReadRegister(&ChData0, 0, &regValue);

	conversionResult = NAFE113x8_VoltageTranslation(regValue,HV,0,0.4);
	PRINTF("\r\nVoltage = %6fV, Current (220ohms Shunt)=%6fA\r\n",conversionResult,(conversionResult/220));

	time = (ticks[i]) / (SystemCoreClock / 1000.F);
	PRINTF("Elapsed Time for Channel %d = %3fms\r\n", i ,time);
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
