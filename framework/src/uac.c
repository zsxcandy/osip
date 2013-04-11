#if defined(__arc__)
#define LOG_PERROR 1
#include <includes_api.h>
#include <os_cfg_pub.h>
#endif

#if !defined(WIN32) && !defined(_WIN32_WCE)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>
#ifndef OSIP_MONOTHREAD
#include <pthread.h>
#endif
#endif


#include "sip_system.h"
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>
#include "sip_handle_request.h"

#if !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__arc__)
#define _GNU_SOURCE
#include <getopt.h>
#endif

#define PROG_NAME "sipreg"
#define PROG_VER  "1.0"
#define UA_STRING "SipReg v" PROG_VER
#define SYSLOG_FACILITY LOG_DAEMON

#if defined(WIN32) || defined(_WIN32_WCE)
	static void
syslog_wrapper (int a, const char *fmt, ...)
{
	va_list args;

	va_start (args, fmt);
	printf(fmt, args);
	vfprintf (stdout, fmt, args);
	va_end (args);
}

#define LOG_INFO 0
#define LOG_ERR 0
#define LOG_WARNING 0
#define LOG_DEBUG 0

#elif defined(LOG_PERROR)
/* If we can, we use syslog() to emit the debugging messages to stderr. */
#define syslog_wrapper    syslog
#else
#define syslog_wrapper(a,b...) fprintf(stderr,b);fprintf(stderr,"\n")
#endif






	static void *
register_proc (void *arg)
{
}
struct eXosip_t *context_eXosip;
int main (int argc, char *argv[])
{
#ifndef OSIP_MONOTHREAD
	struct osip_thread *register_thread;
#endif

	eXosip_event_t *je;
	osip_message_t *reg = NULL;
	osip_message_t *invite = NULL;
	osip_message_t *ack = NULL;
	osip_message_t *info = NULL;
	osip_message_t *message = NULL;

	context_eXosip = eXosip_malloc ();
	int call_id, dialog_id;
	int i,flag;
	int flag1 = 1;
	int id;

	char *identity = "sip:140@192.168.0.140";
	char *registerer = "sip:192.168.0.133:5060";
	char *source_call = "sip:233@192.168.5.115:5060";
	char *dest_call = "sip:255@192.168.5.118:5060";

	char command;
	char tmp[4096];
	char localip[128];

	printf("r     向服务器注册\r\n");
	printf("c     取消注册\r\n");
	printf("i     发起呼叫请求\r\n");
	printf("h     挂断\r\n");
	printf("q     退出程序\r\n");
	printf("s     执行方法INFO\r\n");
	printf("m     执行方法MESSAGE\r\n");
	//初始化

	i = eXosip_init (context_eXosip);
	if (i != 0)
	{
		printf ("Couldn't initialize eXosip!/n");
		return -1;
	}
	else
	{
		printf ("eXosip_init successfully!/n");
	}
	register_thread = osip_thread_create (20000, register_proc, NULL);
	i = eXosip_listen_addr (context_eXosip, IPPROTO_UDP, "192.168.5.115", 5060, AF_INET, 0);
	if (i != 0)
	{
		eXosip_quit (context_eXosip);
		fprintf (stderr, "Couldn't initialize transport layer!/n");
		return -1;
	}
	flag = 1;

#if 1
	while (flag)
	{
		printf ("please input the comand:\n");

		scanf ("%c", &command);
		getchar ();

		switch (command)
		{
			case 'r':
				printf ("This modal isn't commpleted!/n");
				break;
			case 'i':/* INVITE */
				{
					i = eXosip_call_build_initial_invite (context_eXosip,&invite, dest_call, source_call, NULL, "This si a call for a conversation");
					if (i != 0)
					{
						printf ("Intial INVITE failed!/n");
						break;
					}
					//符合SDP格式,其中属性a是自定义格式,也就是说可以存放自己的信息,但是只能是两列,比如帐户信息
					//但是经测试,格式:v o t必不可少,原因未知,估计是协议栈在传输时需要检查的
					snprintf (tmp, 4096,
							"v=0/r/n"
							"o=anonymous 0 0 IN IP4 0.0.0.0/r/n"
							"t=1 10/r/n"
							"a=username:rainfish/r/n"
							"a=password:123/r/n");
					osip_message_set_body (invite, tmp, strlen(tmp));
					osip_message_set_content_type (invite, "application/sdp");

					eXosip_lock (context_eXosip);
					i = eXosip_call_send_initial_invite (context_eXosip,invite);
					eXosip_unlock (context_eXosip);
					flag1 = 1;
					while (flag1)
					{
						je = eXosip_event_wait (context_eXosip,0, 200);

						if (je == NULL)
						{
							printf ("No response or the time is over!/n");
							break;
						}

						switch (je->type)
						{
							case EXOSIP_CALL_INVITE:
								printf ("a new invite reveived!/n");
								break;
							case EXOSIP_CALL_PROCEEDING:
								printf ("proceeding!/n");
								break;
							case EXOSIP_CALL_RINGING:
								printf ("ringing!/n");
								// call_id = je->cid;
								// dialog_id = je->did;
								printf ("call_id is %d, dialog_id is %d /n", je->cid, je->did);
								break;
							case EXOSIP_CALL_ANSWERED:
								printf ("ok! connected!/n");
								call_id = je->cid;
								dialog_id = je->did;
								printf ("call_id is %d, dialog_id is %d /n", je->cid, je->did);

								eXosip_call_build_ack (context_eXosip, je->did, &ack);
								eXosip_call_send_ack (context_eXosip, je->did, ack);
								flag1 = 0;
								break;
							case EXOSIP_CALL_CLOSED:
								printf ("the other sid closed!/n");
								break;
							case EXOSIP_CALL_ACK:
								printf ("ACK received!/n");
								break;
							default:
								printf ("other response!/n");
								break;
						}
						eXosip_event_free (je);

					}
					break;
				}
			case 'h':
				printf ("Holded !/n");

				eXosip_lock (context_eXosip);
				eXosip_call_terminate (context_eXosip, call_id, dialog_id);
				eXosip_unlock (context_eXosip);
				break;
			case 'c':
				printf ("This modal isn't commpleted!/n");
				break;
			case 's':
				//传输INFO方法
				eXosip_call_build_info (context_eXosip, dialog_id, &info);
				printf("--------------------1\r\n");
				snprintf (tmp , 4096,
						"hello rainfish");
				osip_message_set_body (info, tmp, strlen(tmp));
				printf("--------------------2\r\n");
				//格式可以任意设定,text/plain代表文本信息
				osip_message_set_content_type (info, "text/plain");
				printf("--------------------3\r\n");
				eXosip_call_send_request (context_eXosip, dialog_id, info);
				break;
			case 'm':
				//传输MESSAGE方法,也就是即时消息，和INFO方法相比，我认为主要区别，是MESSAGE不用建立连接，直接传输信息，而INFO必须
				//在建立INVITE的基础上传输。
				printf ("the mothed :MESSAGE\n");
				int value;
				eXosip_message_build_request (context_eXosip,  &message, "MESSAGE", dest_call, source_call, NULL);
				snprintf (tmp, 4096,
						"hello rainfish");
				printf("tmp is %s\r\n",tmp);
				osip_message_set_body (message, tmp, strlen(tmp));
				//假设格式是xml
				value = osip_message_set_content_type (message, "text/xml");
				printf("osip_message_set_content_type is %d\r\n",value);
				value = eXosip_message_send_request (context_eXosip, message);
				printf("value is %d\r\n",value);
				break;
			case 'q':
				eXosip_quit (context_eXosip);
				printf ("Exit the setup!/n");
				flag = 0;
				break;
		}
	}
#endif
	return (0);
}

