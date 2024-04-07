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

#ifndef R_RTC_C_H
#define R_RTC_C_H

/*******************************************************************************************************************//**
 * @addtogroup RTC_C
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_rtc_c_cfg.h"
#include "r_rtc_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** RTC sub_clock division */
typedef enum e_rtc_c_subclock_division
{
    RTC_CLOCK_SOURCE_SUBCLOCK_DIV_BY_1   = 0, ///< Using sub_clock for rct_c clock
    RTC_CLOCK_SOURCE_SUBCLOCK_DIV_BY_256 = 1, ///< Using (sub_clock / 256) for rct_c clock
} rtc_c_subclock_division_t;

/** RTC extended configuration */
typedef struct st_rtc_c_extended_cfg
{
    rtc_c_subclock_division_t clock_source_div; ///< The sub clock division value
} rtc_c_extended_cfg;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when @ref rtc_api_t::open is called */
typedef struct st_rtc_c_instance_ctrl
{
    uint32_t          open;                     ///< Whether or not driver is open
    const rtc_cfg_t * p_cfg;                    ///< Pointer to initial configurations

    void (* p_callback)(rtc_callback_args_t *); // Pointer to callback that is called when a rtc_event_t occurs.

    void const * p_context;                     // Pointer to context to be passed into callback function
} rtc_c_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const rtc_api_t g_rtc_on_rtc_c;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t R_RTC_C_Open(rtc_ctrl_t * const p_ctrl, rtc_cfg_t const * const p_cfg);
fsp_err_t R_RTC_C_Close(rtc_ctrl_t * const p_ctrl);
fsp_err_t R_RTC_C_ClockSourceSet(rtc_ctrl_t * const p_ctrl);
fsp_err_t R_RTC_C_CalendarTimeSet(rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time);
fsp_err_t R_RTC_C_CalendarTimeGet(rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time);
fsp_err_t R_RTC_C_CalendarAlarmSet(rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm);
fsp_err_t R_RTC_C_CalendarAlarmGet(rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm);
fsp_err_t R_RTC_C_PeriodicIrqRateSet(rtc_ctrl_t * const p_ctrl, rtc_periodic_irq_select_t const rate);
fsp_err_t R_RTC_C_ErrorAdjustmentSet(rtc_ctrl_t * const p_ctrl, rtc_error_adjustment_cfg_t const * const err_adj_cfg);
fsp_err_t R_RTC_C_InfoGet(rtc_ctrl_t * const p_ctrl, rtc_info_t * const p_rtc_info);
fsp_err_t R_RTC_C_CallbackSet(rtc_ctrl_t * const          p_ctrl,
                              void (                    * p_callback)(rtc_callback_args_t *),
                              void const * const          p_context,
                              rtc_callback_args_t * const p_callback_memory);
fsp_err_t R_RTC_C_TimeCaptureSet(rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture);
fsp_err_t R_RTC_C_TimeCaptureGet(rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // R_RTC_C_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RTC_C)
 **********************************************************************************************************************/
