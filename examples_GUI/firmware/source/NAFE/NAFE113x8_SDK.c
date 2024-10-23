/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "NAFE113x8_Register.h"
#include "NAFE113x8_SDK.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_debug_console.h" //LP
#include "systick_utils.h"//LP

static uint16_t NAFE113x8_CaptureSamplesSCCR(uint16_t count);
static uint16_t NAFE113x8_CaptureSamplesMCMR(uint16_t activeChannelCount, bool dataOut16, bool statusByteEn);
static uint8_t NAFE113x8_CountChannelsEnabled();

void NAFE113x8_DELAY_ms(uint32_t delay_ms);//LP

// Buffer used to store Rx Even bytes directly by DMA
AT_NONCACHEABLE_SECTION_INIT(uint8_t rx_bufEven[bufferDMASize]);
// Buffer used to store Rx Odd bytes directly by DMA
AT_NONCACHEABLE_SECTION_INIT(uint8_t rx_bufOdd[bufferDMASize]);


// SPI1 Rx DMA Handle.
edma_handle_t LP_FLEXCOMM1_RX_Handle;
// SPI1 Tx DMA Handle for Command.
edma_handle_t LP_FLEXCOMM1_TXCmd_Handle;
// SPI1 Tx DMA Handle (triggered by DRDY) used to transmit dummy bytes.
edma_handle_t LP_FLEXCOMM1_TXDummy_Handle;
// SPI1 Tx DMA Handle (triggered by SPI1 TxRequest) used to transmit dummy bytes.
edma_handle_t LP_FLEXCOMM1_TXDummy2_Handle;

// Flag used to sync with DMA process.
volatile bool SampleReceived = false;

// Counter used to keep track of processed sample.
uint32_t samplesBuffer[2048*16];
// Address of SPI1 Tx FIFO
uint32_t TxSpi1Adr;
// Address of SPI1 Rx FIFO
uint32_t RxSpi1Adr;

void NAFE113x8_InitNafe (){

	// Configuration for SPI1
	const lpspi_master_config_t LP_FLEXCOMM1_config = {
	  .baudRate = 11000000UL, //LP (1000000->11000000)
	  .bitsPerFrame = 8UL,
	  .cpol = kLPSPI_ClockPolarityActiveHigh,
	  .cpha = kLPSPI_ClockPhaseSecondEdge,
	  .direction = kLPSPI_MsbFirst,
	  .pcsToSckDelayInNanoSec = 50UL,
	  .lastSckToPcsDelayInNanoSec = 50UL,
	  .betweenTransferDelayInNanoSec = 50UL,
	  .whichPcs = LPSPI1_PCS,
	  .pcsActiveHighOrLow = kLPSPI_PcsActiveLow,
	  .pinCfg = kLPSPI_SdiInSdoOut,
	  .pcsFunc = kLPSPI_PcsAsCs,
	  .dataOutConfig = kLpspiDataOutRetained,
	  .enableInputDelay = false
	};

	// Base Configuration for DMA0
	edma_config_t DMA0_config = {
	  .enableMasterIdReplication = false,
	  .enableGlobalChannelLink = true,
	  .enableHaltOnError = true,
	  .enableDebugMode = false,
	  .enableRoundRobinArbitration = false
	};

	// Clear DMA configuration for all channels
	memset(DMA0_config.channelConfig, 0, FSL_FEATURE_EDMA_INSTANCE_CHANNELn(DMA0_DMA_BASEADDR) * sizeof(edma_channel_config_t *));

	// Clear DMA buffers.
    memset(rx_bufEven, 0, bufferDMASize*sizeof(*rx_bufEven));
    memset(rx_bufOdd, 0, bufferDMASize*sizeof(*rx_bufOdd));

    // SPI1 Init
    LPSPI_MasterInit(LP_FLEXCOMM1_PERIPHERAL, &LP_FLEXCOMM1_config, LP_FLEXCOMM1_CLOCK_FREQ);
    // SPI1 Set COnfigure Select
    LPSPI_SelectTransferPCS(LP_FLEXCOMM1_PERIPHERAL, LPSPI1_PCS);
    // SPI1 Enable
    LPSPI_Enable(LP_FLEXCOMM1_PERIPHERAL,1);
    // SPI1 Save Tx FIFO Address
	TxSpi1Adr = LPSPI_GetTxRegisterAddress(LP_FLEXCOMM1_PERIPHERAL);
	// SPI1 Save Rx FIFO Address
	RxSpi1Adr = LPSPI_GetRxRegisterAddress(LP_FLEXCOMM1_PERIPHERAL);

    //DMA0 initiation
    EDMA_Init(DMASPI1,&DMA0_config);

    // Set DMA Rx Request source to Flexcomm1 Rx.
    EDMA_SetChannelMux(DMASPI1,DMASPI1_CHRx,kDma0RequestMuxLpFlexcomm1Rx);
    EDMA_CreateHandle(&LP_FLEXCOMM1_RX_Handle,DMASPI1,DMASPI1_CHRx);
    // Set DMA Command Tx Request source to none(SW).
    EDMA_SetChannelMux(DMASPI1,DMASPI1_CHTxCmd,0);
    EDMA_CreateHandle(&LP_FLEXCOMM1_TXCmd_Handle,DMASPI1,DMASPI1_CHTxCmd);
    // Set DMA Dummy Tx Request Source to DRDY edge interrupt.
    EDMA_SetChannelMux(DMASPI1,DMASPI1_CHTxDummy,kDma0RequestMuxGpio0PinEventRequest0);
    EDMA_CreateHandle(&LP_FLEXCOMM1_TXDummy_Handle,DMASPI1,DMASPI1_CHTxDummy);
    // Set DMA Dummy2 Tx Request Source to Flexcomm1.
    EDMA_SetChannelMux(DMASPI1,DMASPI1_CHTxDummy2,kDma0RequestMuxLpFlexcomm1Tx);
    EDMA_CreateHandle(&LP_FLEXCOMM1_TXDummy2_Handle,DMASPI1,DMASPI1_CHTxDummy2);
}


void NAFE113x8_WriteCommand(uint16_t command, bool devID){

	// Tx Buffer with Command ID
	uint8_t txData[]={NAFE113x8_SPI_CMD_FORMAT_BYTE1(command), NAFE113x8_SPI_CMD_FORMAT_BYTE0(devID,command)};
	// Rx Dummy Buffer
	uint8_t rxData[2];
	// SPI Transaction description
	lpspi_transfer_t txref={txData,rxData,2,kLPSPI_MasterPcsContinuous|LPSPI1_PCS};
	// SPI Transaction Frame Size - 2 bytes in case of NAFE command format
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, 16);
	// Start a blocking SPI Transaction.
	LPSPI_MasterTransferBlocking(LP_FLEXCOMM1_PERIPHERAL,&txref);
	// Reset to default SPI Transaction Frame Size
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, 8);
}

void NAFE113x8_ReadRegister(const Register_t *RegDef, bool devID, uint32_t *regValue){

	// Tx Buffer with Register addressing plus 3 bytes to read back the value.
	uint8_t txData[]={NAFE113x8_SPI_READ_FORMAT_BYTE0(devID,RegDef->Address),
					  NAFE113x8_SPI_READ_FORMAT_BYTE1(RegDef->Address),
					  0,0,0};
	// Rx Buffer.
	uint8_t rxData[]={0,0,0,0,0};
	// SPI Transaction description
	lpspi_transfer_t txref={txData,rxData,(2 + RegDef->Bytes),kLPSPI_MasterPcsContinuous|LPSPI1_PCS|kLPSPI_MasterByteSwap};

	// SPI Transaction Frame Size - 2 or 3 bytes according to register size.
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, (2 + RegDef->Bytes) * 8);
	// Start a blocking SPI Transaction.
	LPSPI_MasterTransferBlocking(LP_FLEXCOMM1_PERIPHERAL,&txref);
	// Reset to default SPI Transaction Frame Size
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, 8);

	// Decode and store into regValue the register content.
	if (RegDef->Bytes == 2)
		*regValue = (rxData[2]<<8) | (rxData[3]);
	else
		*regValue = (rxData[2]<<16) | (rxData[3]<<8) | rxData[4];

}

void NAFE113x8_WriteRegister(const Register_t *RegDef, bool devID, uint32_t regValue){

	// Rx Dummy Buffer
	uint8_t rxData[5];
	// Tx Buffer with Register addressing plus 3 bytes for data.
	uint8_t txData[]={NAFE113x8_SPI_WRITE_FORMAT_BYTE0(devID,RegDef->Address),
				      NAFE113x8_SPI_WRITE_FORMAT_BYTE1(RegDef->Address),
					  0,0,0};
	// SPI Transaction description
	lpspi_transfer_t txref={txData,rxData,(2 + RegDef->Bytes),kLPSPI_MasterPcsContinuous|LPSPI1_PCS|kLPSPI_MasterByteSwap};

	// Decode and store into Tx Buffer according to register size.
	if (RegDef->Bytes == 2){
		txData[2] = (regValue>>8)&0xFF;
		txData[3] = (regValue>>0)&0xFF;
	}
	else{
		txData[2] = (regValue>>16)&0xFF;
		txData[3] = (regValue>>8)&0xFF;
		txData[4] = (regValue>>0)&0xFF;
	}

	// SPI Transaction Frame Size - 2 or 3 bytes according to register size.
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, (2 + RegDef->Bytes) * 8);
	// Start a blocking SPI Transaction.
	LPSPI_MasterTransferBlocking(LP_FLEXCOMM1_PERIPHERAL,&txref);
	// Reset to default SPI Transaction Frame Size
	LPSPI_SetFrameSize(LP_FLEXCOMM1_PERIPHERAL, 8);
}

void NAFE113x8_WriteField(const Field_t *FieldDef, bool devID, uint16_t fieldValue){

	assert(!(FieldDef->r_w == Read));
	assert((FieldDef->mask&fieldValue) == fieldValue);

	// Variable to store register value.
	uint32_t readBack;

	// Read Register Value
	NAFE113x8_ReadRegister(FieldDef->refRegister,devID,&readBack);

	// Apply new value according to old register content and register mask.
	readBack &= ~((FieldDef->mask)<<FieldDef->Offset);
	readBack |= fieldValue<<(FieldDef->Offset);

	// Update register value.
	NAFE113x8_WriteRegister(FieldDef->refRegister, devID, readBack);
}

void NAFE113x8_ReadField(const Field_t *FieldDef, bool devID, uint16_t *fieldValue){
	assert(!(FieldDef->r_w == Write));

	// Variable to store register value.
	uint32_t readBack;
	// Read Register Value
	NAFE113x8_ReadRegister(FieldDef->refRegister,devID,&readBack);
	// Apply mask to extract the request field
	*fieldValue = (readBack>>FieldDef->Offset)&(FieldDef->mask);
}

void NAFE113x8_selectChannel(uint8_t channel, bool devID){
	uint16_t cmd;
	switch (channel){
	case 0:
		cmd = CMD_CH0;
		break;
	case 1:
		cmd = CMD_CH1;
		break;
	case 2:
		cmd = CMD_CH2;
		break;
	case 3:
		cmd = CMD_CH3;
		break;
	case 4:
		cmd = CMD_CH4;
		break;
	case 5:
		cmd = CMD_CH5;
		break;
	case 6:
		cmd = CMD_CH6;
		break;
	case 7:
		cmd = CMD_CH7;
		break;
	case 8:
		cmd = CMD_CH8;
		break;
	case 9:
		cmd = CMD_CH9;
		break;
	case 10:
		cmd = CMD_CH10;
		break;
	case 11:
		cmd = CMD_CH11;
		break;
	case 12:
		cmd = CMD_CH12;
		break;
	case 13:
		cmd = CMD_CH13;
		break;
	case 14:
		cmd = CMD_CH14;
		break;
	case 15:
		cmd = CMD_CH15;
		break;
	default:
		return;
	}
	NAFE113x8_WriteCommand(cmd,devID);
}

static uint8_t NAFE113x8_CountChannelsEnabled(){

	// Variable to store the value of Config4 Register
	uint32_t activeChannelMask;
	// Counter for channels enabled.
	uint16_t activeChannelCount=0;

	NAFE113x8_ReadRegister(&ChConfig4,0,&activeChannelMask);

	// Count the current active channels.
	while (activeChannelMask){
		activeChannelCount += activeChannelMask&1;
		activeChannelMask>>=1;
	}

	return activeChannelCount;
}

static uint16_t NAFE113x8_CaptureSamplesSCCR(uint16_t count){
	uint16_t sampleCount = 0;
	while (sampleCount <count){
		NAFE113x8_DELAY_ms(1000);//LP
		if(SampleReceived){
			SampleReceived=false;
		if(sampleCount%2==0)
			samplesBuffer[sampleCount]=((rx_bufEven[0])<<16)|((rx_bufEven[1])<<8)|rx_bufEven[2];
	    else{
	    	samplesBuffer[sampleCount]=((rx_bufOdd[0])<<16)|((rx_bufOdd[1])<<8)|rx_bufOdd[2];
	    }
		sampleCount++;
		}
	}
	return sampleCount;
}

static uint16_t NAFE113x8_CaptureSamplesMCMR(uint16_t activeChannelCount, bool dataOut16, bool statusByteEn){
	uint16_t sampleCount=0;
	while (!SampleReceived);
	for (int i=0; i<activeChannelCount;i++) {
		NAFE113x8_DELAY_ms(1000);//LP
		if (dataOut16)
			samplesBuffer[i]=((rx_bufEven[0])<<16)|((rx_bufEven[1])<<8)|rx_bufEven[2]; // TODO
		else
			samplesBuffer[i]=((rx_bufEven[3*i])<<16)|((rx_bufEven[(3*i)+1])<<8)|rx_bufEven[(3*i)+2];
	}
	return sampleCount;
}

static uint16_t NAFE113x8_CaptureSamplesMCCR(uint16_t activeChannelCount, bool dataOut16, bool statusByteEn, uint16_t count){
	uint16_t sampleCount = 0;
	while (sampleCount <count){
		NAFE113x8_DELAY_ms(1000);//LP
		if (SampleReceived){
			SampleReceived=false;
			for (int i=0; i<activeChannelCount;i++) {
				if (dataOut16)
					samplesBuffer[i]=((rx_bufEven[0])<<16)|((rx_bufEven[1])<<8)|rx_bufEven[2]; // TODO
				else{
					if(sampleCount%2==0)
						samplesBuffer[(activeChannelCount * sampleCount) + i]=((rx_bufEven[3*i])<<16)|((rx_bufEven[(3*i)+1])<<8)|rx_bufEven[(3*i)+2];
					else
						samplesBuffer[(activeChannelCount * sampleCount) + i]=((rx_bufOdd[3*i])<<16)|((rx_bufOdd[(3*i)+1])<<8)|rx_bufOdd[(3*i)+2];
				}
			}
			sampleCount++;
		}

	}
	return sampleCount;
}

void NAFE113x8_DELAY_ms(uint32_t delay_ms)//LP (whole function)
{
	int32_t start, elapsed;
	uint32_t systemCoreClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	BOARD_SystickStart(&start);
	do // Loop for requested number of ms.
	{
		elapsed = BOARD_SystickElapsedTicks(&start);
	} while((COUNT_TO_MSEC(elapsed, systemCoreClock) < delay_ms) && !SampleReceived);
}

float NAFE113x8_VoltageTranslation(uint32_t value, uint8_t input , bool dataOut16, float gain){
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

	switch(input){
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

uint32_t NAFE113x8_getSample(uint16_t index){
	return samplesBuffer[index];
}

uint16_t NAFE113x8_doSCCR(uint8_t channel, bool devID, uint16_t count){

	// Dummy variable to use as 0 value.
	uint8_t dummyTx = 0;
	// Dummy variable to use as dummy pointer.
    uint8_t dummyRx = 0;
    uint16_t capturedSamples = 0;
    // Variable used to describe the TCD for all TCDs.
    edma_transfer_config_t edma_tc_description;
	// Buffer used to store Tx Command.
	AT_NONCACHEABLE_SECTION_INIT(uint8_t tx_buf[2]);

    // TCD triggered by software, used to send the start cmd.
    AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_cmd, EDMA_TCD_ALIGN_SIZE);
    // TCD triggered by DRDY, used to send the dummy bytes to clock the slave to return the converted value.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_txDummy, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to handle the dummy value returned by the tcd_cmd DMA transaction.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxDummy, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to store the Even Bytes,  this is active at the end of the previous one (tcd_rxDummy).
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxEven, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to store the Odd Bytes,  this is active at the end of the previous one (tcd_rxDummy).
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxOdd, EDMA_TCD_ALIGN_SIZE);

    // Decode and store the request command into transmit buffer.
    tx_buf[0]=NAFE113x8_SPI_CMD_FORMAT_BYTE0(0,CMD_SC);
    tx_buf[1]=NAFE113x8_SPI_CMD_FORMAT_BYTE1(CMD_SC);

    NAFE113x8_selectChannel(channel, devID);

    // Clear LPSPI Flags and FIFOs
    LPSPI_FlushFifo(LP_FLEXCOMM1_PERIPHERAL, true, true);
    LPSPI_ClearStatusFlags(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllInterruptEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_RxDmaEnable | (uint32_t)kLPSPI_TxDmaEnable);

    // TCD configurations common to all the TCDs described below.
    edma_tc_description.srcAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.dstAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.enableChannelMinorLoopLink=false;
	edma_tc_description.enableChannelMajorLoopLink=false;
	edma_tc_description.srcTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.destTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.enableDstMinorLoopOffset = false;
	edma_tc_description.enableSrcMinorLoopOffset = false;
	edma_tc_description.dstMajorLoopOffset = 0;
	edma_tc_description.srcMajorLoopOffset = 0;

	// TCD triggered by SW configuration.
    edma_tc_description.srcAddr=(uint32_t)tx_buf;
    edma_tc_description.srcOffset=1;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=2;
    edma_tc_description.majorLoopCounts=1;
    edma_tc_description.enabledInterruptMask=0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_cmd);
    EDMA_ConfigChannelSoftwareTCD(&tcd_cmd, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for dummyRx (link to tcd_rx_Even).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&dummyRx;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=1;
    edma_tc_description.majorLoopCounts=2;
    edma_tc_description.enabledInterruptMask= 0;
    edma_tc_description.linkTCD = &tcd_rxEven;
    EDMA_TcdReset(&tcd_rxDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxDummy, &edma_tc_description);

    // TCD triggered by DRDY.
    edma_tc_description.srcAddr=(uint32_t)&dummyTx;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=3;
    edma_tc_description.majorLoopCounts=count;
    edma_tc_description.enabledInterruptMask=0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_txDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_txDummy, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for store the Rx Even bytes.
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&rx_bufEven[0];
    edma_tc_description.destOffset=1;
    edma_tc_description.minorLoopBytes=1;
    edma_tc_description.majorLoopCounts=3;
    edma_tc_description.enabledInterruptMask= kEDMA_MajorInterruptEnable;
    edma_tc_description.linkTCD = &tcd_rxOdd;
    EDMA_TcdReset(&tcd_rxEven);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxEven, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for store the Rx Odd bytes .
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&rx_bufOdd[0];
    edma_tc_description.destOffset=1;
    edma_tc_description.minorLoopBytes=1;
    edma_tc_description.majorLoopCounts=3;
    edma_tc_description.enabledInterruptMask= kEDMA_MajorInterruptEnable;
    edma_tc_description.linkTCD = &tcd_rxEven;
    EDMA_TcdReset(&tcd_rxOdd);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxOdd, &edma_tc_description);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxCmd, &tcd_cmd);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxCmd, true);

    // Install TCD and Disable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHRx, &tcd_rxDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, 0, false);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxDummy, &tcd_txDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxDummy, true);

    EnableIRQ(EDMA_0_CH0_IRQn);//IRQ needed to sync with received samples.

    // Clear all interrupt flag for all DMA CH.
    EDMA_ClearChannelStatusFlags(DMA0,0,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,1,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,2,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,3,kEDMA_InterruptFlag);

    // Enable DMA for LPSPI1.
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);

    // Set PCS in continuos mode, CS remain low till the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,1);

    EDMA_StartTransfer(&LP_FLEXCOMM1_RX_Handle);
    EDMA_StartTransfer(&LP_FLEXCOMM1_TXDummy_Handle);
    EDMA_TriggerChannelStart(DMASPI1, DMASPI1_CHTxCmd);

    capturedSamples = NAFE113x8_CaptureSamplesSCCR(count);

    // Set PCS at the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,0);
    // Stop the DMA channel, the only channel with the autostop disable.
    EDMA_StopTransfer(&LP_FLEXCOMM1_RX_Handle);
    NAFE113x8_WriteCommand(CMD_ADC_ABORT,0);
    // Disable DMA for LPSPI1.
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);
    DisableIRQ(EDMA_0_CH0_IRQn);

    //Clear Flag
    SampleReceived = false;

    return capturedSamples;
}

uint16_t NAFE113x8_doMCMR(bool devID){

	// Variable to store the interrupt behavior for Tx Dummy TCD.
	uint32_t tcd_txDummy_Interrupt;
	// Dummy variable to use as 0 value.
	uint8_t dummyTx = 0;
	// Dummy variable to use as dummy pointer.
    uint8_t dummyRx = 0;
    // Major Loop Count for Tx Dummy TCD.
    uint8_t MajorLoopCount_tcd_txDummy = 0;
    // Minor Loop Count for Tx Dummy TCD.
    uint8_t MinorLoopCount_tcd_txDummy = 0;
    // Major Loop Count for Tx Dummy second TCD.
    uint8_t MajorLoopCount_tcd_txDummy2 = 0;
    // Minor Loop Count for Tx Dummy second TCD.
    uint8_t MinorLoopCount_tcd_txDummy2 = 0;
    // Major Loop Count for Rx TCD.
    uint8_t MajorLoopCount_tcd_rx = 0;
    // Minor Loop Count for Rx TCD.
    uint8_t MinorLoopCount_tcd_rx = 0;

    uint16_t capturedSamples = 0;

    // Variable to store the DRDY_PIN_EDGE value.
    uint16_t drdyEdgeMode=0;
    // Variable to store the active channel count.
    uint16_t activeChannelCount=0;
    // Variable used to describe the TCD for all TCDs.
    edma_transfer_config_t edma_tc_description;
	// Buffer used to store Tx Command.
	AT_NONCACHEABLE_SECTION_INIT(uint8_t tx_buf[2]);

    // TCD triggered by software, used to send the start cmd.
    AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_cmd, EDMA_TCD_ALIGN_SIZE);
    // TCD triggered by DRDY, used to send the dummy bytes to clock the slave to return the converted value.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_txDummy, EDMA_TCD_ALIGN_SIZE);
	/* TCD triggered by FlexcommTx, used to send the dummy bytes to clock the slave to return the converted value.
	   This TCD is used in case tcd_txDummy could risk filling the entire SPI FIFO */
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_txDummy2, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to handle the dummy value returned by the tcd_cmd DMA transaction.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxDummy, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to store the Conversion Data,  this is active at the end of the previous one (tcd_rxDummy).
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rx, EDMA_TCD_ALIGN_SIZE);


    activeChannelCount = NAFE113x8_CountChannelsEnabled();

    NAFE113x8_ReadField(&SysConfig0_DrdyPinEdge,0,&drdyEdgeMode);
    switch(drdyEdgeMode){

    case DRDYPINEDGE_EVERYCH:
    	/* Configuration of TCDs in case of DRDY is issued for every CH conversion
    	 * DMA write 3 bytes (value 0) each DRDY pulse
    	 * NO interrupt at the expiration of tcd_txDummy
    	 * tcd_rx need to store 3 bytes for each active channels */

		MajorLoopCount_tcd_txDummy = activeChannelCount;
		MinorLoopCount_tcd_txDummy = 3;
    	MajorLoopCount_tcd_rx = activeChannelCount*3;
    	MinorLoopCount_tcd_rx = 1;
    	tcd_txDummy_Interrupt = 0;

    break;

    case DRDYPINEDGE_LASTCH:

    	if (activeChannelCount<3){
        	/* Configuration of TCDs in case of DRDY is issued at the end of all ch conversion,
        	 * there is no risk of overflow the FIFO.
        	 * DMA write activeChannelCount*3 bytes (value 0) after DRDY.
        	 * NO interrupt at the expiration of tcd_txDummy  */

			MajorLoopCount_tcd_txDummy = 1;
			MinorLoopCount_tcd_txDummy = activeChannelCount*3;
			 tcd_txDummy_Interrupt = 0;
    	}
    	else{
        	/* Configuration of TCDs in case of DRDY is issued at the end of all ch conversion,
        	 * there is risk of overflow the FIFO.
        	 * DMA write 6 bytes (value 0) after DRDY with tcd_txDummy .
        	 * DMA write remaining bytes (value 0) at each FlexcommTx Request with tcd_txDummy2.
        	 * An interrupt at the expiration of tcd_txDummy is issued to start the tcd_txDummy2 */

			MajorLoopCount_tcd_txDummy = 1;
			MinorLoopCount_tcd_txDummy = 2 * 3;
			MajorLoopCount_tcd_txDummy2 = (activeChannelCount-2)*3;
			MinorLoopCount_tcd_txDummy2 = 1;
			tcd_txDummy_Interrupt = kEDMA_MajorInterruptEnable;
    	}

    	MajorLoopCount_tcd_rx = activeChannelCount*3;
    	MinorLoopCount_tcd_rx = 1;
    break;
    }

    // Decode and store the request command into transmit buffer.
    tx_buf[0]=NAFE113x8_SPI_CMD_FORMAT_BYTE0(0,CMD_MM);
    tx_buf[1]=NAFE113x8_SPI_CMD_FORMAT_BYTE1(CMD_MM);

    // Clear LPSPI Flags and FIFOs
    LPSPI_FlushFifo(LP_FLEXCOMM1_PERIPHERAL, true, true);
    LPSPI_ClearStatusFlags(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllInterruptEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_RxDmaEnable | (uint32_t)kLPSPI_TxDmaEnable);

    // TCD configurations common to all the TCDs described below.
    edma_tc_description.srcAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.dstAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.enableChannelMinorLoopLink=false;
	edma_tc_description.enableChannelMajorLoopLink=false;
	edma_tc_description.srcTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.destTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.enableDstMinorLoopOffset = false;
	edma_tc_description.enableSrcMinorLoopOffset = false;
	edma_tc_description.dstMajorLoopOffset = 0;
	edma_tc_description.srcMajorLoopOffset = 0;

	// TCD triggered by SW configuration.
    edma_tc_description.srcAddr=(uint32_t)tx_buf;
    edma_tc_description.srcOffset=1;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=2;
    edma_tc_description.majorLoopCounts=1;
    edma_tc_description.enabledInterruptMask=0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_cmd);
    EDMA_ConfigChannelSoftwareTCD(&tcd_cmd, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for dummyRx (link to tcd_rx).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&dummyRx;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=1;
    edma_tc_description.majorLoopCounts=2;
    edma_tc_description.enabledInterruptMask= 0;
    edma_tc_description.linkTCD = &tcd_rx;
    EDMA_TcdReset(&tcd_rxDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxDummy, &edma_tc_description);

    // TCD triggered by DRDY.
    edma_tc_description.srcAddr=(uint32_t)&dummyTx;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_txDummy;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_txDummy;
    edma_tc_description.enabledInterruptMask=tcd_txDummy_Interrupt;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_txDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_txDummy, &edma_tc_description);

    // TCD triggered by Tx FlexComm request, this is started only when Major loop of tcd_txDummy expire.
    edma_tc_description.srcAddr=(uint32_t)&dummyTx;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_txDummy2;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_txDummy2;
    edma_tc_description.enabledInterruptMask=0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_txDummy2);
    EDMA_ConfigChannelSoftwareTCD(&tcd_txDummy2, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for store the Rx bytes (start when Major loop of tcd_rxDummy expire).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&rx_bufEven[0];
    edma_tc_description.destOffset=1;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_rx;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_rx;
    edma_tc_description.enabledInterruptMask= kEDMA_MajorInterruptEnable;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_rx);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rx, &edma_tc_description);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxCmd, &tcd_cmd);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxCmd, true);

    // Install TCD and Disable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHRx, &tcd_rxDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, 0, false);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxDummy, &tcd_txDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxDummy, true);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxDummy2, &tcd_txDummy2);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxDummy2, true);

    EnableIRQ(EDMA_0_CH0_IRQn);//IRQ needed to sync with received samples.
    EnableIRQ(EDMA_0_CH2_IRQn);//IRQ needed to start tcd_txDummy2 if needed.

    // Clear all interrupt flag for all DMA CH.
    EDMA_ClearChannelStatusFlags(DMA0,0,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,1,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,2,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,3,kEDMA_InterruptFlag);

    // Enable DMA for LPSPI1.
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);

    // Set PCS in continuos mode, CS remain low till the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,1);

    EDMA_StartTransfer(&LP_FLEXCOMM1_RX_Handle);
    EDMA_StartTransfer(&LP_FLEXCOMM1_TXDummy_Handle);
    EDMA_TriggerChannelStart(DMASPI1, DMASPI1_CHTxCmd);

    capturedSamples = NAFE113x8_CaptureSamplesMCMR(activeChannelCount,false,false);

    // Set PCS at the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,0);
    // Stop the DMA channel, the only channel with the autostop disable.
    EDMA_StopTransfer(&LP_FLEXCOMM1_RX_Handle);
    // Disable DMA for LPSPI1.
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);
    DisableIRQ(EDMA_0_CH0_IRQn);
    DisableIRQ(EDMA_0_CH2_IRQn);
    //Clear Flag
    SampleReceived = false;

    return capturedSamples;
}

uint16_t NAFE113x8_doMCCR(bool devID, uint16_t count){

	// Variable to store the interrupt behavior for Tx Dummy TCD.
	uint32_t tcd_txDummy_Interrupt;
	// Dummy variable to use as 0 value.
	uint8_t dummyTx = 0;
	// Dummy variable to use as dummy pointer.
    uint8_t dummyRx = 0;

    uint16_t capturedSamples = 0;

    // Major Loop Count for Tx Dummy TCD.
    uint8_t MajorLoopCount_tcd_txDummy = 0;
    // Minor Loop Count for Tx Dummy TCD.
    uint8_t MinorLoopCount_tcd_txDummy = 0;
    // Major Loop Count for Tx Dummy second TCD.
    uint8_t MajorLoopCount_tcd_txDummy2 = 0;
    // Minor Loop Count for Tx Dummy second TCD.
    uint8_t MinorLoopCount_tcd_txDummy2 = 0;
    // Major Loop Count for Rx TCD.
    uint8_t MajorLoopCount_tcd_rx = 0;
    // Minor Loop Count for Rx TCD.
    uint8_t MinorLoopCount_tcd_rx = 0;
    // Variable to store the DRDY_PIN_EDGE value.
    uint16_t drdyEdgeMode=0;
    // Variable to store the active channel count.
    uint16_t activeChannelCount=0;
    // Variable used to describe the TCD for all TCDs.
    edma_transfer_config_t edma_tc_description;
	// Buffer used to store Tx Command.
	AT_NONCACHEABLE_SECTION_INIT(uint8_t tx_buf[2]);

    // TCD triggered by software, used to send the start cmd.
    AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_cmd, EDMA_TCD_ALIGN_SIZE);
    // TCD triggered by DRDY, used to send the dummy bytes to clock the slave to return the converted value.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_txDummy, EDMA_TCD_ALIGN_SIZE);
	/* TCD triggered by FlexcommTx, used to send the dummy bytes to clock the slave to return the converted value.
	   This TCD is used in case tcd_txDummy could risk filling the entire SPI FIFO */
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_txDummy2, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to handle the dummy value returned by the tcd_cmd DMA transaction.
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxDummy, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to store the Conversion Data,  this is active at the end of the previous one (tcd_rxDummy).
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxEven, EDMA_TCD_ALIGN_SIZE);
	// TCD Triggered by FlexcommRx used to store the Conversion Data..
	AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t tcd_rxOdd, EDMA_TCD_ALIGN_SIZE);


    activeChannelCount = NAFE113x8_CountChannelsEnabled();

    NAFE113x8_ReadField(&SysConfig0_DrdyPinEdge,0,&drdyEdgeMode);
    switch(drdyEdgeMode){

    case DRDYPINEDGE_EVERYCH:
    	/* Configuration of TCDs in case of DRDY is issued for every CH conversion
    	 * DMA write 3 bytes (value 0) each DRDY pulse
    	 * NO interrupt at the expiration of tcd_txDummy
    	 * tcd_rx need to store 3 bytes for each active channels */

		MajorLoopCount_tcd_txDummy = activeChannelCount * count;
		MinorLoopCount_tcd_txDummy = 3;
    	MajorLoopCount_tcd_rx = 3 * activeChannelCount;
    	MinorLoopCount_tcd_rx = 1;
    	tcd_txDummy_Interrupt = 0;

    break;

    case DRDYPINEDGE_LASTCH:

    	if (activeChannelCount<3){
        	/* Configuration of TCDs in case of DRDY is issued at the end of all ch conversion,
        	 * there is no risk of overflow the FIFO.
        	 * DMA write activeChannelCount*3 bytes (value 0) after DRDY.
        	 * NO interrupt at the expiration of tcd_txDummy  */

			MajorLoopCount_tcd_txDummy = 1 * count;
			MinorLoopCount_tcd_txDummy = activeChannelCount*3;
			 tcd_txDummy_Interrupt = 0;
    	}
    	else{
        	/* Configuration of TCDs in case of DRDY is issued at the end of all ch conversion,
        	 * there is risk of overflow the FIFO.
        	 * DMA write 6 bytes (value 0) after DRDY with tcd_txDummy .
        	 * DMA write remaining bytes (value 0) at each FlexcommTx Request with tcd_txDummy2.
        	 * An interrupt at the expiration of tcd_txDummy is issued to start the tcd_txDummy2 */

			MajorLoopCount_tcd_txDummy = 1;
			MinorLoopCount_tcd_txDummy = 2 * 3;
			MajorLoopCount_tcd_txDummy2 = (activeChannelCount-2)*3;
			MinorLoopCount_tcd_txDummy2 = 1;
			tcd_txDummy_Interrupt = kEDMA_MajorInterruptEnable;
    	}

    	MajorLoopCount_tcd_rx = activeChannelCount*3;
    	MinorLoopCount_tcd_rx = 1;
    break;
    }

    // Decode and store the request command into transmit buffer.
    tx_buf[0]=NAFE113x8_SPI_CMD_FORMAT_BYTE0(0,CMD_MC);
    tx_buf[1]=NAFE113x8_SPI_CMD_FORMAT_BYTE1(CMD_MC);

    // Clear LPSPI Flags and FIFOs
    LPSPI_FlushFifo(LP_FLEXCOMM1_PERIPHERAL, true, true);
    LPSPI_ClearStatusFlags(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_AllInterruptEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL, (uint32_t)kLPSPI_RxDmaEnable | (uint32_t)kLPSPI_TxDmaEnable);

    // TCD configurations common to all the TCDs described below.
    edma_tc_description.srcAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.dstAddrModulo=kEDMA_ModuloDisable;
	edma_tc_description.enableChannelMinorLoopLink=false;
	edma_tc_description.enableChannelMajorLoopLink=false;
	edma_tc_description.srcTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.destTransferSize=kEDMA_TransferSize1Bytes;
	edma_tc_description.enableDstMinorLoopOffset = false;
	edma_tc_description.enableSrcMinorLoopOffset = false;
	edma_tc_description.dstMajorLoopOffset = 0;
	edma_tc_description.srcMajorLoopOffset = 0;

	// TCD triggered by SW configuration.
    edma_tc_description.srcAddr=(uint32_t)tx_buf;
    edma_tc_description.srcOffset=1;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=2;
    edma_tc_description.majorLoopCounts=1;
    edma_tc_description.enabledInterruptMask=0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_cmd);
    EDMA_ConfigChannelSoftwareTCD(&tcd_cmd, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for dummyRx (link to tcd_rx).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&dummyRx;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=1;
    edma_tc_description.majorLoopCounts=2;
    edma_tc_description.enabledInterruptMask= 0;
    edma_tc_description.linkTCD = &tcd_rxEven;
    EDMA_TcdReset(&tcd_rxDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxDummy, &edma_tc_description);

    // TCD triggered by DRDY.
    edma_tc_description.srcAddr=(uint32_t)&dummyTx;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_txDummy;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_txDummy;
    edma_tc_description.enabledInterruptMask=tcd_txDummy_Interrupt;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_txDummy);
    EDMA_ConfigChannelSoftwareTCD(&tcd_txDummy, &edma_tc_description);

    // TCD triggered by Tx FlexComm request, this is started only when Major loop of tcd_txDummy expire.
    edma_tc_description.srcAddr=(uint32_t)&dummyTx;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=TxSpi1Adr;
    edma_tc_description.destOffset=0;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_txDummy2;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_txDummy2;
    edma_tc_description.enabledInterruptMask= 0;
    edma_tc_description.linkTCD = NULL;
    EDMA_TcdReset(&tcd_txDummy2);
    EDMA_ConfigChannelSoftwareTCD(&tcd_txDummy2, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for store the Rx bytes (start when Major loop of tcd_rxDummy expire).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&rx_bufEven[0];
    edma_tc_description.destOffset=1;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_rx;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_rx;
    edma_tc_description.enabledInterruptMask= kEDMA_MajorInterruptEnable;
    edma_tc_description.linkTCD = &tcd_rxOdd;
    EDMA_TcdReset(&tcd_rxEven);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxEven, &edma_tc_description);

    // TCD triggered by Rx Flexcomm request for store the Rx bytes (start when Major loop of tcd_rxDummy expire).
    edma_tc_description.srcAddr=RxSpi1Adr;
    edma_tc_description.srcOffset=0;
    edma_tc_description.destAddr=(uint32_t)&rx_bufOdd[0];
    edma_tc_description.destOffset=1;
    edma_tc_description.minorLoopBytes=MinorLoopCount_tcd_rx;
    edma_tc_description.majorLoopCounts=MajorLoopCount_tcd_rx;
    edma_tc_description.enabledInterruptMask= kEDMA_MajorInterruptEnable;
    edma_tc_description.linkTCD = &tcd_rxEven;
    EDMA_TcdReset(&tcd_rxOdd);
    EDMA_ConfigChannelSoftwareTCD(&tcd_rxOdd, &edma_tc_description);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxCmd, &tcd_cmd);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxCmd, true);

    // Install TCD and Disable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHRx, &tcd_rxDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, 0, false);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxDummy, &tcd_txDummy);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxDummy, false);

    // Install TCD and Enable Auto Stop Auto Mode.
    EDMA_InstallTCD(DMASPI1, DMASPI1_CHTxDummy2, &tcd_txDummy2);
    EDMA_EnableAutoStopRequest(DMASPI1, DMASPI1_CHTxDummy2, true);

    EnableIRQ(EDMA_0_CH0_IRQn);//IRQ needed to sync with received samples.
    EnableIRQ(EDMA_0_CH2_IRQn);//IRQ needed to start tcd_txDummy2 if needed.

    // Clear all interrupt flag for all DMA CH.
    EDMA_ClearChannelStatusFlags(DMA0,0,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,1,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,2,kEDMA_InterruptFlag);
    EDMA_ClearChannelStatusFlags(DMA0,3,kEDMA_InterruptFlag);

    // Enable DMA for LPSPI1.
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_EnableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);

    // Set PCS in continuos mode, CS remain low till the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,1);

    EDMA_StartTransfer(&LP_FLEXCOMM1_RX_Handle);
    EDMA_StartTransfer(&LP_FLEXCOMM1_TXDummy_Handle);
    EDMA_TriggerChannelStart(DMASPI1, DMASPI1_CHTxCmd);

    capturedSamples = NAFE113x8_CaptureSamplesMCCR(activeChannelCount,false,false,count);

    // Set PCS at the end of trasmission.
    LPSPI_SetPCSContinous(LP_FLEXCOMM1_PERIPHERAL,0);
    // Stop the DMA channel, the only channel with the autostop disable.
    EDMA_StopTransfer(&LP_FLEXCOMM1_TXDummy_Handle);
    EDMA_StopTransfer(&LP_FLEXCOMM1_RX_Handle);
    NAFE113x8_WriteCommand(CMD_ADC_ABORT,0);
    // Disable DMA for LPSPI1.
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_RxDmaEnable);
    LPSPI_DisableDMA(LP_FLEXCOMM1_PERIPHERAL,kLPSPI_TxDmaEnable);
    DisableIRQ(EDMA_0_CH0_IRQn);
    DisableIRQ(EDMA_0_CH2_IRQn);
    //Clear Flag
    SampleReceived = false;

    return capturedSamples;
}

void clearReceivedFlag(void){
	SampleReceived = false;
}

bool getReceivedFlag(void){
	return SampleReceived;
}

void enableDRDYHasGPIOInterrupt(bool enable){
	if(enable)
	{
		GPIO_SetPinInterruptConfig(BOARD_INITPINS_DUT_ADCDRDY_GPIO, BOARD_INITPINS_DUT_ADCDRDY_PIN, kGPIO_InterruptRisingEdge);
		EnableIRQ(GPIO00_IRQn);
	}
	else{
		GPIO_SetPinInterruptConfig(BOARD_INITPINS_DUT_ADCDRDY_GPIO, BOARD_INITPINS_DUT_ADCDRDY_PIN, kGPIO_DMARisingEdge);
		DisableIRQ(GPIO00_IRQn);
	}
}

void GPIO00_IRQHandler(void){
	GPIO0->ISFR[0] = BOARD_INITPINS_DUT_ADCDRDY_PIN_MASK;
	SampleReceived = true;
}

void EDMA_0_CH0_IRQHandler(void){
	DMA_CLEAR_INT_STATUS(DMASPI1, DMASPI1_CHRx);
	SampleReceived = true;
}

void EDMA_0_CH2_IRQHandler(void){
	DMA_CLEAR_INT_STATUS(DMASPI1, DMASPI1_CHTxDummy);
	EDMA_StartTransfer(&LP_FLEXCOMM1_TXDummy2_Handle);
}



