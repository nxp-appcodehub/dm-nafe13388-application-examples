/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DIB_Sys.h"

void SYS_Handling(parse_vcom_t *vcom_msg){
	uint8_t action = vcom_msg->imsg[0];

	switch (action){
		case(SYS_REQ_RESET):
				NVIC_SystemReset();
				break;
	}

}
