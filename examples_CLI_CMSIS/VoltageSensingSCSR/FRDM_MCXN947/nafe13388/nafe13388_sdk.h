/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  nafe13388_sdk.h
 *  @brief The nafe13388_sdk.h file describes the NAFE13388 SDK interface and structures.
 */
#ifndef NAFE13388_SDK_H_
#define NAFE13388_SDK_H_

/* Standard C Includes */
#include <stdint.h>
#include "sensor_io_spi.h"
#include "register_io_spi.h"
#include "nafe13388.h"

/*! @def    NAFE13388_SPI_MAX_MSG_SIZE
 *  @brief  The MAX size of SPI message. */
#define NAFE13388_SPI_MAX_MSG_SIZE (64)

/*! @def    NAFE13388_SPI_CMD_LEN
 *  @brief  The size of the Sensor specific SPI Header. */
#define NAFE13388_SPI_CMD_LEN (2)

/*! @def    NAFE13388_SS_ACTIVE_VALUE
 *  @brief  Is the Slave Select Pin Active Low or High. */
#define NAFE13388_SS_ACTIVE_VALUE SPI_SS_ACTIVE_LOW

/*! @def    OUTPUT_BYTES
 *  @brief  ADC conversion results (24 Bit). */
#define OUTPUT_BYTES 3

/*! @def    NAFE13388_CHANNELS
 *  @brief  Total of 16 logical channels */
#define NAFE13388_CHANNELS 16

/*! @def    NAFE13388_MAX_CONV_COUNT
 *  @brief  Set Max Conversion Count */
#define NAFE13388_MAX_CONV_COUNT 100

/*! @def    NAFE13388_2BYTE_REG
 *  @brief  16 bit register */
#define NAFE13388_2BYTE_REG 2

/*! @def    NAFE13388_3BYTE_REG
 *  @brief  24 bit register */
#define NAFE13388_3BYTE_REG 3

/*! @def    NAFE13388_DEVID
 *  @brief  NAFE13388 Device ID */
#define NAFE13388_DEVID  0

/*! @def    NAFE13388_COMMAND_WRITE_SIZE
 *  @brief  NAFE13388 Command Write Size */
#define NAFE13388_COMMAND_WRITE_SIZE 0

/*!
 * @brief This defines the sensor specific information for SPI.
 */
typedef struct
{
	registerDeviceInfo_t deviceInfo;      /*!< SPI device context. */
	ARM_DRIVER_SPI *pCommDrv;             /*!< Pointer to the spi driver. */
	bool isInitialized;                   /*!< Whether sensor is intialized or not.*/
	spiSlaveSpecificParams_t slaveParams; /*!< Slave Specific Params.*/
}  nafe13388_sensorhandle_t;

/*! @brief This structure defines fields required for SCSR conversion.*/
typedef struct {
	double conversionResult[NAFE13388_CHANNELS];
	float time[NAFE13388_CHANNELS];
	float time_tot;
}nafe13388_scsr_conversion;

/*! @brief This structure defines fields required for SCCR conversion.*/
typedef struct {
	float conversionResult[NAFE13388_MAX_CONV_COUNT];
	float voltage;
}nafe13388_sccr_conversion;

/*! @brief This structure defines fields required for MCMR conversion.*/
typedef struct {
	float conversionResult[NAFE13388_CHANNELS];
}nafe13388_mcmr_conversion;

typedef struct {
	float conversionResult[NAFE13388_CHANNELS][100];
	float voltage[NAFE13388_CHANNELS];
}nafe13388_mccr_conversion;

/*--------------------------------
 ** Enum: InputType
 ** @brief InputType
 ** ------------------------------*/
enum InputType{
	HV,
	GPIO0_1,
	REF2,
	VADD,
	VHDD,
	VHSS
};

/*******************************************************************************
 * APIs
 ******************************************************************************/

/*! @brief       Preprocesses a read command for the NAFE13388-UIM.
 *  @details     Prepares a read command to be sent to the NAFE13388-UIM.
 *  @param[in]   pCmdOut  		Pointer to the command output buffer.
 *  @param[in]   offset   		Offset for the read command.
 *  @param[in]   size     		Size of the read command.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant  No
 */
void NAFE13388_ReadPreprocess(void *pCmdOut, uint32_t offset, uint32_t size);

/*! @brief       Preprocesses a write command for the NAFE13388-UIM.
 *  @details     Prepares a write command to be sent to the NAFE13388-UIM.
 *  @param[in]   pCmdOut  		Pointer to the command output buffer.
 *  @param[in]   offset  		Offset for the write command.
 *  @param[in]   size     		Size of the write command.
 *  @param[in] 	 pWritebuffer 	Pointer to the buffer containing data to be written.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant  No
 */
void NAFE13388_WritePreprocess(void *pCmdOut, uint32_t offset, uint32_t size, void *pWritebuffer);

/*! @brief       Preprocesses a command for the NAFE13388-UIM.
 *  @details     Prepares a command to be sent to the NAFE13388-UIM.
 *  @param[in]   pCmdOut  		Pointer to the command output buffer.
 *  @param[in]   offset  		Offset for the write command.
 *  @param[in]   size     		Size of the write command.
 *  @param[in] 	 pWritebuffer 	Pointer to the buffer containing data to be written.
 *  @param[in]   command 	    Two byte command to be sent to NAFE13388-UIM.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant  No
 */
void NAFE13388_CommandPreprocess(void *pCmdOut,uint32_t size, void *pWritebuffer,uint16_t command);

/*! @brief       Initializes the NAFE13388-UIM.
 *  @details     Initializes the NAFE13388-UIM Analog Front End and its handle.
 *  @param[in]   pSensorHandle  Pointer to sensor handle structure.
 *  @param[in]   pBus  			Pointer to CMSIS API compatible SPI bus object.
 *  @param[in]   index     		Index of the sensor.
 *  @param[in] 	 pSlaveSelect 	Pointer to the slave select pin.
 *  @constraints This should be the first API to be called.
 *				 Application has to ensure that previous instances of these APIs have exited before invocation.
 *  @reentrant   No
 *  @return      ::NAFE13388_Initialize() returns the status
 */
int32_t NAFE13388_Initialize(nafe13388_sensorhandle_t *pSensorHandle, ARM_DRIVER_SPI *pBus, uint8_t index, void *pSlaveSelect);

/*! @brief       Function to Read a Register.
 *  @details     This is the function to read a register (16/24 bit).
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   RegAddress  		Register Address.
 *  @param[in]   *Data  		    Pointer to data to be read.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_Read() returns the status.
 */
int32_t NAFE13388_Read(nafe13388_sensorhandle_t *pSensorHandle, uint8_t RegAddress, uint8_t* Data,uint8_t length);

/*! @brief       Function to Write a 16 bit command to NAFE13388-UIM.
 *  @details     This is the function to Write a 16 bit command to NAFE13388-UIM.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   RegAddress  		Register Address.
 *  @param[in]   *Data  		    Pointer to data to be read.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_CommandWrite() returns the status.
 */
int32_t NAFE13388_CommandWrite(nafe13388_sensorhandle_t *pSensorHandle, uint16_t Data);

/*! @brief       Function to Write a Register.
 *  @details     This is the function to write a register (16/24 bit).
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   RegAddress  		Register Address.
 *  @param[out]  Data       	    16 bit data.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_Write() returns the status.
 */
int32_t NAFE13388_Write(nafe13388_sensorhandle_t *pSensorHandle, uint8_t RegAddress, uint8_t *Data,uint8_t length);

/*! @brief       Performs SCSR conversion for the NAFE13388-UIM.
 *  @details     Performs the single channel single reading conversion for the NAFE13388-UIM.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   channels           number of channels enabled for scsr conversion.
 *  @param[in]   gains[]  			Array of gains of type float.
 *  @param[in]   *scsr  		    Pointer to structure containing scsr conversion fields.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_SCSR() returns the status.
 */
int32_t NAFE13388_SCSR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t channels, float gains[], nafe13388_scsr_conversion *scsr);

/*! @brief       Performs MCMR conversion for the NAFE13388-UIM.
 *  @details     Performs the multi channel multi reading conversion for the NAFE13388-UIM.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   channels           number of channels enabled for mcmr conversion.
 *  @param[in]   gains[]  			Array of gains of type float.
 *  @param[in]   *mcmr  		    Pointer to structure containing mcmr conversion fields.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_MCMR() returns the status.
 */
int32_t NAFE13388_MCMR(nafe13388_sensorhandle_t *pSensorHandle,uint8_t channels, float gains[], nafe13388_mcmr_conversion *mcmr);

/*! @brief       Performs SCCR conversion for the NAFE13388-UIM.
 *  @details     Performs the single channel continuous reading conversion for the NAFE13388-UIM.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   count              number of channels enabled for sccr conversion.
 *  @param[in]   gains[]  			Array of gains of type float.
 *  @param[in]   *sccr  		    Pointer to structure containing sccr conversion fields.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_SCCR() returns the status.
 */
int32_t NAFE13388_SCCR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t count, float gains[], nafe13388_sccr_conversion *sccr);

/*! @brief       Function to add delay in ms.
 *  @details     This function is used to add delay in milliseconds.
 *  @param[in]   ms  			Delay in ms.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::delay() returns the status.
 */
void delay(uint32_t ms);

/*! @brief       Function to clear sampleReceived flag.
 *  @details     This function is used to clear the sampleReceived flag.
 *  @param[in]   void
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::clearReceivedFlag() returns the status.
 */
void clearReceivedFlag(void);

/*! @brief       Function to get sampleReceived flag.
 *  @details     This function is used to fetch the value of sampleReceived flag.
 *  @param[in]   void
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::getReceivedFlag() returns the status.
 */
bool getReceivedFlag(void);

/*! @brief       Function to set sampleReceived flag to true.
 *  @details     Function to set sampleReceived flag to true if DRDY gets high.
 *  @param[in]   void
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::GPIO00_IRQHandler() returns the status.
 */
void GPIO00_IRQHandler(void);

/*! @brief       Function for voltage translation.
 *  @details     Function for voltage translation to be used in different reading modes.
 *  @param[in]   value              ADC conversion value.
 *  @param[in]   input              Type of value (HV in case of high voltage conversions).
 *  @param[in]   dataOut16          16 bit data output (true or false).
 *  @param[in]   gain               Gain setting (0.2x to 16x).
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_VoltageTranslation() returns the status.
 */
float NAFE13388_VoltageTranslation(uint32_t value, uint8_t input , bool dataOut16, float gain);

/*! @brief       Function to get systicks.
 *  @details     This is the function to get systicks.
 *  @param[in]   void
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::tic() returns the status.
 */
uint32_t tic(void);

/*! @brief       Function to get count of total number of systicks.
 *  @details     This is the function to get count of total number of systicks.
 *  @param[in]   tic_ticks              Maintains count of systicks.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::toc() returns the status.
 */
uint32_t toc(uint32_t tic_ticks);

/*! @brief       Performs MCCR conversion for the NAFE13388-UIM.
 *  @details     Performs the multi channel continuous reading conversion for the NAFE13388-UIM.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   channels           number of channels enabled for mccr conversion.
 *  @param[in]   gains[]  			Array of gains of type float.
 *  @param[in]   *mcmr  		    Pointer to structure containing mccr conversion fields.
 *  @constraints This can be called any number of times only after NAFE13388_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::NAFE13388_MCMR() returns the status.
 */
int32_t NAFE13388_MCCR(nafe13388_sensorhandle_t *pSensorHandle, uint8_t channels, uint8_t count, float gains[], nafe13388_mccr_conversion *mccr);
#endif /* NAFE13388_SDK_H_ */
