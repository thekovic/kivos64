#ifndef KIVOS64_INTERRUPT_H
#define KIVOS64_INTERRUPT_H

void interrupt_disable(void);

void interrupt_enable(void);

void interrupt_set_AI(bool active);

void interrupt_set_SI(bool active);

void interrupt_set_VI(bool active, uint32_t line);

#endif
