/* $Id: //WIFI_SOC/release/SDK_4_1_0_0/source/linux-2.6.21.x/drivers/isdn/capi/capifs.h#1 $
 * 
 * Copyright 2000 by Carsten Paeth <calle@calle.de>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 */

void capifs_new_ncci(unsigned int num, dev_t device);
void capifs_free_ncci(unsigned int num);
