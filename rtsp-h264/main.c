#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>

#include "rtspservice.h"
#include "rtputils.h"
#include "ringfifo.h"

// 在别的代码里有实现
extern void *sample_venc_1080p_classic(void *p);
extern void SAMPLE_VENC_HandleSig(int signo);
struct ringbuf ringinfo;
void PrefsInit();
void RTP_port_pool_init(int port);
void EventLoop(int sock_fd);
void yoloy5_init(void);
void yoloy5_run(void);
void yoloy5_exit(void);
int stop=1;
static void *yoloy5(void *args)
{
	while(stop)
	{
		yoloy5_run();
	}
        yoloy5_exit();
	return NULL;
}

/**
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
	int s32MainFd,temp;
	struct timespec ts = { 2, 0 };
	pthread_t id;
	pthread_t id2;
	// (信号值 2）：当用户按下 Ctrl+C 时触发，通常用于请求进程中断。
	signal(SIGINT, SAMPLE_VENC_HandleSig);
	// SIGTERM（信号值 15）：请求进程终止（如通过 kill 命令发送）。
	signal(SIGTERM, SAMPLE_VENC_HandleSig);

	ringmalloc();

	pthread_create(&id,NULL,sample_venc_1080p_classic,NULL);

    printf("--------------------------------筱田広亚郎--------------------------------------------------start\n");
	printf("RTSP server START\n");
	PrefsInit();
	printf("listen for client connecting...\n");

	s32MainFd = tcp_listen(SERVER_RTSP_PORT_DEFAULT);

	if (ScheduleInit() == ERR_FATAL)
	{
		fprintf(stderr,"Fatal: Can't start scheduler %s, %i \nServer is aborting.\n", __FILE__, __LINE__);
		return 0;
	}
	RTP_port_pool_init(RTP_DEFAULT_PORT);
	sleep(3);




	printf("start yoloy5\n");
	yoloy5_init();
	pthread_create(&id2,NULL,yoloy5,NULL);
    printf("s32MainFd is : %d\n",s32MainFd);
	while (1)
	{
		nanosleep(&ts, NULL);
		EventLoop(s32MainFd);
	}
	sleep(1);
	ringfree();
	printf("The Server quit!\n");
	return 0;
}



