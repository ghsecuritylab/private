/*
 * hotplug_Storage.c - hotplug storage dispatch routine
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * 
 */

#include <sys/stat.h>  //leng minglang 2007-10-13 add 
#include <unistd.h>
#include "hotplug.h"

int hotplug_storage_mountpoint(void)
{

/* for mountdev */

	FILE *fp = NULL, *fp_test = NULL;
	char *devpath = NULL;
	char *p = NULL;
	char tmp;
	int i;

	if(!(devpath = getenv("DEVPATH")))
		return EINVAL;	

	fp = fopen(MOUNTPOINT,"a+");
	if(!fp)
		return EINVAL;
		
	p = strrchr(devpath,'/');
	p++;
	if(!p)
		return EINVAL;

/* just need 'a' '1', 'a' '2', 'b' '1' , 'b' '2' ...from sda1, sda2, sdb1,sdb2......*/
	for(i=0; i<2; i++) {
		if(p[i+2] == '\0')
			tmp = '1';
		else
			tmp = p[i+2];
		fputc(tmp,fp);
		}
	fclose(fp);
	return 0;


}


int hotplug_storage_mount(void)
{
	FILE *fp = NULL,*fp_test = NULL;
	char *action = NULL,*devpath = NULL, *modalis = NULL, *product = NULL, *subsystem = NULL;
	char tmp[2];
	char cmd[80];
	int i,disk,partition,retv;

	action = getenv("ACTION");	
	if(!action)
		return EINVAL;
	fp = fopen(MOUNTPOINT,"r+");
	if(!fp)
		return EINVAL;		

	if(!strcmp(action,"add")) {
		while(!feof(fp)) {
			for(i=0; i<2; i++) {
				tmp[i] = fgetc(fp);
				}			

			if(!isalpha(tmp[0])) 
			{				
				if(isdigit(tmp[0]))
					InitDeviceInfo();          
				
				fclose(fp);
				//zhubo add 2010.01.09
				//unlink(MOUNTPOINT);			
				return 0;				
			}

			disk = tmp[0] - 'a' + 1; //usb1, usb2 .......
			partition = tmp[1] - '0'; //usb1_1, usb1_2....

			/* create mounpoint under /mnt : usb1_1, usb1_2, usb2_1.......*/
			sprintf(cmd, "mkdir -p /mnt/usb%d_%d", disk, partition);
			bcmSystem(cmd);
			if(tmp[1] == '1') {
				sprintf(cmd, "mount -t vfat /dev/sd%c /mnt/usb%d_%d",tmp[0],disk,partition);
				retv = bcmSystem(cmd);
				sprintf(cmd, "mount -t vfat /dev/sd%c%d /mnt/usb%d_%d",tmp[0],partition,disk,partition);
				bcmSystem(cmd);
				}
			else {
				sprintf(cmd, "mount -t vfat /dev/sd%c%d /mnt/usb%d_%d",tmp[0],partition,disk,partition);
				retv = bcmSystem(cmd);
				if(retv != 0){
					sprintf(cmd, "umount /mnt/usb%d_%d",disk,partition);
					bcmSystem(cmd);
					sprintf(cmd, "rm -rf /mnt/usb%d_%d&", disk, partition);
					bcmSystem(cmd);
					}
					}
				}
		fclose(fp);
		//sprintf(cmd, "rm -rf %s",MOUNTPOINT);
		//bcmSystem(cmd);

		//zhubo add 2010.01.09
		//unlink(MOUNTPOINT);
		}

		return 0;
}
//leng minglang mod 2007-10-13 for usb completeness uninstall
int hotplug_storage_umount(void)
{
	FILE *fp = NULL;
	char *action = NULL,*devpath = NULL;
	char tmp[2];
	char cmd[80];
	int i,disk,partition;
	int ret;
	struct stat stbuf;

	action = getenv("ACTION");
	if(!action)
		return EINVAL;

	fp = fopen(MOUNTPOINT,"r+");
	if(!fp)
		return EINVAL;
		
	if(!strcmp(action,"remove")) {
    unDeviceInfo();
				
		while(!feof(fp)) {
			for(i=0; i<2; i++) {
				tmp[i] = fgetc(fp);
				}

			if(!isalpha(tmp[0])) {
					fclose(fp);

					//zhubo add 2010.01.09
					//unlink(MOUNTPOINT);
					return 0;
			}

			disk = tmp[0] - 'a' + 1;    //usb1, usb2 .......
			partition = tmp[1] - '0';   //usb1_1, usb1_2....

			sprintf(cmd, "/mnt/usb%d_%d",disk,partition);
			if (0 != stat(cmd, &stbuf))				
				continue;
			
			sprintf(cmd, "fuser -mk /mnt/usb%d_%d",disk,partition);
			bcmSystem(cmd);
			sprintf(cmd, "umount /mnt/usb%d_%d",disk,partition);
			ret = (bcmSystem(cmd));

			//fprintf(fptmp, "L%d: %s, cmd ret:%d\n", __LINE__, cmd, ret);
			//fflush(fptmp);

			if (ret){	//(bcmSystem(cmd)){

  				sprintf(cmd, "umount -o ro /mnt/usb%d_%d",disk,partition);

				//fprintf(fptmp, "L%d: %s, cmd ret:%d\n", __LINE__, cmd, ret);
				
				bcmSystem(cmd);
				int i=0;
				while(i < 60){
	   				i++;
   					sleep(1);
					sprintf(cmd, "umount /mnt/usb%d_%d",disk,partition);
					//fprintf(fptmp, "%s %d  return :%d\n", cmd, i,ret);
					ret =bcmSystem(cmd);
    				if (0 == ret)
           			 	break; //exit loop
  				}
			}
			//bcmSystem(cmd);
			sprintf(cmd, "rm -rf /mnt/usb%d_%d", disk, partition);
			bcmSystem(cmd);
			}
		fclose(fp);
		//sprintf(cmd, "rm -rf %s",MOUNTPOINT);
		//bcmSystem(cmd);

		//zhubo add 2010.01.09
		//unlink(MOUNTPOINT);


		}
	//leng minglang mod 2007-10-13 end 
	return 0;
}

 