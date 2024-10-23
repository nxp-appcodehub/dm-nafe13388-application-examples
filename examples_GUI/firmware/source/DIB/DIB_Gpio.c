/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DIB_Gpio.h"
#include "main.h"

static GPIO_Type* uintToGpioType (uint8_t port);
static void gpioConfig(gpioConfigParse_t *config);
static void gpioSet(gpioSetGetParse_t *set);
static uint8_t gpioGet(gpioSetGetParse_t *get);
static PORT_Type* uintToPortType (uint8_t port);

/*
 * This Function Handle the GPIO protocol for configuration/get/set.
 * The protocol message format is:
 * Byte 0: Action (Config, Set or Get).
 * Byte 1: Port Number.
 * Byte 2: Pin Number.
 * if (Action = Config):
 * 	Byte 3: Pin Direction.
 * 	Byte 4: Start Value.
 * else if (Action = Set)
 * 	Byte 3: Value to set 0 or 1.
 */
void GPIO_Handling(parse_vcom_t *vcom_msg){
	uint8_t action = vcom_msg->imsg[0];
	GPIO_Type *gpioPort;
	PORT_Type *port;
	gpioPort=uintToGpioType(vcom_msg->imsg[1]);
	port=uintToPortType(vcom_msg->imsg[1]);
	uint32_t pin=vcom_msg->imsg[2];


	switch (action){

	case(GPIO_REQ_CONFIG):
		gpioConfigParse_t ConfigParse={0};
		ConfigParse.GPIO_Port = gpioPort;
		ConfigParse.Port = port;
		ConfigParse.GPIO_Pin = pin;
		ConfigParse.direction = vcom_msg->imsg[3];
		ConfigParse.startValue = vcom_msg->imsg[4];
		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		gpioConfig(&ConfigParse);

		//Prepare the response to acknowledge the host.
		vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
		vcom_msg->omsg[0]=1;
		vcom_msg->osize = 1;
		vcom_msg->otype = GPIO_REQ;
	break;

	case(GPIO_REQ_SET):
	case(GPIO_REQ_GET):
		gpioSetGetParse_t getSetParse = {0};
		getSetParse.GPIO_Port = gpioPort;
		getSetParse.GPIO_Pin = pin;

		if (action == GPIO_REQ_SET){
			// A set pin is requested.
			getSetParse.Value = vcom_msg->imsg[3];
			free(vcom_msg->imsg);
			vcom_msg->isize = 0;

			gpioSet(&getSetParse);

			//Prepare the response to acknowledge the host.
			vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
			vcom_msg->omsg[0]=1;
			vcom_msg->osize =1;
			vcom_msg->otype = GPIO_REQ;
			break;
		}

		// A read pin is requested.
		free(vcom_msg->imsg);
		vcom_msg->isize = 0;

		uint8_t readValue;
		readValue = gpioGet(&getSetParse);

		//Prepare the value response for the host.
		vcom_msg->omsg = (uint8_t*)malloc(sizeof(uint8_t));
		vcom_msg->omsg[0]=readValue;
		vcom_msg->osize =1;
		vcom_msg->otype = GPIO_REQ;
		break;
	}
}

/*
 * This Function convert a uint8_t port number to the related GPIO_Type Port.
 */
static PORT_Type* uintToPortType (uint8_t port){

	PORT_Type *portPort;
	switch (port){
	case (0):
		portPort=PORT0;
	break;
	case (1):
		portPort=PORT1;
	break;
	case (2):
		portPort=PORT2;
	break;
	case (3):
		portPort=PORT3;
	break;
	case (4):
		portPort=PORT4;
	break;
	case (5):
		portPort=PORT5;
	break;
	}
	return portPort;
}

/*
 * This Function convert a uint8_t port number to the related GPIO_Type Port.
 */
static GPIO_Type* uintToGpioType (uint8_t port){

	GPIO_Type *gpioPort;
	switch (port){
	case (0):
		gpioPort=GPIO0;
	break;
	case (1):
		gpioPort=GPIO1;
	break;
	case (2):
		gpioPort=GPIO2;
	break;
	case (3):
		gpioPort=GPIO3;
	break;
	case (4):
		gpioPort=GPIO4;
	break;
	case (5):
		gpioPort=GPIO5;
	break;
	}
	return gpioPort;
}

static void gpioConfig(gpioConfigParse_t *config){
    gpio_pin_config_t pinConfig = {
        .pinDirection = config->direction,
        .outputLogic = config->startValue
    };

    PORT_Type *port = config->Port;

	GPIO_PinInit(config->GPIO_Port, config->GPIO_Pin, &pinConfig);
	PORT_SetPinMux(port, config->GPIO_Pin, kPORT_MuxAlt0);
	port->PCR[config->GPIO_Pin] =((port->PCR[config->GPIO_Pin] &(~(PORT_PCR_IBE_MASK)))
								   /* Input Buffer Enable: Enables. */
								   | PORT_PCR_IBE(PCR_IBE_ibe1));
}

static void gpioSet(gpioSetGetParse_t *set){
	GPIO_PinWrite(set->GPIO_Port, set->GPIO_Pin,set->Value);
}

static uint8_t gpioGet(gpioSetGetParse_t *get){
	return (uint8_t)GPIO_PinRead(get->GPIO_Port, get->GPIO_Pin);
}
