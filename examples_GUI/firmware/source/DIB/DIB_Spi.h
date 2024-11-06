/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef DIB_SPI_H_
#define DIB_SPI_H_

#include "virtual_com.h"

void SPI_Handling(parse_vcom_t *vcom_msg);

#define SPI_REQ_CONFIG 0
#define SPI_REQ_SEND 1
#define SPI_REQ_RESET 2
#define SPI_REQ_INIT 3

#define LP_FLEXCOMM1_CLOCK_FREQ 64000000UL

#define BYTESTO32BIT(BYTE0,BYTE1,BYTE2,BYTE3) (BYTE3<<24)|(BYTE2<<16)|(BYTE1<<8)|BYTE0

typedef struct spiInitParse
{
	LPSPI_Type *SPIInstace;
	uint32_t frequency;
	uint8_t mode;
	uint8_t pcs_polarity;
} spiInitParse_t;

typedef struct spiConfigParse
{
	LPSPI_Type *SPIInstace;
	uint32_t frequency;
} spiConfigParse_t;

typedef struct spiSendReceiveParse
{
	LPSPI_Type *SPIInstace;
	uint8_t *txData;
	uint8_t *rxData;
	size_t dataSize;
	uint8_t pcs;

}spiSendReceiveParse_t;


#endif /* DIB_SPI_H_ */
