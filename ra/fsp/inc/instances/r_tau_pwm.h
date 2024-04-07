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

#ifndef R_TAU_PWM_H
#define R_TAU_PWM_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_tau_pwm_cfg.h"
#include "r_timer_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/*******************************************************************************************************************//**
 * @addtogroup TAU_PWM
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define TAU_PWM_PRV_MAX_NUM_CHANNELS    (7U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Operation clock. */
typedef enum e_tau_pwm_operation_clock
{
    TAU_PWM_OPERATION_CLOCK_CK00 = 0U, ///< Operation Clock CK00
    TAU_PWM_OPERATION_CLOCK_CK01 = 2U, ///< Operation CLock CK01
} tau_pwm_operation_clock_t;

/** Trigger Source */
typedef enum e_tau_pwm_source
{
    TAU_PWM_SOURCE_PIN_INPUT = 0U,     ///< Use TI0n pin input as trigger source
    TAU_PWM_SOURCE_ELC_EVENT = 1U,     ///< Use ELC events as trigger source
} tau_pwm_source_t;

/**TI0n pin input edge */
typedef enum e_tau_pwm_detect_edge
{
    TAU_PWM_DETECT_EDGE_FALLING = 0U,  ///< Detects falling edge
    TAU_PWM_DETECT_EDGE_RISING  = 1U,  ///< Detects rising edge
    TAU_PWM_DETECT_EDGES_BOTH   = 2U,  ///< Detects both edges
} tau_pwm_detect_edge_t;

/** Level of TAU pin */
typedef enum e_tau_pwm_output_level
{
    TAU_PWM_OUTPUT_LEVEL_LOW  = 0U,    ///< Pin level low
    TAU_PWM_OUTPUT_LEVEL_HIGH = 1U,    ///< Pin level high
} tau_pwm_output_level_t;

/** Timer output polarity */
typedef enum e_tau_pwm_output_polarity
{
    TAU_PWM_OUTPUT_POLARITY_ACTIVE_HIGH = 0U, ///< Positive logic output (active-high)
    TAU_PWM_OUTPUT_POLARITY_ACTIVE_LOW  = 1U, ///< Negative logic output (active-low)
} tau_pwm_output_polarity_t;

/** Input/Output pins, used to select which duty cycle to update in R_TAU_PWM_DutyCycleSet(). */
typedef enum e_tau_pwm_io_pin
{
    TAU_PWM_IO_PIN_CHANNEL_0 = 0,      ///< I/O pin of channel 0
    TAU_PWM_IO_PIN_CHANNEL_1 = 1,      ///< I/O pin of channel 1
    TAU_PWM_IO_PIN_CHANNEL_2 = 2,      ///< I/O pin of channel 2
    TAU_PWM_IO_PIN_CHANNEL_3 = 3,      ///< I/O pin of channel 3
    TAU_PWM_IO_PIN_CHANNEL_4 = 4,      ///< I/O pin of channel 4
    TAU_PWM_IO_PIN_CHANNEL_5 = 5,      ///< I/O pin of channel 5
    TAU_PWM_IO_PIN_CHANNEL_6 = 6,      ///< I/O pin of channel 6
    TAU_PWM_IO_PIN_CHANNEL_7 = 7,      ///< I/O pin of channel 7
} tau_pwm_io_pin_t;

/** TAU_PWM per channel configuration. */
typedef struct st_tau_pwm_channel_cfg
{
    uint8_t  channel;                          ///< Slave Channel Number (1..7)
    uint16_t duty_cycle_counts;                ///< One-shot: Pulse_width_counts; PWM: Duty_cycle_counts

    /* Output pin settings. */
    tau_pwm_output_level_t    output_level;    ///< Setting of output level for TAU
    tau_pwm_output_polarity_t output_polarity; ///< Setting of output polarity for TAU

    uint8_t   cycle_end_ipl;                   ///< TAU slave channel IPL
    IRQn_Type cycle_end_irq;                   ///< TAU slave channel IRQ
} tau_pwm_channel_cfg_t;

/** Extended configuration structure for TAU_PWM */
typedef struct st_tau_pwm_extended_cfg
{
    tau_pwm_operation_clock_t operation_clock;                                        ///< Setting of operation clock for master and slave channels

    /* Input settings. */
    tau_pwm_source_t      trigger_source;                                             ///< Trigger source for master channel
    tau_pwm_detect_edge_t detect_edge;                                                ///< Trigger edge to start pulse period measurement

    tau_pwm_channel_cfg_t const * p_slave_channel_cfgs[TAU_PWM_PRV_MAX_NUM_CHANNELS]; ///< Configuration for each slave channel, at least 1 slave channel is required
} tau_pwm_extended_cfg_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when @ref timer_api_t::open is called. */
typedef struct st_tau_pwm_instance_ctrl
{
    uint32_t            open;                     // Whether or not channel is open
    const timer_cfg_t * p_cfg;                    // Pointer to initial configurations
    uint8_t             channels_mask;            // Masked value of all channels used
    uint8_t             master_channel_mask;      // Masked value of master channel used
    uint8_t             slave_channels_mask;      // Masked value of slave channels used

    void (* p_callback)(timer_callback_args_t *); // Pointer to callback that is called when a timer_event_t occurs.
    void const * p_context;                       // Pointer to context to be passed into callback function
} tau_pwm_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Interface Structure for user access */
extern const timer_api_t g_timer_on_tau_pwm;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t R_TAU_PWM_Open(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t R_TAU_PWM_Stop(timer_ctrl_t * const p_ctrl);
fsp_err_t R_TAU_PWM_Start(timer_ctrl_t * const p_ctrl);
fsp_err_t R_TAU_PWM_Reset(timer_ctrl_t * const p_ctrl);
fsp_err_t R_TAU_PWM_Enable(timer_ctrl_t * const p_ctrl);
fsp_err_t R_TAU_PWM_Disable(timer_ctrl_t * const p_ctrl);
fsp_err_t R_TAU_PWM_PeriodSet(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
fsp_err_t R_TAU_PWM_DutyCycleSet(timer_ctrl_t * const p_ctrl, uint32_t const duty_cycle_counts, uint32_t const pin);
fsp_err_t R_TAU_PWM_InfoGet(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info);
fsp_err_t R_TAU_PWM_StatusGet(timer_ctrl_t * const p_ctrl, timer_status_t * const p_status);
fsp_err_t R_TAU_PWM_CallbackSet(timer_ctrl_t * const          p_api_ctrl,
                                void (                      * p_callback)(timer_callback_args_t *),
                                void const * const            p_context,
                                timer_callback_args_t * const p_callback_memory);
fsp_err_t R_TAU_PWM_Close(timer_ctrl_t * const p_ctrl);

/*******************************************************************************************************************//**
 * @} (end defgroup TAU_PWM)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
