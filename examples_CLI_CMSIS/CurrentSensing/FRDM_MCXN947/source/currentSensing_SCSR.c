/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  currentSensing_SCSR.c
 * @brief The currentSensing_SCSR.c file implements
 *   	  current sensing application to test SCSR reading mode.
 */
//-----------------------------------------------------------------------
// SDK Includes
//-----------------------------------------------------------------------
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"
#include "fsl_debug_console.h"
#include "math.h"

//-----------------------------------------------------------------------
// ISSDK Includes
//-----------------------------------------------------------------------

#include "issdk_hal.h"
#include "gpio_driver.h"
#include "systick_utils.h"

//-----------------------------------------------------------------------
// CMSIS Includes
//-----------------------------------------------------------------------
#include "Driver_GPIO.h"
#include "nafe13388.h"
#include "nafe13388_sdk.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (RTE_SPI1_DMA_EN)
#define EXAMPLE_DMA_BASEADDR (DMA0)
#define EXAMPLE_DMA_CLOCK    kCLOCK_Dma0
#endif

static void ISenseSCSR();

//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------
#define ERROR_NONE       0
#define ERROR            1
#define ACTIVE_CHANNELS  1

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------
nafe13388_sensorhandle_t nafe13388Driver;

/*!@brief        Function to sense current in SCSR mode.
 *  @details     This is the function to sense current across AI4P-AI4N in SCSR conversion mode.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
static void ISenseSCSR(){

	float gains[ACTIVE_CHANNELS] = {0.4};
	uint8_t config[NAFE13388_2BYTE_REG];
	nafe13388_scsr_conversion cs_scsr;

	// Set pointer to Channel 0
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH0);

	// AI4P to AI4N - GAIN 0.4x
	config[0] = 0x44;
	config[1] = 0x30;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	// SINC4 - 12000Sps, Single Cycle Settling Mode
	config[0] = 0x10;
	config[1] = 0x28;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	// Delay 16.5us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	// Note: One can connect any resistance value, just change its value in conversion formula accordingly.
	PRINTF("\r\nConnections Guide\r\n");
	PRINTF("\r\n****************\r\n");
	PRINTF("\r\nProvide Voltage using Power Supply on AI4P-AI4N\r\n"); // Differential Signal (AIxP-AIxN)
	PRINTF("\r\nConnect 220 Ohms Resistance on AI4P-AI4N \r\n");
	PRINTF("\r\n****************\r\n");

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	NAFE13388_SCSR(&nafe13388Driver, ACTIVE_CHANNELS, gains, &cs_scsr);
	for(int i = 0; i < ACTIVE_CHANNELS; i++)
	{
		if (cs_scsr.conversionResult[i] < 0.0f)
			PRINTF("\r\nVoltage = -%6fV, Current (220ohms Shunt)=-%6fA\r\n",fabs(cs_scsr.conversionResult[i]),fabs(cs_scsr.conversionResult[i]/220));
		else
			PRINTF("\r\nVoltage = %6fV, Current (220ohms Shunt)=%6fA\r\n",cs_scsr.conversionResult[i],cs_scsr.conversionResult[i]/220);
		PRINTF("Elapsed Time for Channel %d = %3fms\r\n", i ,cs_scsr.time[i]);
	}
	PRINTF("\r\n\033[33mTotal Elapsed Time = %3fms\033[37m\r\n", cs_scsr.time_tot);
	PRINTF("\r\n-----------------------------------\r\n");
}

/*! @brief      This is the main function implementation.
 *  @details    This function invokes board initializes routines,
 *  			then brings up the NAFE13388-UIM board.
 *  @param[in]   void This is no input parameter.
 *  @constraints None
 *  @reentrant   No
 *  @return      Error.
 */
int main(void)
{
	int32_t status;
	uint8_t character;

#if (RTE_SPI1_DMA_EN)
	/* Enable DMA clock. */
	CLOCK_EnableClock(EXAMPLE_DMA_CLOCK);
	edma_config_t edmaConfig = {0};
	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(EXAMPLE_DMA_BASEADDR, &edmaConfig);
#endif

	ARM_DRIVER_SPI *pdriver = &SPI_S_DRIVER;

	/*! Initialize the MCU hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_SystickEnable();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	/*! Initialize the driver. */
	status = pdriver->Initialize(SPI_S_SIGNAL_EVENT);
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n SPI Initialization Failed\r\n");
		return -1;
	}
	/*! Set the Power mode. */
	status = pdriver->PowerControl(ARM_POWER_FULL);
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n Driver Power Mode setting Failed\r\n");
		return -1;
	}
	/*! Set the Slave speed. */
	status = pdriver->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA1, NAFE13388_SPI_S_BAUDRATE);
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n Driver Control Mode setting Failed\r\n");
		return -1;
	}
	/*! Initialize the NAFE13388 SDK driver. */
	status = NAFE13388_Initialize(&nafe13388Driver, &SPI_S_DRIVER, SPI_S_DEVICE_INDEX, &NAFE13388_CS);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n NAFE13388 Initialization Failed\r\n");
		return -1;
	}

	delay(10);
	PRINTF("\033[0;36m");
	PRINTF("----------------WELCOME TO NAFE MCXN DEMO---------------\r\n");
	PRINTF("\033[0m");
	while(1) 
	{
		PRINTF("\r\n\033[35mCurrent Sensing using Single Channel Single Reading Conversion\033[37m\r\n");  // Single Channel Single Reading Conversion
		ISenseSCSR();
	}

	return 0;
}
