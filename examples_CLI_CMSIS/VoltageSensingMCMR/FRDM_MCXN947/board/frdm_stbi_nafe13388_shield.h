/*
 * Copyright 2022, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! File: frdm_stbi_nafe13388_shield.h
* @brief The frdm_stbi_nafe13388_shield.h file declares arduino pin mapping for frdm_stbi_pcf2131_shield expansion board.
*/

#ifndef _FRDM_NAFE13388_SHIELD_H_
#define _FRDM_NAFE13388_SHIELD_H_

/* The shield name */
#define SHIELD_NAME "FRDM-NAFE13388"

// PCF2131 RTC Information

#define NAFE13388_CS       D10
#define NAFE13388_MOSI     D11
#define NAFE13388_MISO     D12
#define NAFE13388_SCLK     D13

#define NAFE13388_UIM 1

#endif /* _FRDM_NAFE13388_SHIELD_H_ */
