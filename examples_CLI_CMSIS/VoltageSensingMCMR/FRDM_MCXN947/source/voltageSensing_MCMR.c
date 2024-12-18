/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  voltageSensing_MCMR.c
 * @brief voltageSensing_MCMR.c file implements
 *   	  voltage sensing application to test MCMR reading mode.
 */

//-----------------------------------------------------------------------
// SDK Includes
//-----------------------------------------------------------------------
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"
#include "fsl_debug_console.h"
#include<math.h>

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

static void VSenseMCMR();
//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------
#define ERROR_NONE       0
#define ERROR            1
#define ACTIVE_CHANNELS  7

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------
nafe13388_sensorhandle_t nafe13388Driver;

/*!@brief        Function to sense voltage in MCMR mode.
 *  @details     This is the function to sense voltage across '
 *  			 different analog inputs in MCMR conversion mode.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
static void VSenseMCMR(){

	uint8_t config[NAFE13388_2BYTE_REG];
	float gains[ACTIVE_CHANNELS] = {0.2,0.4,0.2,0.4,0.4,2,16};
	nafe13388_mcmr_conversion vs_mcmr;

	//Channel 0 AI1P-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH0);

	//AI1P to AICOM - GAIN 0.2x
	config[0] = 0x17;
	config[1] = 0x11;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 12000Sps
	config[0] = 0x00;
	config[1] = 0x28;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 16.493us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 1 AI1N-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH1);

	//AI1N to AICOM - GAIN 0.4x
	config[0] = 0x71;
	config[1] = 0x31;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 12000Sps
	config[0] = 0x10;
	config[1] = 0x28;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 16.493us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 2 AI2P-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH2);

	//AI2P to AICOM - GAIN 0.2x
	config[0] = 0x27;
	config[1] = 0x11;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 24000Sps
	config[0] = 0x00;
	config[1] = 0x18;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 16.493us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 3 AI1N-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH3);

	//AI1N to AICOM - GAIN 0.4x
	config[0] = 0x71;
	config[1] = 0x31;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 24000Sps
	config[0] = 0x10;
	config[1] = 0x18;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 16.493us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 4 AI3P-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH4);

	//AI3P to AICOM - GAIN 0.4x
	config[0] = 0x37;
	config[1] = 0x31;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 6000Sps
	config[0] = 0x10;
	config[1] = 0x38;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 33.4us
	config[0] = 0x38;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 5 AI3N-AICOM
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH5);

	//AI3N to AICOM - GAIN 2x
	config[0] = 0x73;
	config[1] = 0x91;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4 - 6000ps
	config[0] = 0x40;
	config[1] = 0x70;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 33.4us
	config[0] = 0x38;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	//Channel 6 AI4P-AI4N
	NAFE13388_CommandWrite(&nafe13388Driver,CMD_CH6);

	//AI4P to AI4N - GAIN 16x
	config[0] = 0x44;
	config[1] = 0xF1;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG0,config,NAFE13388_2BYTE_REG);

	//SINC4+1 - 50ps
	config[0] = 0x70;
	config[1] = 0xA1;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG1,config,NAFE13388_2BYTE_REG);

	//Delay 16.493us
	config[0] = 0x2C;
	config[1] = 0x00;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG2,config,NAFE13388_2BYTE_REG);

	// MCH_EN (Enable channel 0 to channel 6)
	config[0] = 0x00;
	config[1] = 0x7F;
	NAFE13388_Write(&nafe13388Driver,NAFE13388_CH_CONFIG4,config,NAFE13388_2BYTE_REG);


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

	NAFE13388_MCMR(&nafe13388Driver, ACTIVE_CHANNELS, gains, &vs_mcmr);
	delay(100);

	PRINTF("\033[0;32m");
	PRINTF("\r\nCh0\t\tCh1\t\tCh2\t\tCh3\t\tCh4\t\tCh5\t\tCh6 \r\n");
	PRINTF("\033[0m");
	for(int i = 0; i < ACTIVE_CHANNELS; i++)
	{
		if (vs_mcmr.conversionResult[i] < 0.0f)
			PRINTF("-%fV\t",fabs(vs_mcmr.conversionResult[i]));
		else
			PRINTF("%fV\t",vs_mcmr.conversionResult[i]);
	}

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
		PRINTF("\r\n\033[35mVoltage Sensing using Multi Channel Multi Reading Conversion\033[37m\r\n"); // Multi Channel Multi Reading Conversion
		VSenseMCMR();
	}

	return 0;
}
