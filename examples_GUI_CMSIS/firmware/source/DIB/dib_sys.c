/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "frdmmcxn947.h"
#include "dib_sys.h"

void SYS_Handling(parse_vcom_t *vcom_msg){
	uint8_t action = vcom_msg->imsg[0];

	switch (action){
		case(SYS_REQ_RESET):
				NVIC_SystemReset();
				break;
	}

}
