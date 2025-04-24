/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  nafe13388_sdk.c
 * @brief The nafe13388_sdk.c file implements SDK functions
 * 		  having different function to support functionalities
 * 		  provided by NAFE13388-UIM
 */
#include "nafe13388_sdk.h"
#include "gpio_driver.h"
#include "systick_utils.h"
#include "fsl_debug_console_cmsis.h"
#include "fsl_edma.h"
#include "frdmmcxn947.h"
#include "Driver_GPIO.h"

GENERIC_DRIVER_GPIO *pGpioDriver = &Driver_GPIO_KSDK;

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------
uint8_t nafe13388_spiRead_CmdBuffer[NAFE13388_SPI_MAX_MSG_SIZE] = {0};
uint8_t nafe13388_spiRead_DataBuffer[NAFE13388_SPI_MAX_MSG_SIZE] = {0};
uint8_t nafe13388_spiWrite_CmdDataBuffer[NAFE13388_SPI_MAX_MSG_SIZE] = {0};

volatile bool SampleReceived = false;

/*! -----------------------------------------------------------------------
 *  @brief       Initialize NAFE13388 Interrupt Source and Enable IRQ
 *  @details     This function initializes NAFE13388 DRDY pin
 *  @return      void  There is no return value.
 *  -----------------------------------------------------------------------*/
void enableDRDYHasGPIOInterrupt(bool enable){

	if(enable)
	{
		pGpioDriver->pin_init(&NAFE_DRDY, GPIO_DIRECTION_IN, &NAFE_DRDY_config, (void *)GPIO00_IRQHandler, NULL);
	}

}

void NAFE13388_ReadPreprocess(void *pCmdOut, uint32_t offset, uint32_t size)
{
	spiCmdParams_t *pSlaveCmd = pCmdOut;
	uint8_t *pWBuff = nafe13388_spiRead_CmdBuffer;
	uint8_t *pRBuff = nafe13388_spiRead_DataBuffer;

	*(pWBuff) = (NAFE13388_DEVID << 7) | (1 << 6) | (offset >> 7);
	*(pWBuff + 1) = (offset & 0x7F) << 1;

	/* Create the slave read command. */
	pSlaveCmd->size = size + 2;
	pSlaveCmd->pWriteBuffer = pWBuff;
	pSlaveCmd->pReadBuffer = pRBuff;
}

void NAFE13388_WritePreprocess(void *pCmdOut, uint32_t offset, uint32_t size, void *pWritebuffer)
{
	spiCmdParams_t *pSlaveCmd = pCmdOut;
	uint8_t *pWBuff = nafe13388_spiWrite_CmdDataBuffer;
	uint8_t *pRBuff = nafe13388_spiWrite_CmdDataBuffer + size + 2;

	*(pWBuff) = (NAFE13388_DEVID << 7) | (offset >> 7); /* offset is the internal register address of the sensor at which write is performed. */
	*(pWBuff + 1) = (offset & 0x7F) << 1;

	memcpy(pWBuff + 2, pWritebuffer, size);

	/* Create the slave command. */
	pSlaveCmd->size = size + 2;
	pSlaveCmd->pWriteBuffer = pWBuff;
	pSlaveCmd->pReadBuffer = pRBuff;
}

void NAFE13388_CommandPreprocess(void *pCmdOut, uint32_t size, void *pWritebuffer, uint16_t command)
{
	spiCmdParams_t *pSlaveCmd = pCmdOut;
	uint8_t *pWBuff = nafe13388_spiWrite_CmdDataBuffer;
	uint8_t *pRBuff = nafe13388_spiWrite_CmdDataBuffer + size + 2;

	*(pWBuff )  =  (NAFE13388_DEVID << 7) | (command >> 7);
	*(pWBuff + 1)  = (command & 0x7F) << 1;

	memcpy(pWBuff + 2, pWritebuffer, size);

	/* Create the slave command. */
	pSlaveCmd->size = size + 2;
	pSlaveCmd->pWriteBuffer = pWBuff;
	pSlaveCmd->pReadBuffer = pRBuff;
}

int32_t NAFE13388_Initialize(nafe13388_sensorhandle_t *pSensorHandle, ARM_DRIVER_SPI *pBus, uint8_t index, void *pSlaveSelect)
{
	int32_t status;
	uint8_t reg;
	GENERIC_DRIVER_GPIO *pGPIODriver = &Driver_GPIO_KSDK;

	/*! Check the input parameters. */
	if ((pSensorHandle == NULL) || (pBus == NULL) || (pSlaveSelect == NULL))
	{
		return SENSOR_ERROR_INVALID_PARAM;
	}

	/*! Initialize the sensor handle. */
	pSensorHandle->pCommDrv = pBus;
	pSensorHandle->slaveParams.pReadPreprocessFN = NAFE13388_ReadPreprocess;
	pSensorHandle->slaveParams.pWritePreprocessFN = NAFE13388_WritePreprocess;
	pSensorHandle->slaveParams.pCommandPreprocessFN = NAFE13388_CommandPreprocess;
	pSensorHandle->slaveParams.pTargetSlavePinID = pSlaveSelect;
	pSensorHandle->slaveParams.spiCmdLen = NAFE13388_SPI_CMD_LEN;
	pSensorHandle->slaveParams.ssActiveValue = NAFE13388_SS_ACTIVE_VALUE;

	pSensorHandle->deviceInfo.deviceInstance = index;
	pSensorHandle->deviceInfo.functionParam = NULL;
	pSensorHandle->deviceInfo.idleFunction = NULL;

	pSensorHandle->isInitialized = true;

    pGpioDriver->pin_init(&NAFE_RST, GPIO_DIRECTION_OUT, &NAFE_RST_config, NULL, NULL);
    pGpioDriver->pin_init(&ADC_SYNC, GPIO_DIRECTION_OUT, &ADC_SYNC_config, NULL, NULL);
    pGpioDriver->pin_init(&NAFE_nINT, GPIO_DIRECTION_IN, &NAFE_nINT_config, NULL, NULL);
    enableDRDYHasGPIOInterrupt(true);

	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_Read(nafe13388_sensorhandle_t *pSensorHandle, uint8_t RegAddress, uint8_t* Data,uint8_t length)
{
	int32_t status;

	status = Register_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			RegAddress, length, Data);

	if (ARM_DRIVER_OK != status)
	{
		pSensorHandle->isInitialized = false;
		return SENSOR_ERROR_INIT;
	}

	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_Write(nafe13388_sensorhandle_t *pSensorHandle, uint8_t RegAddress, uint8_t *Data,uint8_t length)
{
	int32_t status;

	status = Register_SPI_BlockWrite(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			RegAddress,Data,length);
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_CommandWrite(nafe13388_sensorhandle_t *pSensorHandle, uint16_t Data)
{
	int32_t status;

	status = Command_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams, Data, NAFE13388_COMMAND_WRITE_SIZE);

	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_SCSR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t channels, float gains[], nafe13388_scsr_conversion *scsr)
{
	int32_t status;
	float conversionResult;
	uint32_t ticks[NAFE13388_CHANNELS];
	uint8_t output[OUTPUT_BYTES];
	float time;
	float time_total;
	time_total = 0;
	bool outputData16bit = 0;
	uint32_t temp = 0;

	for(int i = 0; i < channels; i++)
	{
		clearReceivedFlag();
		ticks[i] = tic();
		NAFE13388_CommandWrite(pSensorHandle, CMD_CH0+i);
		NAFE13388_CommandWrite(pSensorHandle, CMD_SS);
		while(!getReceivedFlag());
		ticks[i] = toc(ticks[i]);

		status = NAFE13388_Read(pSensorHandle, NAFE13388_CH_DATA0+i, output, NAFE13388_3BYTE_REG);
		if (ARM_DRIVER_OK != status)
		{
			return SENSOR_ERROR_WRITE;
		}
		temp = output[0] << 16 | output[1] << 8 | output[2];
		scsr->conversionResult[i] = NAFE13388_VoltageTranslation(temp, HV, outputData16bit, gains[i]);
		scsr->time[i] = (ticks[i]) / (SystemCoreClock / 1000.F);
		time_total += scsr->time[i];
	}
	scsr->time_tot = time_total;

	return SENSOR_ERROR_NONE;
}


int32_t NAFE13388_MCMR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t channels, float gains[], nafe13388_mcmr_conversion *mcmr)
{
	int32_t status;
	float conversionResult;
	bool outputData16bit = 0;
	uint8_t output[OUTPUT_BYTES] = {0};
	uint32_t temp;

	for(int i = 0; i < channels; i++)
	{
		clearReceivedFlag();
		NAFE13388_CommandWrite(pSensorHandle, CMD_CH0+i);
		NAFE13388_CommandWrite(pSensorHandle, CMD_MM);
		while(!getReceivedFlag());

		status = NAFE13388_Read(pSensorHandle, NAFE13388_CH_DATA0+i, output, NAFE13388_3BYTE_REG);
		if (ARM_DRIVER_OK != status)
		{
			return SENSOR_ERROR_WRITE;
		}
		temp = output[0] << 16 | output[1] << 8 | output[2];
		mcmr->conversionResult[i] = NAFE13388_VoltageTranslation(temp, HV, outputData16bit, gains[i]);
		delay(100); //delay in ms
	}
	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_SCCR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t count, float gains[], nafe13388_sccr_conversion *sccr)
{
	int32_t status;
	float conversionResult;
	float voltageSum;
	bool outputData16bit = 0;
	uint32_t temp;
	uint8_t output[OUTPUT_BYTES];
	float time;
	float time_total;
	time_total = 0;
	voltageSum = 0;

	for(int i = 0; i < count; i++)
	{
		enableDRDYHasGPIOInterrupt(true);
		clearReceivedFlag();
		NAFE13388_CommandWrite(pSensorHandle, CMD_CH0);
		NAFE13388_CommandWrite(pSensorHandle, CMD_SC);
		while(!getReceivedFlag());
		delay(10); //delay in ms

		status = NAFE13388_Read(pSensorHandle, NAFE13388_CH_DATA0, output, NAFE13388_3BYTE_REG);
		if (ARM_DRIVER_OK != status)
		{
			return SENSOR_ERROR_WRITE;
		}
		temp = output[0] << 16 | output[1] << 8 | output[2];
		sccr->conversionResult[i] = NAFE13388_VoltageTranslation(temp, HV, outputData16bit , gains[0]);
		voltageSum += sccr->conversionResult[i];
	}
	sccr->voltage = voltageSum / count;

	return SENSOR_ERROR_NONE;
}

int32_t NAFE13388_MCCR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t channels, uint8_t count, float gains[], nafe13388_mccr_conversion *mccr)
{
	int32_t status;
	float conversionResult;
	bool outputData16bit = 0;
	uint8_t output[OUTPUT_BYTES] = {0};
	float voltageSum;
	voltageSum = 0;
	uint32_t temp;

	for(int i = 0;i < channels; i++)
	{
		for(int j = 0; j < count; j++)
			{
				clearReceivedFlag();
				NAFE13388_CommandWrite(pSensorHandle, CMD_CH0+i);
				NAFE13388_CommandWrite(pSensorHandle, CMD_MC);
				while(!getReceivedFlag());

				status = NAFE13388_Read(pSensorHandle, NAFE13388_CH_DATA0+i, output, NAFE13388_3BYTE_REG);
				if (ARM_DRIVER_OK != status)
				{
					return SENSOR_ERROR_WRITE;
				}
				temp = output[0] << 16 | output[1] << 8 | output[2];
				mccr->conversionResult[i][j] = NAFE13388_VoltageTranslation(temp, HV, outputData16bit, gains[i]);
				voltageSum += mccr->conversionResult[i][j];
				delay(10); //delay in ms
			}
		mccr->voltage[i] = voltageSum / count;
	}
	return SENSOR_ERROR_NONE;
}


float NAFE13388_VoltageTranslation(uint32_t value, uint8_t input , bool dataOut16, float gain){
	int sInt;
	float ConvertedValue;

	if (dataOut16){
		sInt = ((value + 0x10000) % (0x8000)) - 0x10000;
		ConvertedValue = 0.000152587 * sInt;
	}
	else
	{
		sInt = ((value + 0x800000) % (0x1000000)) - 0x800000;
		ConvertedValue = 0.000000596 * sInt;
	}

	switch(input)
	{
		case(GPIO0_1):
			ConvertedValue = ConvertedValue/2.5;
			break;
		case(REF2):
		case(VADD):
			ConvertedValue = ConvertedValue/2.5;
			ConvertedValue = 2*(ConvertedValue+1.5);
			break;
		case(VHDD):
			ConvertedValue = ConvertedValue/2.5;
			ConvertedValue = 32*(ConvertedValue+0.25);
			break;
		case(VHSS):
			ConvertedValue = ConvertedValue/2.5;
			ConvertedValue = -1*32*(ConvertedValue-0.25);
			break;
		default:
			ConvertedValue = ConvertedValue/gain;
			break;
	}
	return ConvertedValue;
}

void clearReceivedFlag(void){
	SampleReceived = false;
}

bool getReceivedFlag(void){
	return SampleReceived;
}



void GPIO00_IRQHandler(void)
{
	/* Clear external interrupt flag. */
	GPIO_GpioClearInterruptFlags(NAFE_DRDY.base, 1U << NAFE_DRDY.pinNumber);
	SampleReceived = true;
	SDK_ISR_EXIT_BARRIER;
}

void delay(uint32_t ms)
{
	volatile int i,j;
	for(i = 0; i < ms; i++)
	{
		for(j = 0; j < 15000; j++)
		{
			__NOP();
		}
	}
}

uint32_t tic(void)
{
	SysTick_Config(0x00FFFFFF);
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return SysTick->VAL;
}

uint32_t toc(uint32_t tic_ticks)
{
	return tic_ticks - SysTick->VAL;
}
