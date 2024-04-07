/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (4)
#endif
/* ISR prototypes */
void r_icu_isr(void);
void gpt_counter_overflow_isr(void);
void usbfs_interrupt_handler(void);
void usbfs_resume_handler(void);

/* Vector table allocations */
#define VECTOR_NUMBER_ICU_IRQ6 ((IRQn_Type) 0) /* ICU IRQ6 (External pin interrupt 6) */
#define ICU_IRQ6_IRQn          ((IRQn_Type) 0) /* ICU IRQ6 (External pin interrupt 6) */
#define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type) 1) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define GPT0_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 1) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_USBFS_INT ((IRQn_Type) 2) /* USBFS INT (USBFS interrupt) */
#define USBFS_INT_IRQn          ((IRQn_Type) 2) /* USBFS INT (USBFS interrupt) */
#define VECTOR_NUMBER_USBFS_RESUME ((IRQn_Type) 3) /* USBFS RESUME (USBFS resume interrupt) */
#define USBFS_RESUME_IRQn          ((IRQn_Type) 3) /* USBFS RESUME (USBFS resume interrupt) */
#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
