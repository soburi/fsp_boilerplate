/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED  AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 05.10.2020 1.00        First Release.
 *         : 02.12.2020 1.01        Added new functions such as the Brainpool curve.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_sce_if.h"
#include "hw_sce_ra_private.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

fsp_err_t HW_SCE_Aes256CcmDecryptFinalSub(const uint32_t *InData_Text, const uint32_t *InData_TextLen, const uint32_t *InData_MAC, const uint32_t *InData_MACLength, uint32_t *OutData_Text)
{
    uint32_t iLoop    = 0U;
    uint32_t iLoop1   = 0U;
    uint32_t iLoop2   = 0U;
    int32_t  jLoop    = 0U;
    uint32_t kLoop    = 0U;
    uint32_t oLoop    = 0U;
    uint32_t oLoop1   = 0U;
    uint32_t oLoop2   = 0U;
    uint32_t dummy    = 0U;
    uint32_t KEY_ADR  = 0U;
    uint32_t OFS_ADR  = 0U;
    uint32_t MAX_CNT2 = 0U;
    (void)iLoop;
    (void)iLoop1;
    (void)iLoop2;
    (void)jLoop;
    (void)kLoop;
    (void)oLoop;
    (void)oLoop1;
    (void)oLoop2;
    (void)dummy;
    (void)KEY_ADR;
    (void)OFS_ADR;
    (void)MAX_CNT2;
    HW_SCE_func001(0xa70b2107U, 0x1b584345U, 0x4125e25bU, 0x955e95b3U);
    SCE->REG_1CH = 0x00400000U;
    SCE->REG_1D0H = 0x00000000U;
    if (1U == (SCE->REG_1CH_b.B22))
    {
        SCE->REG_1CH = 0x002d0000U;
        HW_SCE_func001(0xcf41d2b5U, 0xfb1010c9U, 0x708df706U, 0x22abf5b8U);
        SCE->REG_1CH = 0x00400000U;
        SCE->REG_1D0H = 0x00000000U;
        if (1U == (SCE->REG_1CH_b.B22))
        {
            SCE->REG_00H = 0x00000113U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            HW_SCE_func002(0x3267a8e0U, 0xa81aea60U, 0x31d3670fU, 0xf221a76dU);
        }
        else
        {
            HW_SCE_func002(0x295e6f7dU, 0x5fcaa357U, 0x81c1c79bU, 0x678302b2U);
        }
        HW_SCE_func003(0xb78d9597U, 0xb85fa1adU, 0xafdbc15dU, 0x44553c60U);
        SCE->REG_1BCH = 0x00000040U;
        /* WAIT_LOOP */
        while (0U != SCE->REG_18H_b.B12)
        {
            /* waiting */
        }
        return FSP_ERR_CRYPTO_SCE_FAIL;
    }
    else
    {
        SCE->REG_104H = 0x00000068U;
        SCE->REG_E0H = 0x800100e0U;
        SCE->REG_1D0H = 0x00000000U;
        /* WAIT_LOOP */
        while (1U != SCE->REG_104H_b.B31)
        {
            /* waiting */
        }
        SCE->REG_100H = InData_MACLength[0];
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_ECH = 0x380088e0U;
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_ECH = 0x00000000U;
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_E0H = 0x00000080U;
        SCE->REG_1CH = 0x00260000U;
        SCE->REG_ECH = 0x0000b460U;
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_ECH = 0x00000010U;
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_ECH = 0x34202867U;
        SCE->REG_1D0H = 0x00000000U;
        SCE->REG_E0H = 0x00000080U;
        SCE->REG_1CH = 0x00260000U;
        HW_SCE_func001(0x098cc749U, 0xe3808805U, 0x697846a2U, 0x96f9a032U);
        SCE->REG_1CH = 0x00400000U;
        SCE->REG_1D0H = 0x00000000U;
        if (1U == (SCE->REG_1CH_b.B22))
        {
            HW_SCE_func003(0x3a78cafeU, 0xf31528d0U, 0xaae53fd9U, 0x87af9b37U);
            SCE->REG_1BCH = 0x00000040U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_18H_b.B12)
            {
                /* waiting */
            }
            return FSP_ERR_CRYPTO_SCE_FAIL;
        }
        else
        {
            SCE->REG_104H = 0x00000068U;
            SCE->REG_E0H = 0x80010000U;
            SCE->REG_1D0H = 0x00000000U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = InData_TextLen[0];
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_ECH = 0x00008c00U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_ECH = 0x0000000fU;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_ECH = 0x38008800U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_ECH = 0x00000000U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_E0H = 0x00000080U;
            SCE->REG_1CH = 0x00A60000U;
            HW_SCE_func001(0xf76adefbU, 0x19a0b516U, 0xbf0475e1U, 0x56880b94U);
            SCE->REG_1CH = 0x00400000U;
            SCE->REG_1D0H = 0x00000000U;
            if (1U == (SCE->REG_1CH_b.B22))
            {
                HW_SCE_func001(0x31c7fd38U, 0x8d2d7b40U, 0xf483d8abU, 0x7f9ae0c9U);
                SCE->REG_104H = 0x00000361U;
                SCE->REG_B0H = 0x40000000U;
                SCE->REG_A4H = 0x000086bdU;
                /* WAIT_LOOP */
                while (1U != SCE->REG_104H_b.B31)
                {
                    /* waiting */
                }
                SCE->REG_100H = InData_Text[0];
                SCE->REG_100H = InData_Text[1];
                SCE->REG_100H = InData_Text[2];
                SCE->REG_100H = InData_Text[3];
                SCE->REG_ECH = 0x00000821U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_E0H = 0x80840001U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_00H = 0x00008113U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_00H_b.B25)
                {
                    /* waiting */
                }
                SCE->REG_1CH = 0x00001800U;
                SCE->REG_ECH = 0x00000863U;
                SCE->REG_1D0H = 0x00000000U;
                for (iLoop = 0; iLoop < 16; iLoop = iLoop+1)
                {
                    SCE->REG_ECH = 0x3c002860U;
                    SCE->REG_1D0H = 0x00000000U;
                    SCE->REG_ECH = 0x12003c23U;
                    SCE->REG_1D0H = 0x00000000U;
                    SCE->REG_ECH = 0x00002c60U;
                    SCE->REG_1D0H = 0x00000000U;
                }
                SCE->REG_ECH = 0x00000821U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_E0H = 0x81840001U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_04H = 0x00000612U;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[0] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[1] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[2] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[3] = SCE->REG_100H;
                SCE->REG_B0H = 0x40000000U;
                SCE->REG_A4H = 0x00008c5dU;
                SCE->REG_E0H = 0x81840001U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_00H = 0x00001813U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_00H_b.B25)
                {
                    /* waiting */
                }
                SCE->REG_1CH = 0x00001800U;
                HW_SCE_func002(0x32b3a18fU, 0xe36f31d8U, 0x133dca0cU, 0xdd77eadfU);
            }
            else
            {
                HW_SCE_func002(0x21070664U, 0xf0d841deU, 0x6e301872U, 0x6f4e5340U);
            }
            SCE->REG_104H = 0x00000051U;
            SCE->REG_A4H = 0x00000ec4U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = change_endian_long(0x00000000U);
            SCE->REG_B0H = 0x40000000U;
            SCE->REG_A4H = 0x010086bdU;
            SCE->REG_00H = 0x00001113U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_ECH = 0x00000821U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_E0H = 0x80840001U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_00H = 0x00008113U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_ECH = 0x00000863U;
            SCE->REG_1D0H = 0x00000000U;
            for (iLoop = 0; iLoop < 16; iLoop = iLoop+1)
            {
                SCE->REG_ECH = 0x3c002867U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_ECH = 0x12003c23U;
                SCE->REG_1D0H = 0x00000000U;
                SCE->REG_ECH = 0x00002c60U;
                SCE->REG_1D0H = 0x00000000U;
            }
            SCE->REG_104H = 0x00000361U;
            SCE->REG_A4H = 0x00050805U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = InData_MAC[0];
            SCE->REG_100H = InData_MAC[1];
            SCE->REG_100H = InData_MAC[2];
            SCE->REG_100H = InData_MAC[3];
            SCE->REG_A4H = 0x00900c45U;
            SCE->REG_ECH = 0x00000821U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_E0H = 0x81840001U;
            SCE->REG_1D0H = 0x00000000U;
            SCE->REG_00H = 0x00001813U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_A4H = 0x00000000U;
            SCE->REG_1D0H = 0x00000000U;
            HW_SCE_func001(0x369c85c1U, 0x1b74ddc3U, 0xdfa9a261U, 0xec2f7e00U);
            SCE->REG_1CH = 0x00400000U;
            SCE->REG_1D0H = 0x00000000U;
            if (1U == (SCE->REG_1CH_b.B22))
            {
                HW_SCE_func003(0x282747faU, 0x78607218U, 0xd4b6909eU, 0xc9b6a68bU);
                SCE->REG_1BCH = 0x00000040U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_18H_b.B12)
                {
                    /* waiting */
                }
                return FSP_ERR_CRYPTO_SCE_FAIL;
            }
            else
            {
                HW_SCE_func003(0xe1d4dc16U, 0x646d60e1U, 0x62cb1049U, 0x63df74cbU);
                SCE->REG_1BCH = 0x00000040U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_18H_b.B12)
                {
                    /* waiting */
                }
                return FSP_SUCCESS;
            }
        }
    }
}

/***********************************************************************************************************************
End of function ./input_dir/HW_SCE_Sec_200408/200408/RA4M1/Cryptographic/HW_SCE_p81.prc
***********************************************************************************************************************/
