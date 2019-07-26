#ifndef __BOOT_H_
#define __BOOT_H_

#include <stdint.h>

void boot_page_erase_safe(uint32_t addr);
void boot_page_fill_safe(uint32_t addr, uint16_t val);
void boot_page_write_safe(uint32_t addr);
void boot_rww_enable_safe(void);

#endif