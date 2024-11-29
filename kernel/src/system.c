#include "system.h"
#include "cop0.h"
#include "mi.h"

int __boot_memsize;
int __boot_random_seed;
int __boot_tvtype;
int __boot_resettype;

volatile const MI_registers_t* MI_regs = (MI_registers_t*) MI_REG_BASE;

void cop0_status_reset()
{
    C0_WRITE_STATUS(C0_STATUS_CU1 | C0_STATUS_SR | C0_STATUS_FR | C0_STATUS_KX | C0_STATUS_SX | C0_STATUS_UX);
}
