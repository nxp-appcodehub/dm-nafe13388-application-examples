/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  main.c
 * @brief The main.c file implements
 *   	  NAFE13388 shield board applications.
 */
//-----------------------------------------------------------------------
// SDK Includes
//-----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"
#include "fsl_debug_console_cmsis.h"
#include "fsl_lpuart_cmsis.h"
#include "fsl_device_registers.h"
#include "main.h"
#include "Driver_USART.h"
#include "hardware_init.h"

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
#include "dib_nafe13388.h"
#include "dib_sys.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (RTE_SPI1_DMA_EN)
#define EXAMPLE_DMA_BASEADDR (DMA0)
#define EXAMPLE_DMA_CLOCK    kCLOCK_Dma0
#endif


//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------

#define VALID_HEADER "*#*"
#define VALID_FOOTER "^&@"
volatile bool rxBufferEmpty           = true;
volatile bool txBufferFull            = false;
volatile bool txOnGoing               = false;
volatile bool rxOnGoing               = false;

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------
nafe13388_sensorhandle_t nafe13388Driver;

static IOStates_t uartCaptureState=STATE_IDLE;
static int pktcount= 0;
unsigned char proc_buffer[1024];
extern char buf_read_write[1024];
extern int count;
int proc_data_flag = 0;
int interrupt_wait = 1;
static char headerbytes[3] = {0};
static char footerbytes[3] = {0};

/*! @brief       Processes the received data packet.
 *  @details     Process the received data packet and assign the particular state to that data.
 *  @param[in]   rx_byte  		Input for the data.
 *  @constraints No
 *  @reentrant   No
 */
int processReceivedPacket(unsigned char rx_byte)
{
	/*
	 * Header Byte buffering
	 * */
	headerbytes[0]=headerbytes[1];
	headerbytes[1]=headerbytes[2];
	headerbytes[2]=rx_byte;
	/*
	 * Footer Byte buffering
	 * */
	footerbytes[0]=footerbytes[1];
	footerbytes[1]=footerbytes[2];
	footerbytes[2]=rx_byte;
	/*
	 * Header Comparison
	 * */
	if(strncmp(headerbytes,VALID_HEADER,3)==0)
	{
		uartCaptureState=STATE_HEADER;

	}
	/*
	 * Footer Comparison
	 * */
	if(strncmp(footerbytes,VALID_FOOTER,3)==0)
	{
		uartCaptureState=STATE_FOOTER;
	}
	switch(uartCaptureState)
	{
	case STATE_IDLE:
		break;
	case STATE_HEADER:
		pktcount=0;
		memset(proc_buffer,0,1024);
		uartCaptureState=STATE_DATA;
		break;
	case STATE_DATA:
		proc_buffer[pktcount]=rx_byte;
		pktcount++;
		uartCaptureState=STATE_DATA;
		break;
	case STATE_FOOTER:
		proc_data_flag = 1;
		proc_buffer[pktcount-2] = '\0';
		uartCaptureState=STATE_IDLE;
		break;
	}
	return 0;
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
	int blen;
	uint8_t *imsg;
	uint8_t *omsg;
	parse_vcom_t vcom_msg;
	vcom_msg.isize = 0;
	vcom_msg.osize=0;
	vcom_msg.imsg=imsg;
	vcom_msg.omsg=omsg;

#if (RTE_SPI1_DMA_EN)
	/* Enable DMA clock. */
	CLOCK_EnableClock(EXAMPLE_DMA_CLOCK);
	edma_config_t edmaConfig = {0};
	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(EXAMPLE_DMA_BASEADDR, &edmaConfig);
#endif

	ARM_DRIVER_SPI *pdriver = &SPI_S_DRIVER;

	/*! Initialize the MCU hardware. */
	BOARD_InitHardware();
    BOARD_PowerMode_OD();
    CLOCK_SetupExtClocking(BOARD_XTAL0_CLK_HZ);

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

	while(1)
	{
		if(proc_data_flag == 0)
		{
			char data = 0;

			rxOnGoing = true;
			DEMO_USART.Receive(&data, 1);
			while(rxOnGoing);

			processReceivedPacket(data);

		}else{

			vcom_msg.imsg = malloc(proc_buffer[0]);
			vcom_msg.isize = proc_buffer[0];
			vcom_msg.itype = proc_buffer[1];
			memcpy(vcom_msg.imsg, proc_buffer + 2, proc_buffer[0]);

		   	switch(vcom_msg.itype){
				case(SYS_REQ):
					SYS_Handling(&vcom_msg);
						break;
				case(NAFE113x8_REQ):
					NAFE113x8_Handling(&vcom_msg);
						break;
			}

			free(vcom_msg.imsg);

			unsigned char uart_tx_buffer[1024];

			uart_tx_buffer[0] = '*';
			uart_tx_buffer[1] = '#';
			uart_tx_buffer[2] = '*';

			blen = buf_read_write[0]+2;
			memcpy(uart_tx_buffer + 3, buf_read_write, buf_read_write[0]+2);

			uart_tx_buffer[3 + 0 + blen] = '^';
			uart_tx_buffer[3 + 1 + blen] = '&';
			uart_tx_buffer[3 + 2 + blen] = '@';

			txOnGoing = true;
			DEMO_USART.Send(uart_tx_buffer, blen + 6);
			while(txOnGoing);

			proc_data_flag = 0;
			interrupt_wait = 0;

	        memset(proc_buffer, 0, sizeof(proc_buffer));
	        memset(buf_read_write, 0, sizeof(buf_read_write));
		}
	}

	return 0;
}
