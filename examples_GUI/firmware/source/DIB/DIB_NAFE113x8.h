/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DIB_DIB_NAFE113X8_H_
#define DIB_DIB_NAFE113X8_H_

#include "virtual_com.h"
#include "NAFE113x8_SDK.h"
#include "NAFE113x8_Register.h"

void NAFE113x8_Handling(parse_vcom_t *vcom_msg);

#define NAFE113x8_REQ_INIT 0
//#define NAFE113x8_REQ_RESET 6
#define NAFE113x8_REQ_READREG 1
#define NAFE113x8_REQ_WRITEREG 2
#define NAFE113x8_REQ_READFIELD 3
#define NAFE113x8_REQ_WRITEFIELD 4
#define NAFE113x8_REQ_SENDCMD 5
#define NAFE113x8_REQ_SCSR 10
#define NAFE113x8_REQ_SCCR 6
#define NAFE113x8_REQ_SAMPLES 7
#define NAFE113x8_REQ_MCMR 8
#define NAFE113x8_REQ_MCCR 9

typedef struct NAFE113x8ReadWriteParse
{
	const Register_t Register;
	bool devID;
	uint32_t value;
} NAFE113x8ReadWriteParse_t;


#endif /* DIB_DIB_NAFE113X8_H_ */
