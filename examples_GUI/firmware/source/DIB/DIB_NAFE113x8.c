/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DIB_NAFE113x8.h"

static void NAFE_Init();
static void NAFE_ReadRegister(NAFE113x8ReadWriteParse_t* ReadParse);
static void NAFE_WriteRegister(NAFE113x8ReadWriteParse_t* WriteParse);
static void NAFE_SendCommand(uint8_t commandID, bool devID);

/*
 * NAFE Parser
 */
void NAFE113x8_Handling(parse_vcom_t *vcom_msg){
	uint8_t action = vcom_msg->imsg[0];
	Register_t Register;
	uint32_t registerValue;
	uint16_t count = 0;
	bool devID = 0;
	uint16_t commandID=0;
	uint8_t channel;
	uint16_t returnedSamples;
	uint8_t bit16;
	uint16_t offset;

	switch (action){

	case(NAFE113x8_REQ_INIT):
			/*
			 * Request to initialize MCU SPI and GPIOs to comunicate with NAFE113x8
			 */
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			NAFE_Init();

			//Prepare the response to acknowledge the host.
			vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
			vcom_msg->omsg[0]=1;
			vcom_msg->osize = 1;
			vcom_msg->otype = NAFE113x8_REQ;
			break;

/*	case(NAFE113x8_REQ_RESET):
			break;*/
	case(NAFE113x8_REQ_READREG):
			/*
			 *  Request of Register Reading
			 */
			NAFE113x8ReadWriteParse_t ReadParse = {.Register.Address = ((vcom_msg->imsg[1])<<8) | ((vcom_msg->imsg[2])),
												   .Register.Bytes = vcom_msg->imsg[3]};
			ReadParse.devID = vcom_msg->imsg[4];
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			NAFE_ReadRegister(&ReadParse);

			//Prepare the response to acknowledge the host.
			vcom_msg->omsg = (uint8_t*)malloc(ReadParse.Register.Bytes * sizeof(uint8_t));
			vcom_msg->omsg[0] = (uint8_t)(ReadParse.value & 0xFF);
			vcom_msg->omsg[1] = (uint8_t)((ReadParse.value >> 8) & 0xFF);
			if (ReadParse.Register.Bytes == 3)
				vcom_msg->omsg[2] = (uint8_t)((ReadParse.value >> 16) & 0xFF);
			vcom_msg->osize = ReadParse.Register.Bytes;
			vcom_msg->otype = NAFE113x8_REQ;
			break;

	case(NAFE113x8_REQ_WRITEREG):
			/*
			 * Request of Register Writing
			 */
			NAFE113x8ReadWriteParse_t WriteParse = {.Register.Address = ((uint16_t)(vcom_msg->imsg[1])<<8) | ((uint16_t)(vcom_msg->imsg[2])),
												   .Register.Bytes = vcom_msg->imsg[3]};
			WriteParse.devID = vcom_msg->imsg[4];
			if(WriteParse.Register.Bytes == 3)
				WriteParse.value = ((uint32_t)(vcom_msg->imsg[5])<<16) | ((uint32_t)(vcom_msg->imsg[6])<<8) | (uint32_t)(vcom_msg->imsg[7]);
			else if(WriteParse.Register.Bytes == 2)
				WriteParse.value = ((uint32_t)(vcom_msg->imsg[5])<<8) | (uint32_t)(vcom_msg->imsg[6]);
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			NAFE_WriteRegister(&WriteParse);

			//Prepare the response to acknowledge the host.
			vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
			vcom_msg->omsg[0]=1;
			vcom_msg->osize = 1;
			vcom_msg->otype = NAFE113x8_REQ;
			break;

	case(NAFE113x8_REQ_READFIELD):
			break;
	case(NAFE113x8_REQ_WRITEFIELD):
			break;
	case(NAFE113x8_REQ_SENDCMD):
			/*
			 * Request to Send A command.
			 */
			uint8_t commandID = vcom_msg->imsg[1];
			bool devID = vcom_msg->imsg[2];
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			NAFE_SendCommand(commandID, devID);

			//Prepare the response to acknowledge the host.
			vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
			vcom_msg->omsg[0]=1;
			vcom_msg->osize = 1;
			vcom_msg->otype = NAFE113x8_REQ;
			break;
	case(NAFE113x8_REQ_SCSR):
					/*
					 * Request to start SCSR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					channel = vcom_msg->imsg[2];
					free(vcom_msg->imsg);
					vcom_msg->isize = 0;

					Register.Address = 0x40 + (channel);
					Register.Bytes = 3;

					enableDRDYHasGPIOInterrupt(true);
					clearReceivedFlag();
					NAFE113x8_WriteCommand(CMD_SS, false);
					while(!getReceivedFlag());
					NAFE113x8_ReadRegister(&Register, devID, &registerValue);
					enableDRDYHasGPIOInterrupt(false);

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 3;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= registerValue & 0xFF;
					vcom_msg->omsg[1]= (registerValue>>8) & 0xFF;
					vcom_msg->omsg[2]= (registerValue>>16) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;
					break;

	case(NAFE113x8_REQ_SCCR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					channel = vcom_msg->imsg[2];
					count = (vcom_msg->imsg[4])<<8  | (vcom_msg->imsg[3]);
					free(vcom_msg->imsg);
					vcom_msg->isize = 0;

					returnedSamples = NAFE113x8_doSCCR(channel, devID,count);

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;
					break;

	case(NAFE113x8_REQ_MCMR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					free(vcom_msg->imsg);
					vcom_msg->isize = 0;

					returnedSamples = NAFE113x8_doMCMR(devID);

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;
					break;

	case(NAFE113x8_REQ_MCCR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					count = (vcom_msg->imsg[3])<<8  | (vcom_msg->imsg[2]);
					free(vcom_msg->imsg);
					vcom_msg->isize = 0;

					returnedSamples = NAFE113x8_doMCCR(devID,count);

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;
					break;

	case(NAFE113x8_REQ_SAMPLES):

			bit16 = vcom_msg->imsg[1];
			offset = (vcom_msg->imsg[3])<<8  | (vcom_msg->imsg[2]);
			count = (vcom_msg->imsg[5])<<8  | (vcom_msg->imsg[4]);
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			if (bit16 == 0)
				vcom_msg->osize = count * 3;
			else
				vcom_msg->osize = count * 2;

			vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
			for (int i=0;i<count; i++ ){
				if (bit16 == 0){
					vcom_msg->omsg[i*3] = (uint8_t)((NAFE113x8_getSample(i + offset)>>16) & 0xFF);
					vcom_msg->omsg[(i*3) + 1] = (uint8_t)((NAFE113x8_getSample(i + offset)>>8) & 0xFF);
					vcom_msg->omsg[(i*3) + 2] = (uint8_t)(NAFE113x8_getSample(i + offset) & 0xFF);
				}
				else{
					vcom_msg->omsg[i*2] = (uint8_t)((NAFE113x8_getSample(i + offset)>>8) & 0xFF);
					vcom_msg->omsg[(i*2)] = (uint8_t)(NAFE113x8_getSample(i + offset) & 0xFF);
				}
			}
			vcom_msg->otype = NAFE113x8_REQ;



			break;
}
}

/*
 * Initialize all the MCU peripherals in order to enable the comunication with NAFE
 */
static void NAFE_Init(){
	NAFE113x8_InitNafe();
}

/*
 * Read NAFE Register
 */
static void NAFE_ReadRegister(NAFE113x8ReadWriteParse_t* ReadParse){
	NAFE113x8_ReadRegister(&(ReadParse->Register), ReadParse->devID, &(ReadParse->value));
}

/*
 * Write NAFE Register
 */
static void NAFE_WriteRegister(NAFE113x8ReadWriteParse_t* WriteParse){
	NAFE113x8_WriteRegister(&(WriteParse->Register),WriteParse->devID,WriteParse->value);
}

/*
 * Send NAFE Command
 */
static void NAFE_SendCommand(uint8_t commandID, bool devID){
	NAFE113x8_WriteCommand(commandID, devID );
}
