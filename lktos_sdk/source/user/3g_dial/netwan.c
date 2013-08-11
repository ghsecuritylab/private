#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<sys/stat.h>
#include 	<sys/types.h>
#include 	<sys/socket.h>
#include  <sys/ioctl.h>
#include 	<netinet/in.h>
#include 	<fcntl.h>
#include 	<arpa/inet.h>
#include 	<net/if_arp.h>
#include 	<net/if.h>
#include	<net/route.h>
#include 	<dirent.h>
#include 	<unistd.h>
#include 	<string.h>
#include 	<errno.h>
#include 	<time.h>
#include 	<assert.h>
#include 	<netinet/tcp.h> 
#include	<sys/ipc.h>
#include	<sys/sem.h>
#include 	<syslog.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include	"nvram.h"

#define PACKET_SIZE     32
#define MAX_WAIT_TIME   2
#define MAX_NO_PACKETS  5
#define MAX_LEAVE_TIME	3

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
int sockfd=-1,datalen=56;

static int nsend=0,nreceived=0,lostCount=0,ontime=0;
static int upFlag=0;

struct sockaddr_in dest_addr;
pid_t pid;
struct sockaddr_in from;
struct timeval tvrecv;

unsigned short cal_chksum(unsigned short *addr,int len);
int pack(int pack_no);
void send_packet(void);
int unpack(char *buf,int len);
void tv_sub(struct timeval *out,struct timeval *in);

typedef struct proc_s {
	char cmd[16];					/* basename of executable file in call to exec(2) */
	int ruid;						/* real only (sorry) */
	int pid;						/* process id */
	int ppid;						/* pid of parent process */
	char state;						/* single-char code for process state (S=sleeping) */
	unsigned int vmsize;			/* size of process as far as the vm is concerned */
} proc_t;

int DBgetPid(char *progname)
{
	proc_t p;
	DIR *dir;
	FILE *file;
	struct dirent *entry;
	char path[32], sbuf[512];
	char uidName[9], stmp[200];
	int len, i, c, pid=0, ret, shcmd=0;

	dir = opendir("/proc");
	if (!dir)
	{
		return 0;
	}


	while ((entry = readdir(dir)) != NULL) {
		if (!isdigit(*entry->d_name))
			continue;
		sprintf(path, "/proc/%s/status", entry->d_name);
		if ((file2str_ex(path, sbuf, sizeof(sbuf))) != -1) {
			ret = parse_proc_status(sbuf, &p);
			if ( ret == 0 ) break;
		}

		sprintf(path, "/proc/%s/cmdline", entry->d_name);
		file = fopen(path, "r");
		if (file == NULL)
			continue;
		i = 0;

		memset( stmp, 0, sizeof(stmp) );
		shcmd = 0;
		while (((c = getc(file)) != EOF) && (i < 200)) {
			stmp[i] = c;
			if (c == '\0' && i)
			{
				if ( strcmp(stmp,"sh") != 0 )
				{
				  if ( strcmp(stmp,"-c") != 0 )
				  	break;
				  else 
				  	shcmd=2;
				}
				else
				    shcmd=1;
				i=0;
			}
			else
				i++;
		}

		fclose(file);
		
		if (i == 0)
		{
			if ( !strcmp(p.cmd, progname) )
			{
				pid = p.pid;
				break;
			}
		}
		else
		{
			if ( !strcmp(stmp, progname) )
			{
				pid = p.pid;
				break;
			}
			else if ( shcmd && strstr(stmp, progname) )
			{
				pid = p.pid;
				break;
			}
		}
	}
	closedir(dir);
	return pid;
}

int file2str_ex(char *filename, char *ret, int cap)
{
	int fd, num_read;

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if ((num_read = read(fd, ret, cap - 1)) <= 0)
		return -1;
	ret[num_read] = 0;
	close(fd);
	return num_read;
}

int parse_proc_status(char *S, proc_t * P)
{
	char *tmp;

	memset(P->cmd, 0, sizeof P->cmd);
	sscanf(S, "Name:\t%15c", P->cmd);
	tmp = strchr(P->cmd, '\n');
	if (tmp)
		*tmp = '\0';
	tmp = strstr(S, "State");
	sscanf(tmp, "State:\t%c", &P->state);

	P->vmsize = 0;
	tmp = strstr(S, "Pid:");
	if (tmp)
		sscanf(tmp, "Pid:\t%d\n" "PPid:\t%d\n", &P->pid, &P->ppid);
	else
		return 0;

	/* For busybox, ignoring effective, saved, etc. */
	tmp = strstr(S, "Uid:");
	if (tmp)
		sscanf(tmp, "Uid:\t%d", &P->ruid);
	else
		return 0;

	tmp = strstr(S, "VmSize:");
	if (tmp)
		sscanf(tmp, "VmSize:\t%d", &P->vmsize);

	return 1;
}


/*У����㷨*/
unsigned short cal_chksum(unsigned short *addr,int len)
{       int nleft=len;
        int sum=0;
        unsigned short *w=addr;
        unsigned short answer=0;
  
/*��ICMP��ͷ������������2�ֽ�Ϊ��λ�ۼ�����*/
        while(nleft>1)
        {       sum+=*w++;
                nleft-=2;
        }
  /*��ICMP��ͷΪ�������ֽڣ���ʣ�����һ�ֽڡ������һ���ֽ���Ϊһ��2�ֽ����ݵĸ��ֽڣ����2�ֽ����ݵĵ��ֽ�Ϊ0�������ۼ�*/
        if( nleft==1)
        {       *(unsigned char *)(&answer)=*(unsigned char *)w;
                sum+=answer;
        }
        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return answer;
}

/*����ICMP��ͷ*/
int pack(int pack_no)
{       
				int i,packsize;
        struct icmp *icmp;
        struct timeval *tval;

        icmp=(struct icmp*)sendpacket;
        icmp->icmp_type=ICMP_ECHO;
        icmp->icmp_code=0;
        icmp->icmp_cksum=0;
        icmp->icmp_seq=pack_no;
        icmp->icmp_id=pid;
        packsize=8+datalen;
        tval= (struct timeval *)icmp->icmp_data;
        gettimeofday(tval,NULL);    /*��¼����ʱ��*/
        icmp->icmp_cksum=cal_chksum( (unsigned short *)icmp,packsize); /*У���㷨*/
        
        return packsize;
}

/*���ͼ�����ָ��ICMP���ĸ���*/
void send_packet()
{       int packetsize;
				int n,fromlen;
        int errno;

        //signal(SIGALRM,statistics);
        fromlen=sizeof(from);

        while( nsend<MAX_NO_PACKETS)
        {       
        				nsend++;
                packetsize=pack(nsend); /*����ICMP��ͷ*/
                
                
                if( sendto(sockfd,sendpacket,packetsize,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr) )<0  )
                {       
                		perror("sendto error");
                    continue;
                }
				 				
				 				
				 				if( (n=recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,&fromlen)) <0)
                {       
                		if(errno==EINTR)
                			continue;
                        
                    perror("recvfrom error");
                    	continue;
                }
                gettimeofday(&tvrecv,NULL);
                
                
                if(unpack(recvpacket,n)==-1)
									continue;
									
                
                nreceived++;
                sleep(1); /*ÿ��һ�뷢��һ��ICMP����*/
        }
}

/*��ȥICMP��ͷ*/
int unpack(char *buf,int len)
{       
				int i,iphdrlen;
        struct ip *ip;
        struct icmp *icmp;
        struct timeval *tvsend;
        double rtt;

        ip=(struct ip *)buf;
        iphdrlen=ip->ip_hl<<2;    /*��ip��ͷ����,��ip��ͷ�ĳ��ȱ�־��4*/
        icmp=(struct icmp *)(buf+iphdrlen);  /*Խ��ip��ͷ,ָ��ICMP��ͷ*/
        len-=iphdrlen;            /*ICMP��ͷ��ICMP���ݱ����ܳ���*/
        if( len<8)                /*С��ICMP��ͷ�����򲻺���*/
        {       
        				printf("ICMP packets\'s length is less than 8\n");
                return -1;
        }
        /*ȷ�������յ����������ĵ�ICMP�Ļ�Ӧ*/
        if( (icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id==pid) )
        {       
        				tvsend=(struct timeval *)icmp->icmp_data;
                tv_sub(&tvrecv,tvsend);  /*���պͷ��͵�ʱ���*/
                rtt=tvrecv.tv_sec*1000+tvrecv.tv_usec/1000;  /*�Ժ���Ϊ��λ����rtt*/
                /*��ʾ�����Ϣ*/
                //printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",len,inet_ntoa(from.sin_addr),icmp->icmp_seq,ip->ip_ttl,rtt);  
			    			return 1;
        }
        else
	    	{
					return -1;
				}
}

/*����timeval�ṹ���*/
void tv_sub(struct timeval *out,struct timeval *in)
{       if( (out->tv_usec-=in->tv_usec)<0)
        {       --out->tv_sec;
                out->tv_usec+=1000000;
        }
        out->tv_sec-=in->tv_sec;
}

int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

/*
	-1 : fail
	0  : successful
*/
int ping_domain()
{
			struct hostent *host;
		  unsigned long inaddr=0l;
		  struct timeval waittime;
		  int size=50*1024;

			/*����ʹ��ICMP��ԭʼ�׽���,�����׽���ֻ��root��������*/
			if(sockfd < 0)
			{
						if( (sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP) )<0)
						{       
								perror("socket error");
								return -1;
						}
						
						printf("\n2222222\n");
	
						setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size) );
						
						if(fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) 
						{
							perror("fcntl");
							close(sockfd);
							return -1;
						}
						
						printf("\n333333\n");
						
						waittime.tv_sec = MAX_WAIT_TIME;
						waittime.tv_usec = 0;
			
						//����ʱ��
						setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&waittime,sizeof(int));
						
						//����ʱ��
						setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&waittime,sizeof(int));
			
						bzero(&dest_addr,sizeof(dest_addr));
						dest_addr.sin_family=AF_INET;
			}
			
			/*
			if((host=gethostbyname("www.baidu.com") )==NULL) 
			{     
						return -1;						
			}
			else
			{
						printf("\n66666\n");
						printf("\n%s\n",inet_ntoa(*(struct in_addr*)host->h_addr));
						memcpy( (char *)&dest_addr.sin_addr,host->h_addr,host->h_length);
			}
			*/
			memcpy( (char *)&dest_addr.sin_addr,"221.179.180.82",strlen("221.179.180.82"));
			printf("\n7777\n");
			
			while(1)
			{
					send_packet();  /*���ͽ�������ICMP����*/
					printf("nreceived = %d\n", nreceived);
					if(nreceived > 0)
					{
							nsend = 0;
							ontime = 0;	
							nreceived = 0;
							return 0;
					 }
					 else
					 {	
							sleep(1);
							ontime += 1;
							if(ontime >= MAX_LEAVE_TIME)
							{
								nsend=0;
								ontime = 0;	
								nreceived=0;
								return -1;
							}
							continue;	
					 }		
			}
}

void start_3g_dial()
{
			char	cmd[64], *wanConnType=NULL, *networkCode=NULL;
			
			wanConnType = nvram_bufget(RT2860_NVRAM,"wanConnType");
			networkCode = nvram_bufget(RT2860_NVRAM,"dial3gnetworkCode");
			
			if(DBgetPid("udhcpc"))
			{
				system("killall -9 udhcpc");
				system("ifconfig eth2.2 down");
			}				
			
			if(DBgetPid("checkat"))
				system("killall -9 checkat");
										
			if(DBgetPid("3gdial"))
				system("killall -9 3gdial");
										
			if(DBgetPid("pppd"))
				system("killall -15 pppd");
										
			memset(cmd, '\0', sizeof(cmd));
			if(!wanConnType||atoi(wanConnType)==0)
			{
					if(networkCode && strlen(networkCode))
					{											
							sprintf(cmd, "3gdial 0 %s", networkCode);
							system(cmd);
					}
					else
							system("3gdial 0");
			}			
			else
			{
					if(networkCode && strlen(networkCode))
					{											
							sprintf(cmd, "3gdial 1 %s", networkCode);
							system(cmd);
					}
					else
							system("3gdial 1");
			}
			
			sleep(30);
}

/*  
����˵����
1.�����ж�eth2.2�ӿ��Ƿ��ȡ��IP�����û�л�ȡ������sleep(5)�����ѭ��������ȡ��IP������뵽��2��
2.����3G�����Ƿ�ɹ����ȶϿ�ppp0��Ȼ��ping www.baidu.com,�����pingͨ����kill 3gdial, pppd, checkat�Ƚ��̣���eth2.2��ΪWAN�ڣ��������pingͨ��������3��
3.�������6�ζ�����pingͨ������Ϊeth2.2���ܻ�ȡIP�����޷�������������3gdial,��3G������ΪWAN��
*/
int main(char argc, char **argv)
{
			pid_t pid;
			char 					wan_ip[16], cmd[64];
			static int		preflag = 0, nowflag = 0, ethdownflag = 0, ethdownnum = 0, first = 0;
			
			if ( daemon(1,1) < 0 )
			{
		       perror("daemon()");
		       exit(-1);
			}
			
			/* ����rootȨ��,���õ�ǰ�û�Ȩ��*/
			setuid(getuid());
			
			pid=getpid();
			
			while(1)
			{
			done:
					memset(wan_ip, '\0', sizeof(wan_ip));
					getIfIp("eth2.2", wan_ip);
					
					if(!ethdownflag)
					{
							if(wan_ip[0] != '\0') //get wan ip
							{
									if(nowflag == 0)
									{
										if(DBgetPid("checkat"))
											system("killall -9 checkat");
										
										if(DBgetPid("3gdial"))
											system("killall -9 3gdial");
										
										if(DBgetPid("pppd"))
											system("killall -15 pppd");

										sleep(2);
										
										if( ! first )
										{
												first = 1;
												system("killall -9 udhcpc");
												sleep(3);
												system("udhcpc -i eth2.2 -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid &");
												sleep(5);
										}										
									}								
											
									printf("\n11111111\n");
									while(1)
									{
											if(ping_domain() == -1)
											{
												nowflag = 0;
												ethdownnum ++;
												if(ethdownnum >= 6)
												{
														ethdownflag = 1;
														ethdownnum  = 0;
														start_3g_dial();
														goto done;
												}
												sleep(2);
												continue;											
											}
											else
											{
												nowflag = 1;
												break;
											}												
									}									
							}
							else
								nowflag = 0;
								
							if(preflag != nowflag)
							{
									if(nowflag == 1)
									{
										if(DBgetPid("checkat"))
											system("killall -9 checkat");
										
										if(DBgetPid("3gdial"))
											system("killall -9 3gdial");
										
										if(DBgetPid("pppd"))
											system("killall -15 pppd");
									}
									else
									{
										start_3g_dial();
									}
																		
									preflag = nowflag ;
							}	
					}					
					sleep(5);	
			}
}