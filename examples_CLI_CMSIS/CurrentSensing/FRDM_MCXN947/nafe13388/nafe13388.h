/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  nafe13388.h
 *  @brief The nafe13388.h file contains the register definitions, access macros, and
 * 		   device access functions.
 */

#ifndef NAFE13388_H_
#define NAFE13388_H_

#define DRDYPINEDGE_EVERYCH 0
#define DRDYPINEDGE_LASTCH 1

#define HVAIP_GND 0
#define HVAIP_AI1P 1
#define HVAIP_AI2P 2
#define HVAIP_AI3P 3
#define HVAIP_AI4P 4
#define HVAIP_REFH 5
#define HVAIP_REFL 6
#define HVAIP_AICOM 7

#define HVAIN_GND 0
#define HVAIN_AI1N 1
#define HVAIN_AI2N 2
#define HVAIN_AI3N 3
#define HVAIN_AI4N 4
#define HVAIN_REFH 5
#define HVAIN_REFL 6
#define HVAIN_AICOM 7

#define LVSINGIN_REF2 0
#define LVSINGIN_GPIO01 1
#define LVSINGIN_VADD 3
#define LVSINGIN_VHDD 4
#define LVSINGIN_VHSS 5

#define NAFE13388_SPI_S_BAUDRATE     6000000U

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum CommonCommandID{
	CMD_CLEAR_ALARM = (uint16_t)0x0012,
	CMD_RESET       = (uint16_t)0x0014,
	CMD_CLEAR_REG   = (uint16_t)0x0015,
	CMD_RELOAD      = (uint16_t)0x0016
};

enum ADCCommandID{
	CMD_CH0        = (uint16_t)0x0000,
	CMD_CH1        = (uint16_t)0x0001,
	CMD_CH2		   = (uint16_t)0x0002,
	CMD_CH3        = (uint16_t)0x0003,
	CMD_CH4        = (uint16_t)0x0004,
	CMD_CH5        = (uint16_t)0x0005,
	CMD_CH6        = (uint16_t)0x0006,
	CMD_CH7        = (uint16_t)0x0007,
	CMD_CH8        = (uint16_t)0x0008,
	CMD_CH9        = (uint16_t)0x0009,
	CMD_CH10	   = (uint16_t)0x000A,
	CMD_CH11       = (uint16_t)0x000B,
	CMD_CH12       = (uint16_t)0x000C,
	CMD_CH13       = (uint16_t)0x000D,
	CMD_CH14       = (uint16_t)0x000E,
	CMD_CH15       = (uint16_t)0x000F,
	CMD_ADC_ABORT  = (uint16_t)0x0010,
	CMD_ADC_END    = (uint16_t)0x0011,
	CMD_CLEAR_DATA = (uint16_t)0x0013,
	CMD_SS         = (uint16_t)0x2000,
	CMD_SC         = (uint16_t)0x2001,
	CMD_MM         = (uint16_t)0x2002,
	CMD_MC         = (uint16_t)0x2003,
	CMD_MS         = (uint16_t)0x2004
};

enum
{
	// 2 Bytes
	NAFE13388_CH_CONFIG0 = 0x20,
	NAFE13388_CH_CONFIG1 = 0x21,
	NAFE13388_CH_CONFIG2 = 0x22,
	NAFE13388_CH_CONFIG3 = 0x23,
	NAFE13388_CH_CONFIG4 = 0x24,

	//3 Bytes
	NAFE13388_CH_CONFIG5_0 = 0x50,
	NAFE13388_CH_CONFIG5_1 = 0x51,
	NAFE13388_CH_CONFIG5_2 = 0x52,
	NAFE13388_CH_CONFIG5_3 = 0x53,
	NAFE13388_CH_CONFIG5_4 = 0x54,
	NAFE13388_CH_CONFIG5_5 = 0x55,
	NAFE13388_CH_CONFIG5_6 = 0x56,
	NAFE13388_CH_CONFIG5_7 = 0x57,
	NAFE13388_CH_CONFIG5_8 = 0x58,
	NAFE13388_CH_CONFIG5_9 = 0x59,
	NAFE13388_CH_CONFIG5_10 = 0x5A,
	NAFE13388_CH_CONFIG5_11 = 0x5B,
	NAFE13388_CH_CONFIG5_12 = 0x5C,
	NAFE13388_CH_CONFIG5_13 = 0x5D,
	NAFE13388_CH_CONFIG5_14 = 0x5E,
	NAFE13388_CH_CONFIG5_15 = 0x5F,

	//3 Bytes
	NAFE13388_CH_CONFIG6_0 = 0x60,
	NAFE13388_CH_CONFIG6_1 = 0x61,
	NAFE13388_CH_CONFIG6_2 = 0x62,
	NAFE13388_CH_CONFIG6_3 = 0x63,
	NAFE13388_CH_CONFIG6_4 = 0x64,
	NAFE13388_CH_CONFIG6_5 = 0x65,
	NAFE13388_CH_CONFIG6_6 = 0x66,
	NAFE13388_CH_CONFIG6_7 = 0x67,
	NAFE13388_CH_CONFIG6_8 = 0x68,
	NAFE13388_CH_CONFIG6_9 = 0x69,
	NAFE13388_CH_CONFIG6_10 = 0x6A,
	NAFE13388_CH_CONFIG6_11 = 0x6B,
	NAFE13388_CH_CONFIG6_12 = 0x6C,
	NAFE13388_CH_CONFIG6_13 = 0x6D,
	NAFE13388_CH_CONFIG6_14 = 0x6E,
	NAFE13388_CH_CONFIG6_15 = 0x6F,

	//3 Bytes
	NAFE13388_CH_DATA0 = 0x40,
	NAFE13388_CH_DATA1 = 0x41,
	NAFE13388_CH_DATA2 = 0x42,
	NAFE13388_CH_DATA3 = 0x43,
	NAFE13388_CH_DATA4 = 0x44,
	NAFE13388_CH_DATA5 = 0x45,
	NAFE13388_CH_DATA6 = 0x46,
	NAFE13388_CH_DATA7 = 0x47,
	NAFE13388_CH_DATA8 = 0x48,
	NAFE13388_CH_DATA9 = 0x49,
	NAFE13388_CH_DATA10 = 0x4A,
	NAFE13388_CH_DATA11 = 0x4B,
	NAFE13388_CH_DATA12 = 0x4C,
	NAFE13388_CH_DATA13 = 0x4D,
	NAFE13388_CH_DATA14 = 0x4E,
	NAFE13388_CH_DATA15 = 0x4F,

	//3 Bytes
	NAFE13388_GAIN_COEFF0 = 0x80,
	NAFE13388_GAIN_COEFF1= 0x81,
	NAFE13388_GAIN_COEFF2 = 0x82,
	NAFE13388_GAIN_COEFF3 = 0x83,
	NAFE13388_GAIN_COEFF4 = 0x84,
	NAFE13388_GAIN_COEFF5 = 0x85,
	NAFE13388_GAIN_COEFF6 = 0x86,
	NAFE13388_GAIN_COEFF7 = 0x87,
	NAFE13388_GAIN_COEFF8 = 0x88,
	NAFE13388_GAIN_COEFF9 = 0x89,
	NAFE13388_GAIN_COEFF10 = 0x8A,
	NAFE13388_GAIN_COEFF11 = 0x8B,
	NAFE13388_GAIN_COEFF12 = 0x8C,
	NAFE13388_GAIN_COEFF13 = 0x8D,
	NAFE13388_GAIN_COEFF14 = 0x8E,
	NAFE13388_GAIN_COEFF15 = 0x8F,

	//3 Bytes
	NAFE13388_OFF_COEFF0 = 0x90,
	NAFE13388_OFF_COEFF1= 0x91,
	NAFE13388_OFF_COEFF2 = 0x92,
	NAFE13388_OFF_COEFF3 = 0x93,
	NAFE13388_OFF_COEFF4 = 0x94,
	NAFE13388_OFF_COEFF5 = 0x95,
	NAFE13388_OFF_COEFF6 = 0x96,
	NAFE13388_OFF_COEFF7 = 0x97,
	NAFE13388_OFF_COEFF8 = 0x98,
	NAFE13388_OFF_COEFF9 = 0x99,
	NAFE13388_OFF_COEFF10 = 0x9A,
	NAFE13388_OFF_COEFF11 = 0x9B,
	NAFE13388_OFF_COEFF12 = 0x9C,
	NAFE13388_OFF_COEFF13 = 0x9D,
	NAFE13388_OFF_COEFF14 = 0x9E,
	NAFE13388_OFF_COEFF15 = 0x9F,

	//3 Bytes
	NAFE13388_OPT_COEFF0 = 0xA0,
	NAFE13388_OPT_COEFF1= 0xA1,
	NAFE13388_OPT_COEFF2 = 0xA2,
	NAFE13388_OPT_COEFF3 = 0xA3,
	NAFE13388_OPT_COEFF4 = 0xA4,
	NAFE13388_OPT_COEFF5 = 0xA5,
	NAFE13388_OPT_COEFF6 = 0xA6,
	NAFE13388_OPT_COEFF7 = 0xA7,
	NAFE13388_OPT_COEFF8 = 0xA8,
	NAFE13388_OPT_COEFF9 = 0xA9,
	NAFE13388_OPT_COEFF10 = 0xAA,
	NAFE13388_OPT_COEFF11 = 0xAB,
	NAFE13388_OPT_COEFF12 = 0xAC,
	NAFE13388_OPT_COEFF13 = 0xAD,

	//2 Bytes
	NAFE13388_SYS_CONFIG0 = 0x30,
	NAFE13388_SYS_STATUS0 = 0x31,
	NAFE13388_GLOBAL_ALARM_E = 0x32,
	NAFE13388_GLOBAL_ALARM_I = 0x33,

	//2 Bytes
	NAFE13388_GPI_DATA = 0x29,
	NAFE13388_GPO_DATA = 0x2F,
	NAFE13388_GPIO_CONFIG0 = 0x2A,
	NAFE13388_GPIO_CONFIG1 = 0x2B,
	NAFE13388_GPIO_CONFIG2 = 0x2C,
	NAFE13388_GPI_EDGE_POS = 0x2D,
	NAFE13388_GPI_EDGE_NEG = 0x2E,

	//2 Bytes
	NAFE13388_PN0 = 0x7E,
	NAFE13388_PN1 = 0x7D,
	NAFE13388_PN2 = 0x7C,

};

/*--------------------------------
 ** Register: CH_CONFIG0
 ** Enum: NAFE13388_CHCONFIG0
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t tcc_off : 1;

		uint16_t lvsig_in : 3;

		uint16_t hv_sel : 1;

		uint16_t ch_gain : 3;

		uint16_t hv_aip : 4;

		uint16_t hv_ain : 4;
	} b;
	uint16_t w;
} NAFE13388_CHCONFIG0;

/*
 * CH_CONFIG0 - Bit field mask definitions
 */
#define NAFE13388_CHCONFIG0_TCC_OFF_MASK      ((uint16_t)0x0001)
#define NAFE13388_CHCONFIG0_TCC_OFF_SHIFT     ((uint16_t)0)

#define NAFE13388_CHCONFIG0_LVSIG_IN_MASK     ((uint16_t)0x000E)
#define NAFE13388_CHCONFIG0_LVSIG_IN_SHIFT    ((uint16_t)1)

#define NAFE13388_CHCONFIG0_HV_SEL_MASK       ((uint16_t)0x0010)
#define NAFE13388_CHCONFIG0_HV_SEL_SHIFT      ((uint16_t)4)

#define NAFE13388_CHCONFIG0_CH_GAIN_MASK      ((uint16_t)0x00E0)
#define NAFE13388_CHCONFIG0_CH_GAIN_SHIFT     ((uint16_t)5)

#define NAFE13388_CHCONFIG0_HV_AIP_MASK       ((uint16_t)0x0F00)
#define NAFE13388_CHCONFIG0_HV_AIP_SHIFT      ((uint16_t)8)

#define NAFE13388_CHCONFIG0_HV_AIN_MASK       ((uint16_t)0xF000)
#define NAFE13388_CHCONFIG0_HV_AIN_SHIFT      ((uint16_t)12)



/*--------------------------------
 ** Register: CH_CONFIG1
 ** Enum: NAFE13388_CHCONFIG1
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t adc_sinc : 3;

		uint16_t adc_data_rate : 5;

		uint16_t ch_thrs : 4;

		uint16_t gain_off_ptr : 4;

	} b;
	uint16_t w;
} NAFE13388_CHCONFIG1;

/*
 * CH_CONFIG1 - Bit field mask definitions
 */
#define NAFE13388_CHCONFIG1_ADC_SINC_MASK      		((uint16_t)0x0001)
#define NAFE13388_CHCONFIG1_ADC_SINC_SHIFT     		((uint16_t)0)

#define NAFE13388_CHCONFIG1_ADC_DATA_RATE_MASK     	((uint16_t)0x00F8)
#define NAFE13388_CHCONFIG1_ADC_DATA_RATE_SHIFT    	((uint16_t)3)

#define NAFE13388_CHCONFIG1_CH_THRS_MASK       		((uint16_t)0x00F0)
#define NAFE13388_CHCONFIG1_CH_THRS_SHIFT      		((uint16_t)8)

#define NAFE13388_CHCONFIG1_GAIN_OFF_PTR_MASK      	((uint16_t)0xF000)
#define NAFE13388_CHCONFIG1_GAIN_OFF_PTR_SHIFT     	((uint16_t)12)


/*--------------------------------
 ** Register: CH_CONFIG2
 ** Enum: NAFE13388_CHCONFIG2
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 7;

		uint16_t ch_chop : 1;

		uint16_t adc_filter_reset : 1;

		uint16_t adc_normal_settling : 1;

		uint16_t ch_delay : 6;

	} b;
	uint16_t w;
} NAFE13388_CHCONFIG2;

/*
 * CH_CONFIG2 - Bit field mask definitions
 */
#define NAFE13388_CHCONFIG2_CH_CHOP_MASK      			((uint16_t)0x0080)
#define NAFE13388_CHCONFIG2_CH_CHOP_SHIFT     			((uint16_t)7)

#define NAFE13388_CHCONFIG2_ADC_FILTER_RESET_MASK     	((uint16_t)0x0100)
#define NAFE13388_CHCONFIG2_ADC_FILTER_RESET_SHIFT    	((uint16_t)8)

#define NAFE13388_CHCONFIG2_ADC_NORMAL_SETTLING_MASK    ((uint16_t)0x0200)
#define NAFE13388_CHCONFIG2_ADC_NORMAL_SETTLING_SHIFT   ((uint16_t)9)

#define NAFE13388_CHCONFIG2_CH_DELAY_MASK      			((uint16_t)0xFC00)
#define NAFE13388_CHCONFIG2_CH_DELAY_SHIFT     			((uint16_t)10)


/*--------------------------------
 ** Register: CH_CONFIG3
 ** Enum: NAFE13388_CHCONFIG3
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t viex_ain_en : 3;

		uint16_t viex_aip_en : 3;

		uint16_t viex_chop : 1;

		uint16_t _reserved_ : 1;

		uint16_t odc : 1;

		uint16_t vexc_en : 1;

		uint16_t viex_mag : 4;

		uint16_t viex_pol : 1;

		uint16_t viex_vi : 1;
	} b;
	uint16_t w;
} NAFE13388_CHCONFIG3;

/*
 * CH_CONFIG3 - Bit field mask definitions
 */
#define NAFE13388_CHCONFIG3_VIEX_AIN_EN_MASK      ((uint16_t)0x0007)
#define NAFE13388_CHCONFIG3_VIEX_AIN_EN_SHIFT     ((uint16_t)0)

#define NAFE13388_CHCONFIG3_VIEX_AIP_EN_MASK     ((uint16_t)0x0038)
#define NAFE13388_CHCONFIG3_VIEX_AIP_EN_SHIFT    ((uint16_t)3)

#define NAFE13388_CHCONFIG3_VIEX_CHOP_MASK       ((uint16_t)0x0040)
#define NAFE13388_CHCONFIG3_VIEX_CHOP_SHIFT      ((uint16_t)6)

#define NAFE13388_CHCONFIG3_ODC_MASK      		((uint16_t)0x0100)
#define NAFE13388_CHCONFIG3_ODC_SHIFT     		((uint16_t)8)

#define NAFE13388_CHCONFIG3_VEXC_EN_MASK       	((uint16_t)0x0200)
#define NAFE13388_CHCONFIG3_VEXC_EN_SHIFT      	((uint16_t)9)

#define NAFE13388_CHCONFIG3_VIEX_MAG_MASK       ((uint16_t)0x3C00)
#define NAFE13388_CHCONFIG3_VIEX_MAG_SHIFT      ((uint16_t)10)

#define NAFE13388_CHCONFIG3_VIEX_POL_MASK       ((uint16_t)0x4000)
#define NAFE13388_CHCONFIG3_VIEX_POL_SHIFT      ((uint16_t)14)

#define NAFE13388_CHCONFIG3_VIEX_VI_MASK       ((uint16_t)0x8000)
#define NAFE13388_CHCONFIG3_VIEX_VI_SHIFT      ((uint16_t)15)


/*
 * CH_CONFIG4 - Bit field mask definitions
 */

#define NAFE13388_CHCONFIG4_MASK       ((uint16_t)0xFFFF)
#define NAFE13388_CHCONFIG4_SHIFT      ((uint16_t)0)


/*--------------------------------
 ** Register: SYS_CONFIG0
 ** Enum: NAFE13388_SYSCONFIG0
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t crc_error_on_gpio2 : 1;

		uint16_t intb_driver_type : 1;

		uint16_t spi_dout_drive : 1;

		uint16_t global_alrm_sticky : 1;

		uint16_t drdy_pin_edge : 1;

		uint16_t adc_sync : 1;

		uint16_t status_en : 1;

		uint16_t crc_en : 1;

		uint16_t ck_src_sel : 2;

		uint16_t ref_sel : 2;

		uint16_t mclk_out_enable : 1;

		uint16_t status_sticky : 1;

		uint16_t adc_data_out_16bit : 1;

		uint16_t drdy_pwdt : 1;

	} b;
	uint16_t w;
} NAFE13388_SYSCONFIG0;

/*
 * SYS_CONFIG0 - Bit field mask definitions
 */
#define NAFE13388_SYS_CONFIG0_CRC_ERR_ON_GPIO2_MASK      			((uint16_t)0x0001)
#define NAFE13388_SYS_CONFIG0_CRC_ERR_ON_GPIO2_SHIFT     			((uint16_t)0)

#define NAFE13388_SYS_CONFIG0_INTB_DRV_TYPE_MASK     				((uint16_t)0x0002)
#define NAFE13388_SYS_CONFIG0_INTB_DRV_TYPE_SHIFT    				((uint16_t)1)

#define NAFE13388_SYS_CONFIG0_SPI_DOUT_DRIVE_MASK       			((uint16_t)0x0004)
#define NAFE13388_SYS_CONFIG0_SPI_DOUT_DRIVE_SHIFT      			((uint16_t)2)

#define NAFE13388_SYS_CONFIG0_GLOBAL_ALRM_STICKY_MASK      		((uint16_t)0x0008)
#define NAFE13388_SYS_CONFIG0_GLOBAL_ALRM_STICKY_SHIFT     		((uint16_t)3)

#define NAFE13388_SYS_CONFIG0_DRDY_PIN_EDGE_MASK      			((uint16_t)0x0010)
#define NAFE13388SYS_CONFIG0_DRDY_PIN_EDGE_SHIFT     			((uint16_t)4)

#define NAFE13388_SYS_CONFIG0_ADC_SYNC_MASK       				((uint16_t)0x0020)
#define NAFE13388_SYS_CONFIG0_ADC_SYNC_SHIFT      				((uint16_t)5)

#define NAFE13388_SYS_CONFIG0_STATUS_EN_MASK       				((uint16_t)0x0040)
#define NAFE13388_SYS_CONFIG0_STATUS_EN_SHIFT      				((uint16_t)6)

#define NAFE13388_SYS_CONFIG0_CRC_EN_MASK       					((uint16_t)0x0080)
#define NAFE13388_SYS_CONFIG0_CRC_EN_SHIFT      					((uint16_t)7)

#define NAFE13388_SYS_CONFIG0_CK_SRC_SEL_MASK       				((uint16_t)0x0300)
#define NAFE13388_SYS_CONFIG0_CK_SRC_SEL_SHIFT      				((uint16_t)8)

#define NAFE13388_SYS_CONFIG0_REF_SEL_MASK       					((uint16_t)0x0C00)
#define NAFE13388_SYS_CONFIG0_REF_SEL_SHIFT      					((uint16_t)10)

#define NAFE13388_SYS_CONFIG0_MCLK_OUT_EN_MASK       				((uint16_t)0x1000)
#define NAFE13388_SYS_CONFIG0_MCLK_OUT_EN_SHIFT      				((uint16_t)12)

#define NAFE13388_SYS_CONFIG0_STATUS_STICKY_MASK       			((uint16_t)0x2000)
#define NAFE13388_SYS_CONFIG0_STATUS_STICKY_SHIFT      			((uint16_t)13)

#define NAFE13388_SYS_CONFIG0_ADC_DOUT_16BIT_MASK       			((uint16_t)0x4000)
#define NAFE13388_SYS_CONFIG0_ADC_DOUT_16BIT_SHIFT      			((uint16_t)14)

#define NAFE13388_SYS_CONFIG0_DRDY_PWDT_MASK       				((uint16_t)0x8000)
#define NAFE13388_SYS_CONFIG0_DRDY_PWDT_SHIFT      				((uint16_t)15)


/*--------------------------------
 ** Register: SYS_STATUS0
 ** Enum: NAFE13388_SYSSTATUS0
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 4;

		uint16_t adc_conv_ch : 4;

		uint16_t config_ch_ptr : 4;

		uint16_t crc_error : 1;

		uint16_t chip_ready : 1;

		uint16_t multi_ch_active : 1;

		uint16_t single_ch_active : 1;
	} b;
	uint16_t w;
} NAFE13388_SYSSTATUS0;

/*
 * SYS_CONFIG0 - Bit field mask definitions
 */
#define NAFE13388_SYS_STATUS0_ADC_CONV_CH_MASK      	((uint16_t)0x00F0)
#define NAFE13388_SYS_STATUS0_ADC_CONV_CH_SHIFT    		((uint16_t)4)

#define NAFE13388_SYS_STATUS0_CONFIG_CH_PTR_MASK     	((uint16_t)0x0F00)
#define NAFE13388_SYS_STATUS0_CONFIG_CH_PTR_SHIFT    	((uint16_t)8)

#define NAFE13388_SYS_STATUS0_CRC_ERROR_MASK       		((uint16_t)0x1000)
#define NAFE13388_SYS_STATUS0_CRC_ERROR_SHIFT      		((uint16_t)12)

#define NAFE13388_SYS_STATUS0_ODC_MASK      			((uint16_t)0x2000)
#define NAFE13388_SYS_STATUS0_ODC_SHIFT     			((uint16_t)13)

#define NAFE13388_SYS_STATUS0_VEXC_EN_MASK       		((uint16_t)0x4000)
#define NAFE13388_SYS_STATUS0_VEXC_EN_SHIFT      		((uint16_t)14)

#define NAFE13388_SYS_STATUS0_VIEX_MAG_MASK       		((uint16_t)0x8000)
#define NAFE13388_SYS_STATUS0_VIEX_MAG_SHIFT      		((uint16_t)15)


#define NAFE13388_PN0_MASK       						((uint16_t)0xFFFF)
#define NAFE13388_PN0_SHIFT      						((uint16_t)0)

#define NAFE13388_PN1_MASK       						((uint16_t)0xFFFF)
#define NAFE13388_PN1_SHIFT      						((uint16_t)0)

#define NAFE13388_PN2_MASK       						((uint16_t)0xFFFF)
#define NAFE13388_PN2_SHIFT      						((uint16_t)0)


/*--------------------------------
 ** Register: GPI_DATA
 ** Enum: NAFE13388_GPI_DATA
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 6;

		uint16_t gpi_data : 10;
	} b;
	uint16_t w;
} NAFE13388_GPIDATA;


/*
 * NAFE13388_GPIDATA - Bit field mask definitions
 */
#define NAFE13388_GPI_DATA_GPIO0_MASK      	    		((uint16_t)0x0040)
#define NAFE13388_GPI_DATA_GPIO0_SHIFT    				((uint16_t)6)

#define NAFE13388_GPI_DATA_GPIO1_MASK     				((uint16_t)0x0080)
#define NAFE13388_GPI_DATA_GPIO1_SHIFT    				((uint16_t)7)

#define NAFE13388_GPI_DATA_GPIO2_MASK       			((uint16_t)0x0100)
#define NAFE13388_GPI_DATA_GPIO2_SHIFT      			((uint16_t)8)

#define NAFE13388_GPI_DATA_GPIO3_MASK      				((uint16_t)0x0200)
#define NAFE13388_GPI_DATA_GPIO3_SHIFT     				((uint16_t)9)

#define NAFE13388_GPI_DATA_GPIO4_MASK       			((uint16_t)0x0400)
#define NAFE13388_GPI_DATA_GPIO4_SHIFT     				((uint16_t)10)

#define NAFE13388_GPI_DATA_GPIO5_MASK       			((uint16_t)0x0800)
#define NAFE13388_GPI_DATA_GPIO5_SHIFT      			((uint16_t)11)

#define NAFE13388_GPI_DATA_GPIO6_MASK       			((uint16_t)0x1000)
#define NAFE13388_GPI_DATA_GPIO6_SHIFT      			((uint16_t)12)

#define NAFE13388_GPI_DATA_GPIO7_MASK       			((uint16_t)0x2000)
#define NAFE13388_GPI_DATA_GPIO7_SHIFT      			((uint16_t)13)

#define NAFE13388_GPI_DATA_GPIO8_MASK       			((uint16_t)0x4000)
#define NAFE13388_GPI_DATA_GPIO8_SHIFT      			((uint16_t)14)

#define NAFE13388_GPI_DATA_GPIO9_MASK       			((uint16_t)0x8000)
#define NAFE13388_GPI_DATA_GPIO9_SHIFT      			((uint16_t)15)

/*--------------------------------
 ** Register: GPI0_CONFIG0
 ** Enum: NAFE13388_GPIO_CONFIG0
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 6;

		uint16_t gpi_driving_enable : 10;
	} b;
	uint16_t w;
} NAFE13388_GPICONFIG0;


/*
 * NAFE13388_GPICONFIG0 - Bit field mask definitions
 */
#define NAFE13388_GPI_CONFIG0_GPIO0_MASK      	    		((uint16_t)0x0040)
#define NAFE13388_GPI_CONFIG0_GPIO0_SHIFT    				((uint16_t)6)

#define NAFE13388_GPI_CONFIG0_GPIO1_MASK     				((uint16_t)0x0080)
#define NAFE13388_GPI_CONFIG0_GPIO1_SHIFT    				((uint16_t)7)

#define NAFE13388_GPI_CONFIG0_GPIO2_MASK       			    ((uint16_t)0x0100)
#define NAFE13388_GPI_CONFIG0_GPIO2_SHIFT      			 	((uint16_t)8)

#define NAFE13388_GPI_CONFIG0_GPIO3_MASK      				((uint16_t)0x0200)
#define NAFE13388_GPI_CONFIG0_GPIO3_SHIFT     				((uint16_t)9)

#define NAFE13388_GPI_CONFIG0_GPIO4_MASK       				((uint16_t)0x0400)
#define NAFE13388_GPI_CONFIG0_GPIO4_SHIFT      				((uint16_t)10)

#define NAFE13388_GPI_CONFIG0_GPIO5_MASK       				((uint16_t)0x0800)
#define NAFE13388_GPI_CONFIG0_GPIO5_SHIFT      				((uint16_t)11)

#define NAFE13388_GPI_CONFIG0_GPIO6_MASK       				((uint16_t)0x1000)
#define NAFE13388_GPI_CONFIG0_GPIO6_SHIFT      				((uint16_t)12)

#define NAFE13388_GPI_CONFIG0_GPIO7_MASK       				((uint16_t)0x2000)
#define NAFE13388_GPI_CONFIG0_GPIO7_SHIFT      				((uint16_t)13)

#define NAFE13388_GPI_CONFIG0_GPIO8_MASK       				((uint16_t)0x4000)
#define NAFE13388_GPI_CONFIG0_GPIO8_SHIFT      				((uint16_t)14)

#define NAFE13388_GPI_CONFIG0_GPIO9_MASK       				((uint16_t)0x8000)
#define NAFE13388_GPI_CONFIG0_GPIO9_SHIFT      				((uint16_t)15)



/*--------------------------------
 ** Register: GPI0_CONFIG1
 ** Enum: NAFE13388_GPIO_CONFIG1
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 6;

		uint16_t gpio_con : 10;
	} b;
	uint16_t w;
} NAFE13388_GPICONFIG1;


/*
 * NAFE13388_GPICONFIG1 - Bit field mask definitions
 */
#define NAFE13388_GPI_CONFIG1_GPIO0_MASK      	    		((uint16_t)0x0040)
#define NAFE13388_GPI_CONFIG1_GPIO0_SHIFT    				((uint16_t)6)

#define NAFE13388_GPI_CONFIG1_GPIO1_MASK     				((uint16_t)0x0080)
#define NAFE13388_GPI_CONFIG1_GPIO1_SHIFT    				((uint16_t)7)

#define NAFE13388_GPI_CONFIG1_GPIO2_MASK       			    ((uint16_t)0x0100)
#define NAFE13388_GPI_CONFIG1_GPIO2_SHIFT      			 	((uint16_t)8)

#define NAFE13388_GPI_CONFIG1_GPIO3_MASK      				((uint16_t)0x0200)
#define NAFE13388_GPI_CONFIG1_GPIO3_SHIFT     				((uint16_t)9)

#define NAFE13388_GPI_CONFIG1_GPIO4_MASK       				((uint16_t)0x0400)
#define NAFE13388_GPI_CONFIG1_GPIO4_SHIFT      				((uint16_t)10)

#define NAFE13388_GPI_CONFIG1_GPIO5_MASK       				((uint16_t)0x0800)
#define NAFE13388_GPI_CONFIG1_GPIO5_SHIFT      				((uint16_t)11)

#define NAFE13388_GPI_CONFIG1_GPIO6_MASK       				((uint16_t)0x1000)
#define NAFE13388_GPI_CONFIG1_GPIO6_SHIFT      				((uint16_t)12)

#define NAFE13388_GPI_CONFIG1_GPIO7_MASK       				((uint16_t)0x2000)
#define NAFE13388_GPI_CONFIG1_GPIO7_SHIFT      				((uint16_t)13)

#define NAFE13388_GPI_CONFIG1_GPIO8_MASK       				((uint16_t)0x4000)
#define NAFE13388_GPI_CONFIG1_GPIO8_SHIFT      				((uint16_t)14)

#define NAFE13388_GPI_CONFIG1_GPIO9_MASK       				((uint16_t)0x8000)
#define NAFE13388_GPI_CONFIG1_GPIO9_SHIFT      				((uint16_t)15)


/*--------------------------------
 ** Register: GPI0_CONFIG1
 ** Enum: NAFE13388_GPIO_CONFIG1
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint16_t _reserved_ : 6;

		uint16_t gpio_read_enable : 10;
	} b;
	uint16_t w;
} NAFE13388_GPICONFIG2;


/*
 * NAFE13388_GPICONFIG2 - Bit field mask definitions
 */
#define NAFE13388_GPI_CONFIG2_GPIO0_MASK      	    		((uint16_t)0x0040)
#define NAFE13388_GPI_CONFIG2_GPIO0_SHIFT    				((uint16_t)6)

#define NAFE13388_GPI_CONFIG2_GPIO1_MASK     				((uint16_t)0x0080)
#define NAFE13388_GPI_CONFIG2_GPIO1_SHIFT    				((uint16_t)7)

#define NAFE13388_GPI_CONFIG2_GPIO2_MASK       			    ((uint16_t)0x0100)
#define NAFE13388_GPI_CONFIG2_GPIO2_SHIFT      			 	((uint16_t)8)

#define NAFE13388_GPI_CONFIG2_GPIO3_MASK      				((uint16_t)0x0200)
#define NAFE13388_GPI_CONFIG2_GPIO3_SHIFT     				((uint16_t)9)

#define NAFE13388_GPI_CONFIG2_GPIO4_MASK       				((uint16_t)0x0400)
#define NAFE13388_GPI_CONFIG2_GPIO4_SHIFT      				((uint16_t)10)

#define NAFE13388_GPI_CONFIG2_GPIO5_MASK       				((uint16_t)0x0800)
#define NAFE13388_GPI_CONFIG2_GPIO5_SHIFT      				((uint16_t)11)

#define NAFE13388_GPI_CONFIG2_GPIO6_MASK       				((uint16_t)0x1000)
#define NAFE13388_GPI_CONFIG2_GPIO6_SHIFT      				((uint16_t)12)

#define NAFE13388_GPI_CONFIG2_GPIO7_MASK       				((uint16_t)0x2000)
#define NAFE13388_GPI_CONFIG2_GPIO7_SHIFT      				((uint16_t)13)

#define NAFE13388_GPI_CONFIG2_GPIO8_MASK       				((uint16_t)0x4000)
#define NAFE13388_GPI_CONFIG2_GPIO8_SHIFT      				((uint16_t)14)

#define NAFE13388_GPI_CONFIG2_GPIO9_MASK       				((uint16_t)0x8000)
#define NAFE13388_GPI_CONFIG2_GPIO9_SHIFT      				((uint16_t)15)

#endif /* NAFE13388_H_ */
