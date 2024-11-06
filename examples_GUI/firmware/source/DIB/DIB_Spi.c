/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpspi.h"
#include "DIB_Spi.h"

static LPSPI_Type* uintToLPSPIType (uint8_t instance);
static void SPIReset(LPSPI_Type* instance);
static void SPIinit(spiInitParse_t *initParse);
static void SPIConfig(spiConfigParse_t* configParse);
static void SPISendRecv(spiSendReceiveParse_t* configParse);


/*
 * This Function Handle the SPI protocol for Init/Config/RcvSend/Rst.
 * The protocol message format is:
 * Byte 0: Action (Init/Config/RcvSend/Rst).
 * Byte 1: SPI Number.
 * if (Action = Init):
 * 	Byte 2: Frequency(Byte 0).
 * 	Byte 3: Frequency(Byte 1).
 * 	Byte 4: Frequency(Byte 2).
 * 	Byte 5: Frequency(Byte 3).
 * 	Byte 6: Spi Mode
 * 	Byte 7: Chip Select Polarity
 * else if (Action = Config):
 * 	Byte 2: Frequency(Byte 0).
 * 	Byte 3: Frequency(Byte 1).
 * 	Byte 4: Frequency(Byte 2).
 * 	Byte 5: Frequency(Byte 3).
 * else if (Action = RcvSend):
 * 	Byte 2: Chip Select Pin.
 * 	Byte 3: Message to transmit first byte.
 * 	Byte n: Message to transmit last byte.
 */
void SPI_Handling(parse_vcom_t *vcom_msg){

	uint8_t action = vcom_msg->imsg[0];
	LPSPI_Type *SPIInstace = uintToLPSPIType(vcom_msg->imsg[1]);

	switch (action){
	case(SPI_REQ_INIT):

		spiInitParse_t initParse;
		initParse.SPIInstace=SPIInstace;
		initParse.frequency = BYTESTO32BIT(vcom_msg->imsg[5],vcom_msg->imsg[4],vcom_msg->imsg[3],vcom_msg->imsg[2]);
		initParse.mode =vcom_msg->imsg[6];
		initParse.pcs_polarity =vcom_msg->imsg[7];
		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		SPIinit(&initParse);

		//Prepare the response to acknowledge the host.
		vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
		vcom_msg->omsg[0]=1;
		vcom_msg->osize = 1;
		vcom_msg->otype = SPI_REQ;
		break;

	case(SPI_REQ_CONFIG):

		spiConfigParse_t configParse;
		configParse.SPIInstace = SPIInstace;
		configParse.frequency =  BYTESTO32BIT(vcom_msg->imsg[5],vcom_msg->imsg[4],vcom_msg->imsg[3],vcom_msg->imsg[2]);
		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		SPIConfig(&configParse);

		//Prepare the response to acknowledge the host.
		vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
		vcom_msg->omsg[0]=1;
		vcom_msg->osize = 1;
		vcom_msg->otype = SPI_REQ;
		break;

	case(SPI_REQ_SEND):

		uint8_t* TxBuf;
		uint8_t* RxBuf;
		spiSendReceiveParse_t SendReceiveParse;
		SendReceiveParse.SPIInstace = SPIInstace;
		SendReceiveParse.dataSize = vcom_msg->isize - 3;
		SendReceiveParse.pcs = vcom_msg->imsg[2];
		TxBuf = (uint8_t*)malloc(SendReceiveParse.dataSize * sizeof(uint8_t));
		RxBuf = (uint8_t*)malloc(SendReceiveParse.dataSize * sizeof(uint8_t));
		memcpy(TxBuf,&(vcom_msg->imsg[3]),SendReceiveParse.dataSize);
		SendReceiveParse.txData = TxBuf;
		SendReceiveParse.rxData = RxBuf;
		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		SPISendRecv(&SendReceiveParse);

		free(TxBuf);

		//Prepare the response to acknowledge the host.
		vcom_msg->omsg = (uint8_t*)malloc(SendReceiveParse.dataSize * sizeof(uint8_t));
		memcpy(vcom_msg->omsg,RxBuf,SendReceiveParse.dataSize);
		free(RxBuf);
		vcom_msg->osize = SendReceiveParse.dataSize;
		vcom_msg->otype = SPI_REQ;
		break;

	case(SPI_REQ_RESET):

		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		SPIReset(SPIInstace);

		//Prepare the response to acknowledge the host.
		vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
		vcom_msg->omsg[0]=1;
		vcom_msg->osize = 1;
		vcom_msg->otype = SPI_REQ;
		break;
	}
}

static void SPIReset(LPSPI_Type* instance){
	LPSPI_Deinit(instance);
	LPSPI_Reset(instance);
}

static void SPIConfig(spiConfigParse_t* configParse){
	uint32_t tcrPrescaleValue = 0;

	LPSPI_Enable(configParse->SPIInstace, 0);
	LPSPI_MasterSetBaudRate(configParse->SPIInstace, configParse->frequency, LP_FLEXCOMM1_CLOCK_FREQ, &tcrPrescaleValue);
	(configParse->SPIInstace)->TCR |= LPSPI_TCR_PRESCALE(tcrPrescaleValue);
	LPSPI_Enable(configParse->SPIInstace, 1);
}

static void SPISendRecv(spiSendReceiveParse_t* sendRcvParse){

	uint32_t pcsMask;
	switch(sendRcvParse->pcs){
	case(0):
		pcsMask = kLPSPI_MasterPcs0;
			break;
	case(1):
		pcsMask = kLPSPI_MasterPcs1;
			break;
	case(2):
		pcsMask = kLPSPI_MasterPcs2;
			break;
	case(3):
		pcsMask = kLPSPI_MasterPcs3;
			break;
	}

	lpspi_transfer_t txref={sendRcvParse->txData,sendRcvParse->rxData,
							sendRcvParse->dataSize, kLPSPI_MasterPcsContinuous|pcsMask};

	LPSPI_MasterTransferBlocking(sendRcvParse->SPIInstace,&txref);
}

static void SPIinit(spiInitParse_t *initParse){

	uint32_t timing = 1e9/(2*initParse->frequency);

	uint8_t pol;
	uint8_t pha;

	switch(initParse->mode){
	case(0):
		pol = 0;
		pha = 0;
			break;
	case(1):
		pol = 0;
		pha = 1;
			break;
	case(2):
		pol = 1;
		pha = 0;
			break;
	case(3):
		pol = 1;
		pha = 1;
			break;
	}

	const lpspi_master_config_t LP_FLEXCOMMx_config = {
	  .baudRate = initParse->frequency,
	  .bitsPerFrame = 8UL,
	  .cpol = pol,
	  .cpha = pha,
	  .direction = kLPSPI_MsbFirst,
	  .pcsToSckDelayInNanoSec = timing,
	  .lastSckToPcsDelayInNanoSec = timing,
	  .betweenTransferDelayInNanoSec = timing,
	  .whichPcs = kLPSPI_Pcs0,
	  .pcsActiveHighOrLow = initParse->pcs_polarity,
	  .pinCfg = kLPSPI_SdiInSdoOut,
	  .pcsFunc = kLPSPI_PcsAsCs,
	  .dataOutConfig = kLpspiDataOutRetained,
	  .enableInputDelay = false
	};

	LPSPI_MasterInit(initParse->SPIInstace, &LP_FLEXCOMMx_config, LP_FLEXCOMM1_CLOCK_FREQ);
	LPSPI_SetPCSContinous(initParse->SPIInstace, 1);
	LPSPI_Enable(initParse->SPIInstace, 1);
}

static LPSPI_Type* uintToLPSPIType (uint8_t instance){
	LPSPI_Type *spiPort;
	switch (instance){
	case (1):
		spiPort=(LPSPI_Type *)LP_FLEXCOMM1;
	break;
	default:
		spiPort=NULL;
	break;

	return spiPort;
	}
}
