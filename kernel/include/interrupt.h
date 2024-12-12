#ifndef KIVOS64_INTERRUPT_H
#define KIVOS64_INTERRUPT_H

void interrupt_disable();

void interrupt_enable();

void interrupt_set_VI(bool active, uint32_t line);

void interrupt_set_SI(bool active);

#endif
