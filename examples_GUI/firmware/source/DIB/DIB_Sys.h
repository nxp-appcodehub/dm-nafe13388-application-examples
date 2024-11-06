/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DIB_SYS_H_
#define DIB_SYS_H_

#include "virtual_com.h"

void SYS_Handling(parse_vcom_t *vcom_msg);

#define SYS_REQ_RESET 0

#endif /* DIB_SYS_H_ */
