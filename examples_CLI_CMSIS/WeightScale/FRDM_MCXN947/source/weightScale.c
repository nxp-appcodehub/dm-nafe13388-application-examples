/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  weightScale.c
 * @brief The weightScale.c file implements
 *   	  weight scale application to test SCCR reading mode.
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

static void weightScale();

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

/*!@brief        Function to sense voltage across wires of load cell.
 *  @details     This is the function sense voltage across wires of load cell in SCCR mode.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */

static void weightScale()
{

	int knownWeight;
	char key;
	float gains[ACTIVE_CHANNELS] = {16};
	uint8_t config[NAFE13388_2BYTE_REG];
	int numOfConversions = 50;

	float offset;
	float voltage;
	float coefficient;
	float weight;
	bool confirmation = true;
	int status;
	nafe13388_sccr_conversion weightScale_sccr;

	// Ch0 Force/Sense Configuration
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH0); // Set Pointer to ch0

	// AI1P to AI1N - GAIN 16x
	config[0] = 0x11;
	config[1] = 0xF0;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	// SINC4+1 - 100Sps
	config[0] = 0x70;
	config[1] = 0xA1;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	// Delay 16.5us
	config[0] = 0x2E;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	// Voltage - Positive Pol - 6V - AI2P
	config[0] = 0x34;
	config[1] = 0x10;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG3,config,NAFE13388_2BYTE_REG);

	PRINTF("\r\n********************\r\n");
	PRINTF("\r\n-> Calculation of Offset <-\r\n");
	PRINTF("\r\n-> Step 1:Offset Calculation <-\r\n");
	PRINTF("\r\nLeave the weight scale without weight and click any buttons.\r\n");
	NAFE13388_SCCR(&nafe13388Driver, numOfConversions, gains, &weightScale_sccr);

	PRINTF("\r\nPress any key to start conversion\r\n");
	GETCHAR(); // Wait a keyboard import to start a conversion sequence.

	offset = weightScale_sccr.voltage;

	PRINTF("-----------------------------------");
	PRINTF("\r\n-> Step 1: Done <-\r\n");
	PRINTF("\r\n********************\r\n");

	PRINTF("\r\n-> Step 2 : Calibration Coefficient Calculation <-\r\n");
	PRINTF("\r\nFor First Calculation Put Known Weight for Calibration\r\n");

	while (confirmation){
		PRINTF("\r\nPut an object of known weight on the scale\r\n",knownWeight);
		PRINTF("\r\nPlease enter the weight value in grams and then click enter key\r\n");
		SCANF("%i", &knownWeight); // Wait a keyboard import to start a conversion sequence.
		PRINTF("\r\nThe weight on the scale is %dg, do you confirm(y/n)?\r\n", knownWeight);
		key = GETCHAR();
		if (key == 'y'){confirmation = false;}
		else{confirmation = true;}
	}
	NAFE13388_SCCR(&nafe13388Driver, numOfConversions, gains, &weightScale_sccr);
	coefficient = knownWeight / (weightScale_sccr.voltage - offset);

	PRINTF("\r\n-> Step 2: Done <-\r\n");
	PRINTF("\r\n********************\r\n");
	while(1)
	{
		PRINTF("\r\n->Click any button to start one Conversion sequences<-\r\n");
		GETCHAR(); // Wait a keyboard import to start a conversion sequence.

		NAFE13388_SCCR(&nafe13388Driver, numOfConversions, gains, &weightScale_sccr);
		weight = coefficient*(weightScale_sccr.voltage-offset);
		PRINTF("\r\n\033[32mVoltage Load Cell=%6fmV, Weight=%1fg\033[37m\r\n", weightScale_sccr.voltage*1000,weight);
		PRINTF("\r\n********************\r\n");

		PRINTF("\r\nPut any item to calculate its weight \r\n");
	}

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
		PRINTF("\r\n\033[35mWeight Scale (Load Cell)\033[37m\r\n");

		PRINTF("\r\n********************\r\n");
		PRINTF("\r\nThree Step Procedure:\r\n");
		PRINTF("\r\nStep1: Offset Calculation\r\n");
		PRINTF("\r\nStep2: Calibration Coefficient Calculation\r\n");
		PRINTF("\r\nStep3: Actual Weight Calculation\r\n");
		PRINTF("\r\n********************\r\n");

		PRINTF("\r\nPress any key to start the procedure\r\n");
		GETCHAR();
		weightScale();
	}

	return 0;
}
