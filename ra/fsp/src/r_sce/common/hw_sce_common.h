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

/*******************************************************************************************************************//**
 * @addtogroup SCE
 * @{
 ***********************************************************************************************************************/

#ifndef HW_SCE_COMMON_H
#define HW_SCE_COMMON_H

/**********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "bsp_api.h"
#include "r_sce_if.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define R_SCE_BITS_TO_BYTES(x)     (((x) + 7U) >> 3U)
#define R_SCE_BYTES_TO_BITS(x)     ((x) << 3U)
#define R_SCE_WORDS_TO_BYTES(x)    ((x) << 2U)
#define R_SCE_BYTES_TO_WORDS(x)    (((x) + 3U) >> 2U)

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 ***********************************************************************************************************************/
uint32_t change_endian_long(uint32_t data);

/*******************************************************************************************************************//**
 *
 **********************************************************************************************************************/
__STATIC_INLINE void HW_SCE_PowerOn (void)
{
#if BSP_FEATURE_TRNG_HAS_MODULE_STOP

    // RA2 MCU series has separate power control for RNG
    R_MSTP->MSTPCRC_b.MSTPC28 = 0;
#endif

    // power on the SCE module
    R_MSTP->MSTPCRC_b.MSTPC31 = 0;
}

__STATIC_INLINE void HW_SCE_PowerOff (void)
{
#if BSP_FEATURE_TRNG_HAS_MODULE_STOP

    // RA2 MCU series has separate power control for RNG
    R_MSTP->MSTPCRC_b.MSTPC28 = 1;
#endif

    // power off the SCE module
    R_MSTP->MSTPCRC_b.MSTPC31 = 1;
}

#endif                                 /* HW_SCE_COMMON_H */

/*******************************************************************************************************************//**
 * @}
 **********************************************************************************************************************/
