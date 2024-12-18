/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  twoWireRTD.c
 *  @brief The twoWireRTD.c file implements
 *   	   voltage sensing application to test SCSR reading mode.
 */
//-----------------------------------------------------------------------
// SDK Includes
//-----------------------------------------------------------------------
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"
#include "fsl_debug_console.h"

//-----------------------------------------------------------------------
// ISSDK Includes
//-----------------------------------------------------------------------
#include "issdk_hal.h"
#include "gpio_driver.h"
#include "systick_utils.h"
#include "math.h"

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

static void twoWireRTD();

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

/*!@brief        Function to sense temperature across wires of two wire RTD.
 *  @details     This is the function to to sense temperature across wires of two wire RTD in SCCR mode.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
static void twoWireRTD()
{
	const float R0=100;
	const float A=0.003908;
	const float ISource=0.000765;
	int numOfConversions = 20;
	float gains[ACTIVE_CHANNELS] = {2};
	uint8_t config[NAFE13388_2BYTE_REG];
	float temperatureResult;
	nafe13388_sccr_conversion two_wire_rtd_sccr;

	// RTD Sensing and Forcing
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH0); // Set Pointer to ch0

	// AI1P to AICOM - GAIN 2x
	config[0] = 0x17;
	config[1] = 0x90;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	// SINC4+1 - 562.50Sps
	config[0] = 0x40;
	config[1] = 0x81;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	// Delay 16.5us
	config[0] = 0x2E;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	// Current - Positive Pol - 750uA - AI1P
	config[0] = 0xB0;
	config[1] = 0x08;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG3,config,NAFE13388_2BYTE_REG);

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR();

	PRINTF("\033[0;32m");
	PRINTF("\r\nVoltage\t\t\t\tTemperature");
	PRINTF("\033[0m");

	NAFE13388_SCCR(&nafe13388Driver, numOfConversions ,gains, &two_wire_rtd_sccr);
	for(int i = 0; i < numOfConversions; i++)
	{
		temperatureResult = ((two_wire_rtd_sccr.conversionResult[i]/ISource)-(R0+2398))/(A*R0);
		if (two_wire_rtd_sccr.conversionResult[i] < 0.0f)
			PRINTF("\r\nSample:%d Voltage RTD=-%6fV, Temperature RTD=-%3fC", i, fabs(two_wire_rtd_sccr.conversionResult[i]),fabs(temperatureResult));
		else
			PRINTF("\r\nSample:%d Voltage RTD=%6fV, Temperature RTD=%3fC", i, two_wire_rtd_sccr.conversionResult[i],temperatureResult);
	}
	PRINTF("\r\n-----------------------------------\r\n");
	temperatureResult = ((two_wire_rtd_sccr.voltage/ISource)-(R0+2398))/(A*R0);

	PRINTF("\r\n------------Average Conversion-------------");
	PRINTF("\r\n\033[32mVoltage RTD=%6fV, Temperature RTD=%3fC\033[37m",two_wire_rtd_sccr.voltage,temperatureResult);
	PRINTF("\r\n---------------------------\r\n");
}

/*! @brief      This is the The main function implementation.
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
		PRINTF("\r\n\033[35mTwo Wire RTD\033[37m\r\n");
		twoWireRTD();
	}

	return 0;
}
