/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#ifndef DIB_GPIO_H_
#define DIB_GPIO_H_

#include "fsl_gpio.h"
#include "virtual_com.h"
#include "fsl_port.h"
#include "pin_mux.h"

void GPIO_Handling(parse_vcom_t *vcom_msg);

typedef struct gpioConfigParse
{
	GPIO_Type *GPIO_Port;
	PORT_Type *Port;
	uint32_t GPIO_Pin;
	gpio_pin_direction_t direction;
	uint8_t startValue;
} gpioConfigParse_t;

typedef struct gpioSetGetParse
{
	GPIO_Type *GPIO_Port;
	uint32_t GPIO_Pin;
	uint8_t Value;
} gpioSetGetParse_t;

#define GPIO_REQ_CONFIG 0
#define GPIO_REQ_SET 1
#define GPIO_REQ_GET 2





#endif /* DIB_GPIO_H_ */
