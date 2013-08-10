/* $Id: //WIFI_SOC/release/SDK_4_1_0_0/source/linux-2.6.21.x/include/asm-cris/arch-v32/hwregs/reg_rdwr.h#1 $
 *
 * Read/write register macros used by *_defs.h
 */

#ifndef reg_rdwr_h
#define reg_rdwr_h


#define REG_READ(type, addr) *((volatile type *) (addr))

#define REG_WRITE(type, addr, val) \
   do { *((volatile type *) (addr)) = (val); } while(0)

#endif
