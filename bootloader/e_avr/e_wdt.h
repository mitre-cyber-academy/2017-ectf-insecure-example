#ifndef __WDT_H_
#define __WDT_H_

#define WDTO_2S 2

void wdt_reset(void);
void wdt_enable(int);
void wdt_disable(void);

#endif

