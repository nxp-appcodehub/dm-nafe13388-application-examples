/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file frdmmcxn947.c
 * @brief The frdmmcxn947.c file defines GPIO pins and I2C CMSIS utilities for frdmmcxn947 board.
 */

#include "frdmmcxn947.h"

// I2C2 Pin Handles
gpioHandleKSDK_t D14 = {.base = GPIO4, .pinNumber = 0, .mask = 1 << (0), .clockName = kCLOCK_Gpio4, .portNumber = 4};
gpioHandleKSDK_t D15 = {.base = GPIO4, .pinNumber = 1, .mask = 1 << (1), .clockName = kCLOCK_Gpio4, .portNumber = 4};

// SPI1 Handle
/* [PIO0_20] (coords: 74) configured as SPI master-out/slave-in (MOSI) data */
gpioHandleKSDK_t D11 = {.base = GPIO0, .pinNumber = 24, .mask = 1 << (24), .clockName = kCLOCK_Gpio0, .portNumber = 0};
/* [PIO0_19] (coords: 90) is configured as SPI master-in/slave-out (MISO) data */
gpioHandleKSDK_t D12 = {.base = GPIO0, .pinNumber = 26, .mask = 1 << (26), .clockName = kCLOCK_Gpio0, .portNumber = 0};
/* [PIO0_21] (coords: 76) configured as SPI SCK */
gpioHandleKSDK_t D13 = {.base = GPIO0, .pinNumber = 25, .mask = 1 << (25), .clockName = kCLOCK_Gpio0, .portNumber = 0};
/* [PIO0_21] (coords: 76) configured as SPI SCK */
gpioHandleKSDK_t D10 = {.base = GPIO0, .pinNumber = 27, .mask = 1 << (27), .clockName = kCLOCK_Gpio0, .portNumber = 0};

gpioHandleKSDK_t D2 = {.base = GPIO0, .pinNumber = 29, .mask = 1 << (29), .clockName = kCLOCK_Gpio0, .portNumber = 0};

//GPIO Pin Handles
gpioHandleKSDK_t NAFE_DRDY = {.base = GPIO0, .pinNumber = 30, .mask = 1 << (30), .irq = GPIO00_IRQn , .clockName = kCLOCK_Gpio0, .portNumber = 0};

gpioHandleKSDK_t NAFE_RST = {.base = GPIO0, .pinNumber = 31, .mask = 1 << (31), .clockName = kCLOCK_Gpio0, .portNumber = 0};

gpioHandleKSDK_t ADC_SYNC = {.base = GPIO1, .pinNumber = 2, .mask = 1 << (2), .clockName = kCLOCK_Gpio1, .portNumber = 1};

gpioHandleKSDK_t NAFE_nINT = {.base = GPIO1, .pinNumber = 23, .mask = 1 << (23), .clockName = kCLOCK_Gpio1, .portNumber = 1};

gpioHandleKSDK_t INTB_PIN = {.base = GPIO0, .pinNumber = 10, .mask = 1 << (10), .irq = GPIO01_IRQn , .clockName = kCLOCK_Gpio0, .portNumber = 0};


gpioConfigKSDK_t NAFE_DRDY_config = {
		.pinConfig = {kGPIO_DigitalInput, 0}, .portPinConfig = {0}, .interruptMode = kGPIO_InterruptRisingEdge};

gpioConfigKSDK_t NAFE_RST_config = {
		.pinConfig = {kGPIO_DigitalOutput, 1}};

gpioConfigKSDK_t ADC_SYNC_config = {
		.pinConfig = {kGPIO_DigitalOutput, 0}};

gpioConfigKSDK_t NAFE_nINT_config = {
		.pinConfig = {kGPIO_DigitalInput, 0}};

// LPCXpresso55s69 Internal Peripheral Pin Definitions
gpioHandleKSDK_t RED_LED = {
    .base = GPIO0, .pinNumber = 10, .mask = 1 << (10), .clockName = kCLOCK_Gpio0, .portNumber = 0};
gpioHandleKSDK_t GREEN_LED = {
    .base = GPIO0, .pinNumber = 27, .mask = 1 << (27), .clockName = kCLOCK_Gpio0, .portNumber = 0};
gpioHandleKSDK_t BLUE_LED = {
    .base = GPIO1, .pinNumber = 2, .mask = 1 << (2), .clockName = kCLOCK_Gpio1, .portNumber = 1};

/*!
 * @brief Kinetis style Wrapper API for handling all Clock related configurations.
 *
 * @param void
 * @return void
 */
void BOARD_BootClockRUN(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug/UART0) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0);
    /* attach 12 MHz clock to FLEXCOMM4 (I2C4) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
    /* attach 12 MHz clock to FLEXCOMM4 (SPI7) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    BOARD_PowerMode_OD();
    /* Configure FROHF96M Clock */
    BOARD_BootClockPLL150M();
}

/*!
 * @brief Configures the system to WAIT power mode.
 *        API name used from Kinetis family to maintain compatibility.
 *
 * @param Power peripheral base address (dummy).
 * @return Configuration error code.
 */
status_t SMC_SetPowerModeWait(void *arg)
{
    // POWER_EnterSleep();

    return kStatus_Success;
}

/*!
 * @brief Configures the system to VLPR power mode.
 *        API name used from Kinetis family to maintain compatibility.
 *
 * @param Power peripheral base address (dummy).
 * @return Configuration error code.
 */
status_t SMC_SetPowerModeVlpr(void *arg)
{
    // POWER_EnterSleep();

    return kStatus_Success;
}

/*! @brief       Determines the Clock Frequency feature.
 *  @details     The Clock Frequecny computation API required by fsl_uart_cmsis.c.
 *  @param[in]   void
 *  @Constraints None
 *  @Reentrant   Yes
 *  @return      uint32_t Returns the clock frequency .
 */
uint32_t LPUART4_GetFreq(void)
{
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);

    return CLOCK_GetLPFlexCommClkFreq(4U);
}

/*! @brief       Determines the Clock Frequency feature.
 *  @details     The Clock Frequecny computation API required by fsl_i2c_cmsis.c.
 *  @param[in]   void
 *  @Constraints None
 *  @Reentrant   Yes
 *  @return      uint32_t Returns the clock frequency .
 */
uint32_t LPI2C2_GetFreq(void)
{
    /* attach FRO 12M to FLEXCOMM2 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    return CLOCK_GetLPFlexCommClkFreq(2U);
}

/*! @brief       Determines the Clock Frequency feature.
 *  @details     The Clock Frequecny computation API required by fsl_spi_cmsis.c.
 *  @param[in]   void
 *  @Constraints None
 *  @Reentrant   Yes
 *  @return      uint32_t Returns the clock frequency .
 */
uint32_t LPSPI1_GetFreq(void)
{
    /* attach FRO 12M to FLEXCOMM1 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1);

    return CLOCK_GetLPFlexCommClkFreq(1U);
}
