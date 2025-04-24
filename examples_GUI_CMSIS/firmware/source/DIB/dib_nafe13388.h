/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DIB_NAFE13388_H_
#define DIB_NAFE13388_H_

#include "nafe13388_sdk.h"
#include "nafe13388.h"

void NAFE113x8_Handling(struct parse_vcom_t *vcom_msg);

#define NAFE113x8_REQ_INIT 0
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


#endif /* DIB_NAFE13388_H_ */
