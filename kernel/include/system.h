#ifndef KIVOS64_SYSTEM_H
#define KIVOS64_SYSTEM_H

// Memory size as detected by IPL3.
extern int __boot_memsize;
// Random seed provided by IPL3.
extern int __boot_random_seed;
// TV type as detected by IPL3.
extern int __boot_tvtype;
// Reset type as detected by IPL3.
extern int __boot_resettype;

#endif
