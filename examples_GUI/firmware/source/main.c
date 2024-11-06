/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "virtual_com.h"
#include "main.h"
#include "DIB_Gpio.h"
#include "DIB_Spi.h"
#include "DIB_Sys.h"
#include "DIB_NAFE113x8.h"
 
int main(void)

{
	uint8_t *imsg;
	uint8_t *omsg;
	parse_vcom_t vcom_msg;
	vcom_msg.isize = 0;
	vcom_msg.osize=0;
	vcom_msg.imsg=imsg;
	vcom_msg.omsg=omsg;

    BOARD_InitBootPins();
    BOARD_PowerMode_OD();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    CLOCK_SetupExtClocking(BOARD_XTAL0_CLK_HZ);

    USBInit();

    while (1)
    {
        if (CMD_RECEIVED == USBTask(&vcom_msg)){

        	switch(vcom_msg.itype){
        	case(GPIO_REQ):
				GPIO_Handling(&vcom_msg);
        			break;
        	case(SPI_REQ):
				SPI_Handling(&vcom_msg);
        			break;
        	case(SYS_REQ):
				SYS_Handling(&vcom_msg);
        			break;
        	case(NAFE113x8_REQ):
				NAFE113x8_Handling(&vcom_msg);
        			break;
        	}
        }
    }
}
