#ifndef __FLASH_H_
#define __FLASH_H_

#ifndef SPM_PAGESIZE
#define SPM_PAGESIZE 128
#endif

void program_flash(uint32_t, unsigned char *);

#endif