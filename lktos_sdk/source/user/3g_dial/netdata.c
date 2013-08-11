//////////////////////////////////////////////////////////////////////////     
/// COPYRIGHT NOTICE     
 // Copyright (c) 2009, ���пƼ���ѧticktick Group     
/// All rights reserved.      
///      
/// @file    ortpSend.c       
/// @brief   ortpSend�Ĳ���     
///     
/// ���ļ�ʾ��ʹ��ortp�����rtp���ݰ��ķ���    
///      
/// @version 1.0        
/// @author  tickTick   
/// @date    2010/07/07      
/// @E-mail  lujun.hust@gmail.com     
///     
/// �޶�˵���������ļ�    
//////////////////////////////////////////////////////////////////////////      
 
#include <ortp/ortp.h>  
#include <signal.h>  
#include <stdlib.h>  
 
#ifndef _WIN32   
#include <sys/types.h>  
#include <sys/time.h>  
#include <stdio.h>  
#endif

#include "pcm_ctrl.h"

#define cprintf(fmt, args...) do { \
	FILE *fp = fopen("/dev/ttyS1", "w"); \
	if (fp) { \
		fprintf(fp, fmt, ## args); \
		fclose(fp); \
	} \
} while (0)

typedef struct tagRtp_Info
{
	char addr[32];
	char port[16];
}Rtp_Info_S;
 
 
// ʱ�������
#define TIME_STAMP_INC 	160
#define BYTES_PER_COUNT	256
 
// ʱ���   
uint32_t g_user_ts;  
 
/**  ��ʼ��     
 *       
 *   ��Ҫ���ڶ�ortp�Լ������������г�ʼ��    
 *   @param:  char * ipStr Ŀ�Ķ�IP��ַ������     
 *   @param:  iint port Ŀ�Ķ�RTP�����˿�     
 *   @return:  RtpSession * ����ָ��RtpSession�����ָ��,���ΪNULL�����ʼ��ʧ��     
 *   @note:       
 */   
RtpSession * rtpInit(char * ipStr,int port)  
{  
    // Rtp�Ự����    
    RtpSession *session;  
    char *ssrc;  
      
    // ʱ�����ʼ��    
    g_user_ts = 0;  
 
    // ortp��һЩ������ʼ������   
    ortp_init();  
    ortp_scheduler_init();  
    // �����µ�rtp�Ự����  
    session=rtp_session_new(RTP_SESSION_SENDONLY);    
      
    rtp_session_set_scheduling_mode(session,1);  
    rtp_session_set_blocking_mode(session,1);  
    // ����Զ��RTP�ͻ��˵ĵ�IP�ͼ����˿ڣ�����rtp���ݰ��ķ���Ŀ�ĵ�ַ��   
    rtp_session_set_remote_addr(session,ipStr,port);  
    // ���ø�������    
    rtp_session_set_payload_type(session,0);  
      
    // ��ȡͬ��Դ��ʶ    
    ssrc=getenv("SSRC");  
    if (ssrc!=NULL)   
    {  
        printf("using SSRC=%i.\n",atoi(ssrc));  
        rtp_session_set_ssrc(session,atoi(ssrc));  
    }  
      
    return session;  
 
}  
 
/**  ����rtp���ݰ�     
 *       
 *   ��Ҫ���ڷ���rtp���ݰ�     
 *   @param:  RtpSession *session RTP�Ự�����ָ��     
 *   @param:  const char *buffer Ҫ���͵����ݵĻ�������ַ      
 *   @param: int len Ҫ���͵����ݳ���     
 *   @return:  int ʵ�ʷ��͵����ݰ���Ŀ     
 *   @note:     ���Ҫ���͵����ݰ����ȴ���BYTES_PER_COUNT���������ڲ�����зְ�����     
 */ 
int rtpSend(RtpSession *session,const char *buffer, int len)  
{  
    int curOffset = 0;  
    int sendBytes = 0;  
    int clockslide=500;   
    // ���Ͱ��ĸ���  
    int sendCount = 0;  
 
    ortp_message("send data len %i\n ",len);  
 
    // �Ƿ�ȫ���������    
    while(curOffset < len )  
    {  
        // �����Ҫ���͵����ݳ���С�ڵ�����ֵ����ֱ�ӷ���  
        if( len <= BYTES_PER_COUNT )  
        {  
            sendBytes = len;  
        }  
        else 
        {  
            // �����ǰƫ�� + ��ֵ С�ڵ��� �ܳ��ȣ�������ֵ��С������  
            if( curOffset + BYTES_PER_COUNT <= len )  
            {  
                sendBytes = BYTES_PER_COUNT;  
            }  
            // ����ͷ���ʣ�೤�ȵ�����  
            else 
            {  
                sendBytes = len - curOffset;  
            }  
        }  
          
        ortp_message("send data bytes %i\n ",sendBytes);  
          
        rtp_session_send_with_ts(session,(char *)(buffer+curOffset),sendBytes,g_user_ts);  
          
        // �ۼ�  
        sendCount ++;  
        curOffset += sendBytes;                   
        g_user_ts += TIME_STAMP_INC;  
      
        // ����һ�����ݰ�������һ��  
        if (sendCount%10==0)   
        {  
            usleep(20000);  
        }     
    }  
    return 0;  
}  
 
/**  ����ortp�ķ��ͣ��ͷ���Դ     
 *      
 *   @param:  RtpSession *session RTP�Ự�����ָ��     
 *   @return:  0��ʾ�ɹ�     
 *   @note:         
 */ 
int rtpExit(RtpSession *session)  
{  
    g_user_ts = 0;  
      
    rtp_session_destroy(session);  
    ortp_exit();  
    ortp_global_stats_display();  
 
    return 0;  
}  
 
// �����������в���  
int main(int argc, char **argv)  
{  
    // �����͵����ݻ�����  
    char pBuffer[2048];
    char *rtp_addr, *rtp_port;
    int  pcm_fd;      
    RtpSession * pRtpSession = NULL;
    FILE *fp_pcm, *fp_rtp;
    Rtp_Info_S Rtp_Info;
    
    fp_rtp = fopen("/var/rtp", "r"); 	
		if(fp_rtp < 0) 
		{
			printf("fopen rtp failed (%d)...exit\n",fp_rtp);
			return -1;
		}
		fread(&Rtp_Info, sizeof(Rtp_Info_S), 1, fp_rtp);
		fclose(fp_rtp);
		
		printf("\naddr = %s, port = %s\n\n\n",Rtp_Info.addr, Rtp_Info.port);
    
    rtp_addr = Rtp_Info.addr;
    rtp_port = Rtp_Info.port;
    
    pcm_fd = open("/dev/pcm0", O_RDWR); 	
		if(pcm_fd < 0) 
		{
			printf("open pcm driver failed (%d)...exit\n",pcm_fd);
			return -1;
		}
		
		{
				long param[2];
				param[0] = 0; ///ch
				param[1] = 1; /// for ulaw
				ioctl(pcm_fd, PCM_SET_CODEC_TYPE, param);
				ioctl(pcm_fd, PCM_START, 1);				
			}

    fp_pcm = fopen("/var/record.pcm","wb");
		if(fp_pcm==NULL)
		{
			printf("open pcm file failed..exit\n");
			return -1;
		}
    
    memset(pBuffer, 0, sizeof(pBuffer));
    
    pRtpSession = rtpInit(rtp_addr,atoi(rtp_port));  
    if(pRtpSession==NULL)  
    {  
        printf("error rtpInit");  
        return 0;  
    }  
      
    // ѭ������  
    while(1)  
    {  
    		ioctl(pcm_fd, PCM_GETDATA, pBuffer+PCM_FIFO_SIZE);
    		
    		fwrite(pBuffer+PCM_FIFO_SIZE, 1, PCM_FIFO_SIZE, fp_pcm);
    		
        if( rtpSend(pRtpSession,pBuffer+PCM_FIFO_SIZE,PCM_FIFO_SIZE) != 0)  
        {  
            printf("error rtpInit");  
            break;  
        }  
    }  
      
    // �˳�  
    rtpExit(pRtpSession);  
      
    return 0;  
} 