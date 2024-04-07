/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_rtc_c.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "RTCC" in ASCII, used to determine if device is open. */
#define RTC_C_OPEN                             (0x52544343ULL)

#define RTC_C_MASK_MSB                         (0x0F)
#define RTC_C_MASK_LSB                         (0xF0)

#define RTC_C_FIRST_DAY_OF_A_MONTH             (1)

/* Month : valid range between 0 to 11.*/
#define RTC_C_MONTHS_IN_A_YEAR                 (11)
#define RTC_C_LAST_DAY_OF_LEAP_FEB_MONTH       (29U)
#define RTC_C_YEAR_VALUE_MIN                   (100)
#define RTC_C_YEAR_VALUE_MAX                   (199)

/* Seconds : valid range between 0 to 59.*/
#define RTC_C_SECONDS_IN_A_MINUTE              (59)

/* Minute : valid range between 0 to 59. */
#define RTC_C_MINUTES_IN_A_HOUR                (59)

/* Hours : valid range between 0 to 23. */
#define RTC_C_HOURS_IN_A_DAY                   (23)

/* Days of week : valid alarm range between 0 to 127. */
#define RTC_C_ACTUAL_DAYS_IN_A_WEEK            (0x7F)

/* In Zeller algorithm value of (-[Y/100] + [Y/400]) is 15 for Y = 2000 to Y = 2099) */
#define RTC_C_ZELLER_ALGM_CONST_FIFTEEN        (15)

/*The RTC has a 100 year calendar to match the starting year 2000, year offset(1900) is added like 117 + 1900 = 2017 */
#define RTC_C_TIME_H_YEAR_OFFSET               (1900)

/* Macro definitions for February and March months */
#define RTC_C_FEBRUARY_MONTH                   (2U)
#define RTC_C_MARCH_MONTH                      (3U)

/* The sum register value for "by {F[5:0] – 1} × 2" and "by {/F[5:0] + 1} × 2" settings */
#define RTC_C_INVERTED_ADJUSTMENT_VALUE_SUM    (65)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/* Armclang attempts to inline this function, which causes huge code size increases.
 * It is marked as __attribute__((noinline)) explicitly to prevent this. */
static uint8_t r_rtc_c_dec_to_bcd(uint8_t to_convert) __attribute__((noinline));
static uint8_t r_rtc_c_bcd_to_dec(uint8_t to_convert) __attribute__((noinline));

void rtc_c_alarm_prd_or_alm_isr(void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/* RTC Implementation of Real Time Clock  */
const rtc_api_t g_rtc_on_rtc_c =
{
    .open               = R_RTC_C_Open,
    .close              = R_RTC_C_Close,
    .clockSourceSet     = R_RTC_C_ClockSourceSet,
    .calendarTimeGet    = R_RTC_C_CalendarTimeGet,
    .calendarTimeSet    = R_RTC_C_CalendarTimeSet,
    .calendarAlarmGet   = R_RTC_C_CalendarAlarmGet,
    .calendarAlarmSet   = R_RTC_C_CalendarAlarmSet,
    .periodicIrqRateSet = R_RTC_C_PeriodicIrqRateSet,
    .infoGet            = R_RTC_C_InfoGet,
    .errorAdjustmentSet = R_RTC_C_ErrorAdjustmentSet,
    .callbackSet        = R_RTC_C_CallbackSet,
    .timeCaptureSet     = R_RTC_C_TimeCaptureSet,
    .timeCaptureGet     = R_RTC_C_TimeCaptureGet,
};

#if RTC_C_CFG_PARAM_CHECKING_ENABLE

/* Number of days in each months start from January to December */
static const uint8_t days_in_months[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
#endif

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

static void r_rtc_c_config_rtc_interrupts(rtc_c_instance_ctrl_t * const p_ctrl, rtc_cfg_t const * const p_cfg);

static void r_rtc_c_call_callback(rtc_c_instance_ctrl_t * p_ctrl, rtc_event_t event);

#if RTC_C_CFG_PARAM_CHECKING_ENABLE
static fsp_err_t r_rtc_c_err_adjustment_parameter_check(rtc_error_adjustment_cfg_t const * const err_adj_cfg);

static fsp_err_t r_rtc_c_time_and_date_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_c_time_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_c_date_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_c_alarm_time_validate(rtc_alarm_time_t * const p_time);

#endif

static void r_rtc_c_error_adjustment_set(rtc_error_adjustment_cfg_t const * const err_adj_cfg);

/*******************************************************************************************************************//**
 * @addtogroup RTC_C
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Opens and configures the RTC driver module. Implements @ref rtc_api_t::open.
 * Configuration includes clock source, and interrupt callback function.
 *
 * Example:
 * @snippet r_rtc_c_example.c R_RTC_C_Open
 *
 * @retval FSP_SUCCESS              Initialization was successful and RTC has started.
 * @retval FSP_ERR_UNSUPPORTED      Invalid clock source.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.
 * @retval FSP_ERR_INVALID_MODE     Subsystem clock should use SOSC before setting realtime clock to SOSC/256.
 * @retval FSP_ERR_ASSERTION        Invalid time parameter field.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_Open (rtc_ctrl_t * const p_ctrl, rtc_cfg_t const * const p_cfg)
{
    fsp_err_t               err             = FSP_SUCCESS;
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;

    /* Parameter checking */
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    rtc_c_extended_cfg * p_extend_cfg = (rtc_c_extended_cfg *) p_cfg->p_extend;
    FSP_ASSERT(NULL != p_extend_cfg);
    FSP_ERROR_RETURN(RTC_C_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
    FSP_ERROR_RETURN(0U == (R_SYSTEM_OSMC_WUTMMCK0_Msk & R_SYSTEM->OSMC) ||
                     RTC_CLOCK_SOURCE_SUBCLOCK_DIV_BY_1 == p_extend_cfg->clock_source_div,
                     FSP_ERR_INVALID_MODE);
#else
    rtc_c_extended_cfg * p_extend_cfg = (rtc_c_extended_cfg *) p_cfg->p_extend;
#endif

    /* Save the configuration  */
    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->p_callback = p_cfg->p_callback;
    p_instance_ctrl->p_context  = p_cfg->p_context;

    /* Enable the RTC channel and reset the registers to their initial state. */
    R_BSP_MODULE_START(FSP_IP_RTC, 0);

    R_RTC_C->RTCC0 = (uint8_t) ((uint8_t) (p_extend_cfg->clock_source_div << R_RTC_C_RTCC0_RTC128EN_Pos) |
                                R_RTC_C_RTCC0_AMPM_Msk);

    r_rtc_c_config_rtc_interrupts(p_instance_ctrl, p_cfg);

    /* Mark driver as open by initializing it to "RTC" in its ASCII equivalent. */
    p_instance_ctrl->open = RTC_C_OPEN;

    return err;
}

/*******************************************************************************************************************//**
 * Close the RTC driver.
 * Implements @ref rtc_api_t::close
 *
 * @retval FSP_SUCCESS              De-Initialization was successful and RTC driver closed.
 * @retval FSP_ERR_ASSERTION    Invalid p_ctrl.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for close.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_Close (rtc_ctrl_t * const p_ctrl)
{
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;

    /* Parameter checking */
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Set the START bit to 0 */
    R_RTC_C->RTCC0_b.RTCE = 0U;

    if (p_instance_ctrl->p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);
    }

    /* Enable the RTC_C and reset the registers to their initial state. */
    R_BSP_MODULE_STOP(FSP_IP_RTC, 0);

    if (p_instance_ctrl->p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->periodic_irq, NULL);
    }

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Setting clock source is not supported on the RTC_C.
 *
 * @retval FSP_ERR_UNSUPPORTED      Please set clock source in clock page.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_ClockSourceSet (rtc_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Set the calendar time.
 *
 * Implements @ref rtc_api_t::calendarTimeSet.
 *
 * @retval FSP_SUCCESS              Calendar time set operation was successful.
 * @retval FSP_ERR_ASSERTION        Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for operation.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid parameter field.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_CalendarTimeSet (rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time)
{
    fsp_err_t               err             = FSP_SUCCESS;
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;

#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    rtc_c_extended_cfg * p_extend_cfg = (rtc_c_extended_cfg *) p_instance_ctrl->p_cfg->p_extend;
    FSP_ASSERT(NULL != p_extend_cfg);
    FSP_ASSERT(NULL != p_time);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    if (0 != p_instance_ctrl->p_cfg->p_err_cfg->adjustment_value)
    {
        FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->p_err_cfg->adjustment_type != RTC_ERROR_ADJUSTMENT_NONE,
                         FSP_ERR_INVALID_ARGUMENT);
        FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->p_err_cfg->adjustment_period != RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND,
                         FSP_ERR_INVALID_ARGUMENT);
        FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->p_err_cfg->adjustment_period != RTC_ERROR_ADJUSTMENT_PERIOD_NONE,
                         FSP_ERR_INVALID_ARGUMENT);
    }

    /* Verify the seconds, minutes, hours, year ,day of the week, day of the month, month and year are valid values */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_c_time_and_date_validate(p_time), FSP_ERR_INVALID_ARGUMENT);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
    rtc_c_extended_cfg * p_extend_cfg = (rtc_c_extended_cfg *) p_instance_ctrl->p_cfg->p_extend;
#endif

    /* Set the START bit to 1 */
    R_RTC_C->RTCC0_b.RTCE = 1U;

    R_RTC_C->RTCC1 |= R_RTC_C_RTCC1_RWAIT_Msk;
    FSP_HARDWARE_REGISTER_WAIT(R_RTC_C->RTCC1_b.RWST, 1);

    /* Set the year, month, day of the week, ... */
    R_RTC_C->SEC   = r_rtc_c_dec_to_bcd((uint8_t) p_time->tm_sec);
    R_RTC_C->MIN   = r_rtc_c_dec_to_bcd((uint8_t) p_time->tm_min);
    R_RTC_C->HOUR  = r_rtc_c_dec_to_bcd((uint8_t) p_time->tm_hour);
    R_RTC_C->WEEK  = r_rtc_c_dec_to_bcd((uint8_t) p_time->tm_wday);
    R_RTC_C->DAY   = r_rtc_c_dec_to_bcd((uint8_t) p_time->tm_mday);
    R_RTC_C->MONTH = r_rtc_c_dec_to_bcd((uint8_t) (p_time->tm_mon + 1));
    R_RTC_C->YEAR  = r_rtc_c_dec_to_bcd((uint8_t) (p_time->tm_year - RTC_C_YEAR_VALUE_MIN));

    if (RTC_CLOCK_SOURCE_SUBCLOCK_DIV_BY_1 == p_extend_cfg->clock_source_div)
    {
        /* Set Error Adjustment values */
        r_rtc_c_error_adjustment_set(p_instance_ctrl->p_cfg->p_err_cfg);
    }

    R_RTC_C->RTCC1 &= (uint8_t) ~R_RTC_C_RTCC1_RWAIT_Msk;
    FSP_HARDWARE_REGISTER_WAIT(R_RTC_C->RTCC1_b.RWST, 0);

    return err;
}

/*******************************************************************************************************************//**
 * Get the calendar time.
 * @warning Do not call this function from a critical section or from an interrupt with higher priority than the carry
 * interrupt, or the time returned may be inaccurate.
 *
 * Implements @ref rtc_api_t::calendarTimeGet
 *
 * @retval FSP_SUCCESS          Calendar time get operation was successful.
 * @retval FSP_ERR_ASSERTION    Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN     Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_CalendarTimeGet (rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time)
{
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_time);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
#endif

    fsp_err_t err = FSP_SUCCESS;

    R_RTC_C->RTCC1 |= R_RTC_C_RTCC1_RWAIT_Msk;
    FSP_HARDWARE_REGISTER_WAIT(R_RTC_C->RTCC1_b.RWST, 1);

    /* Get the year, month, day of the week, ... */
    p_time->tm_sec  = r_rtc_c_bcd_to_dec(R_RTC_C->SEC);
    p_time->tm_min  = r_rtc_c_bcd_to_dec(R_RTC_C->MIN);
    p_time->tm_hour = r_rtc_c_bcd_to_dec(R_RTC_C->HOUR);
    p_time->tm_wday = r_rtc_c_bcd_to_dec(R_RTC_C->WEEK);
    p_time->tm_mday = r_rtc_c_bcd_to_dec(R_RTC_C->DAY);
    p_time->tm_mon  = r_rtc_c_bcd_to_dec(R_RTC_C->MONTH) - 1;
    p_time->tm_year = r_rtc_c_bcd_to_dec(R_RTC_C->YEAR) + RTC_C_YEAR_VALUE_MIN;

    R_RTC_C->RTCC1 &= (uint8_t) ~R_RTC_C_RTCC1_RWAIT_Msk;
    FSP_HARDWARE_REGISTER_WAIT(R_RTC_C->RTCC1_b.RWST, 0);

    return err;
}

/*******************************************************************************************************************//**
 * Set the calendar alarm time. For the p_alarm, only minutes, hours and weekdays are valid.
 * Minutes  0 to 59.
 * Hours    0 to 23.
 * Weekdays 0 to 127.
 *
 * Implements @ref rtc_api_t::calendarAlarmSet.
 *
 * @pre The calendar counter must be running before the alarm can be set.
 *
 * @retval FSP_SUCCESS              Calendar alarm time set operation was successful.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid time parameter field.
 * @retval FSP_ERR_ASSERTION        Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_CalendarAlarmSet (rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm)
{
    fsp_err_t               err             = FSP_SUCCESS;
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;

#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_alarm);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Verify the seconds, minutes, hours, year ,day of the week, day of the month and month are valid values */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_c_alarm_time_validate(p_alarm), FSP_ERR_INVALID_ARGUMENT);
#endif

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);
    R_RTC_C->RTCC1 &= (uint8_t) ~(R_RTC_C_RTCC1_WAFG_Msk | R_RTC_C_RTCC1_WALE_Msk);
    R_RTC_C->RTCC1 |= R_RTC_C_RTCC1_WALIE_Msk;

    /* Set alarm time */
    R_RTC_C->ALARMWM = r_rtc_c_dec_to_bcd((uint8_t) p_alarm->time.tm_min);

    R_RTC_C->ALARMWH = r_rtc_c_dec_to_bcd((uint8_t) p_alarm->time.tm_hour);

    R_RTC_C->ALARMWW = (uint8_t) p_alarm->time.tm_wday;

    R_RTC_C->RTCC1 |= R_RTC_C_RTCC1_WALE_Msk;
    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->periodic_irq);

    return err;
}

/*******************************************************************************************************************//**
 * Get the calendar alarm time. For the p_alarm, only minutes, hours and weekdays will be got.
 *
 * Implements @ref rtc_api_t::calendarAlarmGet
 *
 * @retval FSP_SUCCESS          Calendar alarm time get operation was successful.
 * @retval FSP_ERR_ASSERTION    Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN     Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_CalendarAlarmGet (rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm)
{
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_alarm);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
#endif

    /* Set alarm time */
    p_alarm->time.tm_min = r_rtc_c_bcd_to_dec(R_RTC_C->ALARMWM);

    p_alarm->time.tm_hour = r_rtc_c_bcd_to_dec(R_RTC_C->ALARMWH);

    p_alarm->time.tm_wday = R_RTC_C->ALARMWW;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set the periodic interrupt rate and enable periodic interrupt.
 *
 * Implements @ref rtc_api_t::periodicIrqRateSet
 *
 * @note To start the RTC @ref R_RTC_C_CalendarTimeSet must be called at least once.
 *
 * Example:
 * @snippet r_rtc_c_example.c R_RTC_C_PeriodicIrqRateSet
 *
 * @retval FSP_SUCCESS               The periodic interrupt rate was successfully set.
 * @retval FSP_ERR_ASSERTION         Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN          Driver not open already for operation.
 * @retval FSP_ERR_IRQ_BSP_DISABLED  User IRQ parameter not valid
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_PeriodicIrqRateSet (rtc_ctrl_t * const p_ctrl, rtc_periodic_irq_select_t const rate)
{
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->periodic_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);
    FSP_ERROR_RETURN(RTC_PERIODIC_IRQ_SELECT_1_DIV_BY_2_SECOND <= rate, FSP_ERR_ASSERTION);
    FSP_ERROR_RETURN(RTC_PERIODIC_IRQ_SELECT_2_SECOND != rate, FSP_ERR_ASSERTION);
#endif
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);

    uint8_t rtcc0 = R_RTC_C->RTCC0;
    rtcc0 &= (uint8_t) ~R_RTC_C_RTCC0_CT_Msk;

    /* The rtc_periodic_irq_select_t enum is from a shared API, and has some offsets from the bit value */
    if (rate < RTC_PERIODIC_IRQ_SELECT_2_SECOND)
    {
        R_RTC_C->RTCC0 = (uint8_t) (rtcc0 | (rate - RTC_PERIODIC_IRQ_SELECT_1_DIV_BY_4_SECOND));
    }
    else
    {
        R_RTC_C->RTCC0 = (uint8_t) (rtcc0 | (rate - RTC_PERIODIC_IRQ_SELECT_1_DIV_BY_2_SECOND));
    }

    R_RTC_C->RTCC1_b.RIFG = 0;
    FSP_HARDWARE_REGISTER_WAIT(R_RTC_C->RTCC1_b.RIFG, 0);

    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->periodic_irq);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Get RTC_C running status information ad store it in provided pointer p_rtc_info
 *
 * Implements @ref rtc_api_t::infoGet
 *
 * @retval FSP_SUCCESS              Get information Successful.
 * @retval FSP_ERR_ASSERTION        Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_InfoGet (rtc_ctrl_t * const p_ctrl, rtc_info_t * const p_rtc_info)
{
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_rtc_info);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
#endif

    p_rtc_info->status = (rtc_status_t) R_RTC_C->RTCC0_b.RTCE;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * This function sets time error adjustment
 *
 * Implements @ref rtc_api_t::errorAdjustmentSet
 *
 * @retval FSP_SUCCESS                 Time error adjustment successful.
 * @retval FSP_ERR_ASSERTION           Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN            Driver not open for operation.
 * @retval FSP_ERR_INVALID_ARGUMENT    Not under sub-clock or invalid error adjustment value.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_ErrorAdjustmentSet (rtc_ctrl_t * const p_ctrl, rtc_error_adjustment_cfg_t const * const err_adj_cfg)
{
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;
#if RTC_C_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_ctrl);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    rtc_c_extended_cfg * p_extend_cfg = (rtc_c_extended_cfg *) p_instance_ctrl->p_cfg->p_extend;

    /* Error adjustment is supported only if clock source is sub-clock */
    if (RTC_CLOCK_SOURCE_SUBCLOCK_DIV_BY_256 == p_extend_cfg->clock_source_div)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Verify the frequency comparison value for RFRL when using LOCO */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_c_err_adjustment_parameter_check(err_adj_cfg), FSP_ERR_INVALID_ARGUMENT);
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif

    /* Set Error Adjustment values */
    r_rtc_c_error_adjustment_set(err_adj_cfg);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements rtc_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Baud rate was successfully changed.
 * @retval  FSP_ERR_ASSERTION            Pointer to RTC control block is NULL or the RTC is not configured to use the
 *                                       internal clock.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_CallbackSet (rtc_ctrl_t * const          p_ctrl,
                               void (                    * p_callback)(rtc_callback_args_t *),
                               void const * const          p_context,
                               rtc_callback_args_t * const p_callback_memory)
{
    rtc_c_instance_ctrl_t * p_instance_ctrl = (rtc_c_instance_ctrl_t *) p_ctrl;

#if (RTC_C_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_callback);
    FSP_ASSERT(NULL == p_callback_memory);
    FSP_ERROR_RETURN(RTC_C_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif
    FSP_PARAMETER_NOT_USED(p_callback_memory);

    /* Store callback and context */
    p_instance_ctrl->p_callback = p_callback;
    p_instance_ctrl->p_context  = p_context;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Capture is not supported on the RTC_C.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_TimeCaptureSet (rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_time_capture);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Capture is not supported on the RTC_C.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_TimeCaptureGet (rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_time_capture);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup RTC_C)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Set IRQ priority and control info for IRQ handler .
 *
 * @param[in]  p_ctrl                  Instance control block
 * @param[in]  p_cfg                   Pointer to rtc configuration.
 **********************************************************************************************************************/
static void r_rtc_c_config_rtc_interrupts (rtc_c_instance_ctrl_t * const p_ctrl, rtc_cfg_t const * const p_cfg)
{
    if (p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqCfg(p_cfg->periodic_irq, p_cfg->periodic_ipl, p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_ctrl     Pointer to RTC instance control block
 * @param[in]     event      Event code
 **********************************************************************************************************************/
static void r_rtc_c_call_callback (rtc_c_instance_ctrl_t * p_ctrl, rtc_event_t event)
{
    rtc_callback_args_t args;

    args.event     = event;
    args.p_context = p_ctrl->p_context;
    p_ctrl->p_callback(&args);
}

#if RTC_C_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * Validate Error Adjustment configuration when using SubClock
 *
 * @param[in]  err_adj_cfg             Pointer to error adjustment config
 * @retval FSP_SUCCESS                 Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT    Invalid error configuration
 **********************************************************************************************************************/
static fsp_err_t r_rtc_c_err_adjustment_parameter_check (rtc_error_adjustment_cfg_t const * const err_adj_cfg)
{
    rtc_error_adjustment_period_t period = err_adj_cfg->adjustment_period;

    /* Validate period value */
    if ((RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND == period) || (RTC_ERROR_ADJUSTMENT_PERIOD_NONE == period))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    rtc_error_adjustment_t mode = err_adj_cfg->adjustment_type;

    /* Validate adjustment type value */
    if (RTC_ERROR_ADJUSTMENT_NONE == mode)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Validate time and date fields of time parameter fields
 * Checking for seconds, minutes, hours are valid values by calling sub-function time validate.
 * Checking for year, month, day of the week and day of a month are valid values by calling sub-function
 * date validate.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_c_time_and_date_validate (rtc_time_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    err = r_rtc_c_time_validate(p_time);
    FSP_ERROR_RETURN(err == FSP_SUCCESS, err);
    err = r_rtc_c_date_validate(p_time);
    FSP_ERROR_RETURN(err == FSP_SUCCESS, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Validate time fields of time type parameter
 * Checking for the seconds, minutes, hours values for valid specified range.
 * Seconds 0 to 59.
 * Minutes 0 to 59.
 * Hours   0 to 23.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_c_time_validate (rtc_time_t * p_time)
{
    fsp_err_t err;
    err = FSP_SUCCESS;
    if ((p_time->tm_sec < 0) || (p_time->tm_sec > RTC_C_SECONDS_IN_A_MINUTE) ||
        (p_time->tm_min < 0) || (p_time->tm_min > RTC_C_MINUTES_IN_A_HOUR) ||
        (p_time->tm_hour < 0) || (p_time->tm_hour > RTC_C_HOURS_IN_A_DAY))
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Validate date fields of time type parameter
 * validating r_rtc_c date fields and setting day of a Week using Zeller's congruence.
 * Checking for year, month, day of the week and day of a month are valid values.
 * Leap year validation and Week of the day is calculated and updated in rtc time.
 * Day of week between 0 to 6
 * Day between 1 to 31
 * Month between 0 to 11 as per standard time.h, There's a mismatch between hardware configuration,
 * UM indicates that "A value from 01 through 12 (in BCD) can be specified" for Month Counter register in the RTC.
 * This difference will be taken care in the Set and Get functions.
 *
 * As per HW manual, value of Year is between 0 to 99, the RTC has a 100 year calendar from 2000 to 2099.
 * (see section 26.1 "Overview" of the RA6M3 manual R01UH0886EJ0100)
 * But as per C standards, tm_year is years since 1900.
 * A sample year set in an application would be like time.tm_year = 2019-1900; (to set year 2019)
 * Since RTC API follows the Date and time structure defined in C standard library <time.h>, the valid value of year is
 * between 100 and 199, which will be internally converted to HW required value.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_c_date_validate (rtc_time_t * const p_time)
{
    uint32_t day_of_week;
    uint32_t num_days_month;
    uint32_t day_of_a_month;
    uint32_t temp_month;
    uint32_t temp_year;

    day_of_a_month = (uint32_t) p_time->tm_mday;
    temp_month     = (uint32_t) (p_time->tm_mon + 1);

    /* The valid value of year is between 100 to 199, The RTC has a 100 year calendar from 2000 to 2099
     * to match the starting year 2000, a sample year offset(1900) is added like 117 + 1900 = 2017*/
    temp_year = (uint32_t) (p_time->tm_year + RTC_C_TIME_H_YEAR_OFFSET);

    /* Checking the error condition for year and months values, here valid value of year is between 100 to 199
     * and for month 0 to 11*/
    if ((p_time->tm_year < RTC_C_YEAR_VALUE_MIN) || (p_time->tm_year > RTC_C_YEAR_VALUE_MAX) ||
        (p_time->tm_mon < 0) || (p_time->tm_mon > RTC_C_MONTHS_IN_A_YEAR))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /*For particular valid month, number of days in a month is updated */
    num_days_month = days_in_months[p_time->tm_mon];

    /* Checking for February month and Conditions for Leap year : Every fourth year is a leap year,
     * The RTC has a 100 year calendar from 2000 to 2099  */
    if ((RTC_C_FEBRUARY_MONTH == temp_month) && (0 == (temp_year % 4U)))
    {
        num_days_month = RTC_C_LAST_DAY_OF_LEAP_FEB_MONTH;
    }

    /* Checking for day of a month values for valid range */
    if ((day_of_a_month >= RTC_C_FIRST_DAY_OF_A_MONTH) && (day_of_a_month <= num_days_month))
    {
        /* Adjust month to run from 3 to 14 for March to February */
        if (temp_month < RTC_C_MARCH_MONTH)
        {
            temp_month = (temp_month + 12U);

            /* Adjust year if January or February*/
            --temp_year;
        }

        /*For the Gregorian calendar, Zeller's congruence formulas is
         * h = ( q + [13(m+1)/5] + Y + [Y/4] - [Y/100] + [Y/400])mod 7 (mod : modulo)
         * h is the day of the week , q is the day of the month,
         * m is the month (3 = March, 4 = April,..., 14 = February)
         * Y is year, which is Y - 1 during January and February */
        day_of_week = (uint32_t) p_time->tm_mday + ((13 * (temp_month + 1)) / 5) + temp_year + (temp_year / 4);
        day_of_week = (day_of_week - RTC_C_ZELLER_ALGM_CONST_FIFTEEN) % 7;

        /* Day of week between 0 to 6 :- Sunday to Saturday */
        /* d = (h + 6)mod 7 (mod : modulo) */
        p_time->tm_wday = (int16_t) ((day_of_week + 6U) % 7U);

        return FSP_SUCCESS;
    }

    return FSP_ERR_INVALID_ARGUMENT;
}

/*******************************************************************************************************************//**
 * Validate alarm time fields of Alarm time type parameter
 * Checking for alarm enable bit with the seconds, minutes, hours value for valid specified range.
 * Minutes 0 to 59.
 * Hours   0 to 23.
 *
 * @param[in]  p_time                     Pointer to rtc_alarm_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_c_alarm_time_validate (rtc_alarm_time_t * const p_time)
{
    fsp_err_t err;
    err = FSP_SUCCESS;
    if ((p_time->time.tm_min < 0) || (p_time->time.tm_min > RTC_C_MINUTES_IN_A_HOUR) ||
        (p_time->time.tm_hour < 0) || (p_time->time.tm_hour > RTC_C_HOURS_IN_A_DAY) ||
        (p_time->time.tm_mday < 0) || (p_time->time.tm_mday > RTC_C_ACTUAL_DAYS_IN_A_WEEK))
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

#endif

/*******************************************************************************************************************//**
 * This function sets time error adjustment mode, period, type and value.
 *
 *@param[in] err_adj_cfg  Pointer to the Error Adjustment Configuration
 **********************************************************************************************************************/
static void r_rtc_c_error_adjustment_set (rtc_error_adjustment_cfg_t const * const err_adj_cfg)
{
    rtc_error_adjustment_period_t period = err_adj_cfg->adjustment_period;
    rtc_error_adjustment_t        type   = err_adj_cfg->adjustment_type;

    uint8_t  reg_subcud = 0;
    uint32_t value      = err_adj_cfg->adjustment_value;

    if (RTC_ERROR_ADJUSTMENT_PERIOD_1_MINUTE == period)
    {
        reg_subcud = R_RTC_C_SUBCUD_DEV_Msk;
    }

    if (0 != value)
    {
        uint8_t adjustValue = 0U;
        adjustValue = (uint8_t) ((value / 2) + 1);

        if (RTC_ERROR_ADJUSTMENT_ADD_PRESCALER == type)
        {
            reg_subcud |= adjustValue;
        }
        else
        {
            reg_subcud |= (uint8_t) (R_RTC_C_SUBCUD_F6_Msk | (RTC_C_INVERTED_ADJUSTMENT_VALUE_SUM - adjustValue));
        }
    }

    R_RTC_C->SUBCUD = reg_subcud;
}

/*******************************************************************************************************************//**
 * Convert decimal to BCD
 *
 * @param[in] to_convert   Decimal Value to be converted
 **********************************************************************************************************************/
static uint8_t r_rtc_c_dec_to_bcd (uint8_t to_convert)
{
    return (uint8_t) ((((to_convert / (uint8_t) 10) << 4) & (uint8_t) RTC_C_MASK_LSB) | (to_convert % (uint8_t) 10));
}

/*******************************************************************************************************************//**
 * Convert  BCD to decimal
 *
 * @param[in] to_convert   BCD Value to be converted
 **********************************************************************************************************************/
static uint8_t r_rtc_c_bcd_to_dec (uint8_t to_convert)
{
    return (uint8_t) ((((to_convert & (uint8_t) RTC_C_MASK_LSB) >> 4) * (uint8_t) 10) +
                      (to_convert & (uint8_t) RTC_C_MASK_MSB));
}

/*******************************************************************************************************************//**
 * RTC Callback ISR for alarm and periodic interrupt.
 **********************************************************************************************************************/
void rtc_c_alarm_prd_or_alm_isr (void)
{
    IRQn_Type               irq    = R_FSP_CurrentIrqGet();
    rtc_c_instance_ctrl_t * p_ctrl = (rtc_c_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Call the callback routine if one is available */
    if (NULL != p_ctrl->p_callback)
    {
        /* Set data to identify callback to user, then call user callback. */
        rtc_event_t event;
        if (1 == R_RTC_C->RTCC1_b.WAFG)
        {
            /* Alarm event is unstable with LOCO clock source. Do not call user callback */
            if (1 == R_SYSTEM->OSMC_b.WUTMMCK0)
            {
                return;
            }

            event           = RTC_EVENT_ALARM_IRQ;
            R_RTC_C->RTCC1 &= (uint8_t) ~R_RTC_C_RTCC1_WAFG_Msk;
        }
        else
        {
            event           = RTC_EVENT_PERIODIC_IRQ;
            R_RTC_C->RTCC1 &= (uint8_t) ~R_RTC_C_RTCC1_RIFG_Msk;
        }

        /* Call callback */
        r_rtc_c_call_callback(p_ctrl, event);
    }
}
