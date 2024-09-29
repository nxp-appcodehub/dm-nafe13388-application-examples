/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 *file NAFE113x8_SDK.h
 */

#ifndef NAFE113X8_SDK_H_
#define NAFE113X8_SDK_H_

#include "fsl_inputmux_connections.h"
#include "fsl_inputmux.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_gpio.h"
#include "NAFE113x8_Register.h"

#define NAFE113x8_SPI_READ_FORMAT_BYTE0(DEVID,ADDRESS)  (DEVID << 7) | (1<<6) | (ADDRESS>>7)
#define NAFE113x8_SPI_READ_FORMAT_BYTE1(ADDRESS)        (ADDRESS & 0x7F)<<1
#define NAFE113x8_SPI_WRITE_FORMAT_BYTE0(DEVID,ADDRESS) (DEVID << 7) | (ADDRESS>>7)
#define NAFE113x8_SPI_WRITE_FORMAT_BYTE1(ADDRESS)       (ADDRESS & 0x7F)<<1
#define NAFE113x8_SPI_CMD_FORMAT_BYTE0(DEVID,CMD)       (DEVID << 7) | (CMD>>7)
#define NAFE113x8_SPI_CMD_FORMAT_BYTE1(CMD)             (CMD & 0x7F)<<1

#define LP_FLEXCOMM1_PERIPHERAL ((LPSPI_Type *)LP_FLEXCOMM1)
#define LP_FLEXCOMM1_CLOCK_FREQ 64000000UL
#define LPSPI1_PCS kLPSPI_MasterPcs0
#define bufferDMASize 64
#define DMASPI1 DMA0
#define DMASPI1_CHRx 0
#define DMASPI1_CHTxCmd 1
#define DMASPI1_CHTxDummy 2
#define DMASPI1_CHTxDummy2 3

void NAFE113x8_InitNafe();
void NAFE113x8_WriteCommand(uint16_t command, bool devID);
void NAFE113x8_ReadRegister(const Register_t *RegDef, bool devID, uint32_t *regValue);
void NAFE113x8_WriteRegister(const Register_t *RegDef, bool devID, uint32_t regValue);
void NAFE113x8_selectChannel(uint8_t channel, bool devID);
void NAFE113x8_WriteField(const Field_t *FieldDef, bool devID, uint16_t fieldValue);
void NAFE113x8_ReadField(const Field_t *FieldDef, bool devID, uint16_t *fieldValue);

void NAFE113x8_doSCCR(uint8_t channel, bool devID, uint16_t count);
void NAFE113x8_doMCMR(bool devID);
void NAFE113x8_doMCCR(bool devID, uint16_t count);

uint32_t NAFE113x8_getSample(uint16_t index);
float NAFE113x8_VoltageTranslation(uint32_t value, uint8_t input , bool dataOut16, float gain);

void clearReceivedFlag(void);
bool getReceivedFlag(void);

enum InputType{
	HV,
	GPIO0_1,
	REF2,
	VADD,
	VHDD,
	VHSS
};

#endif /* NAFE113X8_SDK_H_ */
