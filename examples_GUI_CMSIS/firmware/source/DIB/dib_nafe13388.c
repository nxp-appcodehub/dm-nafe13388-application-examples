/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "dib_nafe13388.h"
#include "issdk_hal.h"
#include "nafe13388_sdk.h"
#include "frdmmcxn947.h"

static void NAFE_Init();
nafe13388_sensorhandle_t nafeDriver;
char buf_read_write[1024];
uint32_t samplesBuffer[2048*16];
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
	uint16_t Address;
	uint8_t length = 3;
	uint8_t output[3];
	uint8_t buf[length];
	uint32_t value;
	float gain[1] = {0.2};
	uint8_t active_channels = 0;
	uint8_t active[2] = {0};
	uint32_t activeChannelMask;
	uint16_t activeChannelCount = 0;
	uint8_t enabled_channels[16];
	uint8_t x = 0, y = 0;

	switch (action){

	case(NAFE113x8_REQ_INIT):
				/*
				 * Request to initialize MCU SPI and GPIOs to comunicate with NAFE113x8
				 */
				vcom_msg->isize = 0;

				NAFE_Init();

				//Prepare the response to acknowledge the host.
				vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
				vcom_msg->omsg[0]=1;
				vcom_msg->osize = 1;
				vcom_msg->otype = NAFE113x8_REQ;

				buf_read_write[0] = vcom_msg->osize;
				buf_read_write[1] = vcom_msg->otype;
				buf_read_write[2] = vcom_msg->omsg[0];
				buf_read_write[3] = 0;

				free(vcom_msg->omsg);

				break;

	case(NAFE113x8_REQ_READREG):
				/*
				 *  Request of Register Reading
				 */
				Address = ((vcom_msg->imsg[1])<<8) | ((vcom_msg->imsg[2]));
				length = vcom_msg->imsg[3];
	            vcom_msg->isize = 0;

				NAFE13388_Read(&nafeDriver, Address, output, length);

				value = 0;

				if(length == 2)
					value = (output[0] << 8) | (output[1]);
				else
					value = (output[0] << 16) | (output[1] << 8) | (output[2]);

				//Prepare the response to acknowledge the host.
				vcom_msg->omsg = (uint8_t*)malloc(length * sizeof(uint8_t));
				vcom_msg->omsg[0] = (uint8_t)(value & 0xFF);
				vcom_msg->omsg[1] = (uint8_t)((value >> 8) & 0xFF);
				if (length == 3)
					vcom_msg->omsg[2] = (uint8_t)((value >> 16) & 0xFF);
				vcom_msg->osize = length;
				vcom_msg->otype = NAFE113x8_REQ;

				buf_read_write[0] = vcom_msg->osize;
				buf_read_write[1] = vcom_msg->otype;
				memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

				free(vcom_msg->omsg);

				break;

	case(NAFE113x8_REQ_WRITEREG):
				/*
				 * Request of Register Writing
				 */
				Address = ((uint16_t)(vcom_msg->imsg[1])<<8) | ((uint16_t)(vcom_msg->imsg[2]));
				length = vcom_msg->imsg[3];

				if(length == 3){
					buf[0] = vcom_msg->imsg[5];
					buf[1] = vcom_msg->imsg[6];
					buf[2] = vcom_msg->imsg[7];
				}else if (length == 2){
					buf[0] = vcom_msg->imsg[5];
					buf[1] = vcom_msg->imsg[6];
				}

				vcom_msg->isize = 0;

				NAFE13388_Write(&nafeDriver, Address, buf, length);

				//Prepare the response to acknowledge the host.
				vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
				vcom_msg->omsg[0]=1;
				vcom_msg->osize = 1;
				vcom_msg->otype = NAFE113x8_REQ;

				buf_read_write[0] = vcom_msg->osize;
				buf_read_write[1] = vcom_msg->otype;
				buf_read_write[2] = vcom_msg->omsg[0];
				buf_read_write[3] = 0;
				free(vcom_msg->omsg);

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
				vcom_msg->isize = 0;

				NAFE13388_CommandWrite(&nafeDriver, commandID);

				//Prepare the response to acknowledge the host.
				vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
				vcom_msg->omsg[0]=1;
				vcom_msg->osize = 1;
				vcom_msg->otype = NAFE113x8_REQ;

				buf_read_write[0] = vcom_msg->osize;
				buf_read_write[1] = vcom_msg->otype;
				buf_read_write[2] = vcom_msg->omsg[0];
				buf_read_write[3] = 0;
				free(vcom_msg->omsg);

				break;
	case(NAFE113x8_REQ_SCSR):
					/*
					 * Request to start SCSR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					channel = vcom_msg->imsg[2];
					vcom_msg->isize = 0;

					Address = 0x40 + (channel);
					length = 3;

					enableDRDYHasGPIOInterrupt(true);
					clearReceivedFlag();
					NAFE13388_CommandWrite(&nafeDriver ,CMD_SS);
					while(!getReceivedFlag());
					NAFE13388_Read(&nafeDriver, Address, output, length);

					value = (output[0] << 16) | (output[1] << 8) | (output[2]);

					enableDRDYHasGPIOInterrupt(false);

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 3;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= value & 0xFF;
					vcom_msg->omsg[1]= (value>>8) & 0xFF;
					vcom_msg->omsg[2]= (value>>16) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;

					buf_read_write[0] = vcom_msg->osize;
					buf_read_write[1] = vcom_msg->otype;
					memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

					free(vcom_msg->omsg);

					break;

	case(NAFE113x8_REQ_SCCR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					channel = vcom_msg->imsg[2];
					count = (vcom_msg->imsg[4])<<8  | (vcom_msg->imsg[3]);
					//
					vcom_msg->isize = 0;

					Address = 0x40 + (channel);
					length = 3;

					for(int i = 0; i < count ; i++){

						enableDRDYHasGPIOInterrupt(true);
						clearReceivedFlag();
						NAFE13388_CommandWrite(&nafeDriver ,CMD_SC);
						while(!getReceivedFlag());
						NAFE13388_Read(&nafeDriver, Address, output, length);

						samplesBuffer[i] = (output[0] << 16) | (output[1] << 8) | (output[2]);

						enableDRDYHasGPIOInterrupt(false);
					}

					returnedSamples = count;

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;

					buf_read_write[0] = vcom_msg->osize;
					buf_read_write[1] = vcom_msg->otype;
					memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

					free(vcom_msg->omsg);

					break;

	case(NAFE113x8_REQ_MCMR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					//
					vcom_msg->isize = 0;

					NAFE13388_Read(&nafeDriver, NAFE13388_CH_CONFIG4, active, 2);

					activeChannelMask = (active[0] << 8) | (active[1]);

						// Count the current active channels.
					while (activeChannelMask){
						activeChannelCount += activeChannelMask&1;
						if(activeChannelMask&1){
							enabled_channels[y] = x;
							y++;
						}
						activeChannelMask>>=1;
						x++;
						}

					for(int i = 0; i < activeChannelCount; i++)
					{
						enableDRDYHasGPIOInterrupt(true);
						clearReceivedFlag();
						NAFE13388_CommandWrite(&nafeDriver, CMD_CH0+enabled_channels[i]);
						NAFE13388_CommandWrite(&nafeDriver, CMD_MM);
						while(!getReceivedFlag());
						delay(10);

						NAFE13388_Read(&nafeDriver, NAFE13388_CH_DATA0+enabled_channels[i], output, 3);

						samplesBuffer[i] = output[0] << 16 | output[1] << 8 | output[2];

						enableDRDYHasGPIOInterrupt(false);

					}

					returnedSamples = 0;

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;

					buf_read_write[0] = vcom_msg->osize;
					buf_read_write[1] = vcom_msg->otype;
					memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

					free(vcom_msg->omsg);

					break;

	case(NAFE113x8_REQ_MCCR):
					/*
					 * Request to start SCCR and waiting the collect samples.
					 */

					devID = vcom_msg->imsg[1];
					count = (vcom_msg->imsg[3])<<8  | (vcom_msg->imsg[2]);
					vcom_msg->isize = 0;

					NAFE13388_Read(&nafeDriver, NAFE13388_CH_CONFIG4, active, 2);

					activeChannelMask = (active[0] << 8) | (active[1]);

					// Count the current active channels.
					while (activeChannelMask){
						activeChannelCount += activeChannelMask&1;
						if(activeChannelMask&1){
							enabled_channels[y] = x;
							y++;
						}
						activeChannelMask>>=1;
						x++;
					}

					for(int i = 0; i < count; i++)
					{
						for(int j = 0; j < activeChannelCount; j++)
							{
							    enableDRDYHasGPIOInterrupt(true);
								clearReceivedFlag();
								NAFE13388_CommandWrite(&nafeDriver, CMD_CH0+enabled_channels[j]);
								NAFE13388_CommandWrite(&nafeDriver, CMD_MC);
								while(!getReceivedFlag());
								delay(10);

								NAFE13388_Read(&nafeDriver, NAFE13388_CH_DATA0+enabled_channels[j], output, 3);

								samplesBuffer[(activeChannelCount*i)+j] = (output[0] << 16) | (output[1] << 8) | (output[2]);

								enableDRDYHasGPIOInterrupt(false);

							}
					}

					returnedSamples = count;

					//Prepare the response to acknowledge the host.
					vcom_msg->osize = 2;
					vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
					vcom_msg->omsg[0]= returnedSamples & 0xFF;
					vcom_msg->omsg[1]= (returnedSamples>>8) & 0xFF;
					vcom_msg->otype = NAFE113x8_REQ;

					buf_read_write[0] = vcom_msg->osize;
					buf_read_write[1] = vcom_msg->otype;
					memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

					free(vcom_msg->omsg);

					break;

	case(NAFE113x8_REQ_SAMPLES):

			bit16 = vcom_msg->imsg[1];
			offset = (vcom_msg->imsg[3])<<8  | (vcom_msg->imsg[2]);
			count = (vcom_msg->imsg[5])<<8  | (vcom_msg->imsg[4]);
			vcom_msg->isize = 0;

			if (bit16 == 0)
				vcom_msg->osize = count * 3;
			else
				vcom_msg->osize = count * 2;

			vcom_msg->omsg = (uint8_t*)malloc(vcom_msg->osize * sizeof(uint8_t));
			for (int i=0;i<count; i++ ){
				if (bit16 == 0){
					vcom_msg->omsg[i*3] = (uint8_t)((NAFE13388_getSample(i + offset)>>16) & 0xFF);
					vcom_msg->omsg[(i*3) + 1] = (uint8_t)((NAFE13388_getSample(i + offset)>>8) & 0xFF);
					vcom_msg->omsg[(i*3) + 2] = (uint8_t)(NAFE13388_getSample(i + offset) & 0xFF);
				}
				else{
					vcom_msg->omsg[i*2] = (uint8_t)((NAFE13388_getSample(i + offset)>>8) & 0xFF);
					vcom_msg->omsg[(i*2)] = (uint8_t)(NAFE13388_getSample(i + offset) & 0xFF);
				}
			}
			vcom_msg->otype = NAFE113x8_REQ;

			buf_read_write[0] = vcom_msg->osize;
			buf_read_write[1] = vcom_msg->otype;
			memcpy(buf_read_write + 2, vcom_msg->omsg, vcom_msg->osize);

			free(vcom_msg->omsg);

			break;
}
}

/*
 * Initialize all the MCU peripherals in order to enable the comunication with NAFE
 */
static void NAFE_Init(){
	NAFE13388_Initialize(&nafeDriver, &SPI_S_DRIVER, 1, &NAFE13388_CS);
}

uint32_t NAFE13388_getSample(uint16_t index){
	return samplesBuffer[index];
}

