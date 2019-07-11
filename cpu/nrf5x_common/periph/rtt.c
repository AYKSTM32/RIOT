/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT implementation for NRF5x CPUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "nrf_clock.h"
#include "periph/rtt.h"

/* get the IRQ configuration */
#if (RTT_DEV == 1)
#define DEV             NRF_RTC1
#define ISR             isr_rtc1
#define IRQn            RTC1_IRQn
#elif (RTT_DEV == 2)
#define DEV             NRF_RTC2
#define ISR             isr_rtc2
#define IRQn            RTC2_IRQn
#else
#error "RTT configuration: invalid or no RTC device specified (RTT_DEV)"
#endif

#define LFCLK_FREQ      (32768U)

/* allocate memory for callbacks and their args */
static rtt_cb_t alarm_cb;
static void *alarm_arg;
static rtt_cb_t overflow_cb;
static void *overflow_arg;
static rtt_cb_t periodic_cb;
static void *periodic_arg;

static volatile uint32_t wakeup_period_us;

void rtt_init(void)
{
    /* enable the low-frequency clock */
    clock_start_lf();
    /* make sure device is powered */
#ifdef CPU_FAM_NRF51
    DEV->POWER = 1;
#endif
    /* stop the RTT during configuration */
    DEV->TASKS_STOP = 1;
    /* configure interrupt */
    NVIC_EnableIRQ(IRQn);
    /* set prescaler */
    DEV->PRESCALER = (LFCLK_FREQ / RTT_FREQUENCY) - 1;
    /* start the actual RTT thing */
    DEV->TASKS_START = 1;
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    overflow_cb = cb;
    overflow_arg = arg;
    DEV->INTENSET = RTC_INTENSET_OVRFLW_Msk;
}

void rtt_clear_overflow_cb(void)
{
    DEV->INTENCLR = RTC_INTENCLR_OVRFLW_Msk;
}

uint32_t rtt_get_counter(void)
{
    return DEV->COUNTER;
}

void rtt_set_counter(uint32_t counter)
{
    (void) counter;
    /* not supported by the nRF5x */
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    alarm_cb = cb;
    alarm_arg = arg;
    if (alarm > RTT_MAX_VALUE) {
        alarm = RTT_MAX_VALUE;
    }
    DEV->CC[0] = alarm;
    DEV->INTENSET = RTC_INTENSET_COMPARE0_Msk;
}

uint32_t rtt_get_alarm(void)
{
    return DEV->CC[0];
}

void rtt_clear_alarm(void)
{
    DEV->INTENCLR = RTC_INTENSET_COMPARE0_Msk;
}

void rtt_poweron(void)
{
#ifdef CPU_FAM_NRF51
    DEV->POWER = 1;
#endif
    DEV->TASKS_START = 1;
}

void rtt_poweroff(void)
{
    DEV->TASKS_STOP = 1;
#ifdef CPU_FAM_NRF51
    DEV->POWER = 0;
#endif
}

void rtt_periodic_task_set(uint32_t period_us, rtt_cb_t cb, void *arg) {
    wakeup_period_us = period_us;
    periodic_cb = cb;
    periodic_arg = arg;

    DEV->CC[1] = (RTT_US_TO_TICKS(period_us) + DEV->COUNTER) % RTT_MAX_VALUE;
}

void rtt_periodic_task_start(void) {
    DEV->INTENSET = RTC_INTENSET_COMPARE1_Msk;
}

void rtt_periodic_task_stop(void) {
    DEV->INTENCLR= RTC_INTENSET_COMPARE1_Msk;
}

void ISR(void)
{
    /* regular timer */
    if (DEV->EVENTS_COMPARE[0] == 1) {
        DEV->EVENTS_COMPARE[0] = 0;
        DEV->INTENCLR = RTC_INTENSET_COMPARE0_Msk;
        alarm_cb(alarm_arg);
    }
    
    /* periodic timer */
    if (DEV->EVENTS_COMPARE[1] == 1) {
        DEV->EVENTS_COMPARE[1] = 0;
        DEV->CC[1] = (RTT_US_TO_TICKS(wakeup_period_us) + DEV->COUNTER) % RTT_MAX_VALUE;
        periodic_cb(periodic_arg);
    }

    /* overflow event */
    if (DEV->EVENTS_OVRFLW == 1) {
        DEV->EVENTS_OVRFLW = 0;
        overflow_cb(overflow_arg);
    }

    cortexm_isr_end();
}
