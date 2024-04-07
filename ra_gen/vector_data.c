/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = r_icu_isr, /* ICU IRQ6 (External pin interrupt 6) */
            [1] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
            [2] = usbfs_interrupt_handler, /* USBFS INT (USBFS interrupt) */
            [3] = usbfs_resume_handler, /* USBFS RESUME (USBFS resume interrupt) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ6,GROUP0), /* ICU IRQ6 (External pin interrupt 6) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_GPT0_COUNTER_OVERFLOW,GROUP1), /* GPT0 COUNTER OVERFLOW (Overflow) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_USBFS_INT,GROUP2), /* USBFS INT (USBFS interrupt) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_USBFS_RESUME,GROUP3), /* USBFS RESUME (USBFS resume interrupt) */
        };
        #endif
        #endif
