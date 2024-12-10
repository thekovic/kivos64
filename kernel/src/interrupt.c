#include "intdef.h"
#include "cop0.h"
#include "mi.h"
#include "vi.h"
#include "system.h"

/** @brief Number of nested disable interrupt calls
 *
 * This represents the number of disable interrupt calls made on the system.
 * If this is set to 0, interrupts are enabled. A number higher than 0 represents
 * that many disable calls that were nested, and consequently the number of interrupt
 * enable calls that need to be made to re-enable interrupts. A negative number means
 * that the interrupt system hasn't been initialized yet.
 */
int __interrupt_depth = -1;

int __interrupt_sr = 0;

/**
 * @brief Initialize the interrupt controller.
 */
bool interrupt_init(void)
{
    // Make sure that we aren't initializing interrupts when they are already enabled.
    if (__interrupt_depth < 0)
    {
        // Clear and mask all interrupts on the system so we start with a clean slate.
        MI_regs->mask = (MI_MASK_CLR_SP | MI_MASK_CLR_SI | MI_MASK_CLR_AI | MI_MASK_CLR_VI | MI_MASK_CLR_PI | MI_MASK_CLR_DP);

        // Set that we are enabled.
        __interrupt_depth = 0;

        // Enable interrupts.
        uint32_t sr = C0_STATUS();
        C0_WRITE_STATUS(sr | C0_STATUS_IE | C0_INTERRUPT_RCP | C0_INTERRUPT_RESET);
        return true;
    }

    return false;
}

void interrupt_disable()
{
    // Don't do anything if we haven't initialized.
    if (__interrupt_depth < 0)
    {
        return;
    }

    if (__interrupt_depth == 0)
    {
        // We must disable the interrupts now.
        uint32_t sr = C0_STATUS();
        C0_WRITE_STATUS(sr & ~C0_STATUS_IE);

        // Save the original SR value away, so that we know if
        // interrupts were enabled and whether to restore them.
        __interrupt_sr = sr;
    }

    // Ensure that we remember nesting levels.
    __interrupt_depth++;
}

void interrupt_enable()
{
    // Don't do anything if we aren't initialized.
    if (__interrupt_depth < 0)
    {
        return;
    }

    // Check that we're not calling enable_interrupts() more times than expected.
    assert(__interrupt_depth > 0, "Unbalanced interrupt_enable() call.");

    // Decrement the nesting level now that we are enabling interrupts.
    __interrupt_depth--;

    if (__interrupt_depth == 0)
    {
        // Restore the interrupt state that was active when interrupts got disabled.
        C0_WRITE_STATUS(C0_STATUS() | (__interrupt_sr & C0_STATUS_IE));
    }
}

void interrupt_set_VI(bool active, uint32_t line)
{
    if (active)
    {
        MI_regs->mask = MI_MASK_SET_VI;
        VI_regs->v_interrupt = line;
    }
    else
    {
        MI_regs->mask = MI_MASK_CLR_VI;
    }
}

void interrupt_handler(void)
{
    // Disable FPU, reset to kernel mode, disable exception flag and disable global exceptions.
    uint32_t sr = C0_STATUS() & ~(C0_STATUS_CU1 | C0_STATUS_KSU | C0_STATUS_EXL | C0_STATUS_IE);
    C0_WRITE_STATUS(sr);

    // Get MI interrupt status to handle.
    uint32_t status = MI_regs->interrupt & MI_regs->mask;

    if (status & MI_INTERRUPT_VI)
    {
        // Clear interrupt.
    	VI_regs->v_current = 4;
    }
}
