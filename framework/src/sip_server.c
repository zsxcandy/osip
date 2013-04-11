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





typedef struct regparam_t {
	int regid;
	int expiry;
	int auth;
} regparam_t;

struct eXosip_t *context_eXosip;
int is_stop_notify = 1;

#ifndef OSIP_MONOTHREAD
static void *notify_proc (void *arg)
{
	struct regparam_t *regparam = arg;
	int reg;
	
/*	while(is_stop_notify)
	{
		
		
	}
*/	return NULL;
}
#endif
	
struct osip_thread *notify_thread;


int main (int argc, char *argv[])
{
	int port = 5060;
	eXosip_event_t *event = NULL;
	succeed_type flag;
#if !defined(__arc__)
	struct servent *service = NULL;
#endif
	struct regparam_t regparam = { 0, 3600, 0 };
	int debug = 1;

	port = 5060;
#if 0
	if (debug > 0)
		TRACE_INITIALIZE (6, NULL);
#endif
	context_eXosip = eXosip_malloc ();
	if (eXosip_init (context_eXosip)) {
		printf("eXosip_init failed\r\n");
		exit (1);
	}
	if (eXosip_listen_addr (context_eXosip, IPPROTO_UDP, "192.168.5.118", port, AF_INET, 0)) {
		printf("eXosip_listen_addr failed\r\n");
		exit (1);
	}

	/*start the notify thread */

#ifndef OSIP_MONOTHREAD
	notify_thread = osip_thread_create(20000, notify_proc, &regparam);

#endif

	for(;;)
	{

		eXosip_event_t *event;

		if (!(event = eXosip_event_wait (context_eXosip, 0, 1))) {
#ifdef OSIP_MONOTHREAD
			eXosip_execute (context_eXosip);
			eXosip_automatic_action (context_eXosip);
#endif
//			osip_usleep (10000);
			continue;
		}

		eXosip_lock(context_eXosip);
#ifdef OSIP_MONOTHREAD	
		eXosip_execute (context_eXosip);
#endif			
		//		eXosip_automatic_action (context_eXosip);
		eXosip_unlock(context_eXosip);
		
		eXosip_automatic_action(context_eXosip);
		switch (event->type) {

			case EXOSIP_CALL_INVITE: 
				if(MSG_IS_INVITE(event->request))
				{
					flag = handle_request_invite(context_eXosip, event);
					if(flag == succeed_type_failed)
					{
						printf("handle invite request failed\r\n");
					}else{
						printf("handle invite request succeed\r\n");
					}
					continue;
				}
				break;
			case EXOSIP_CALL_REINVITE:
				break;
			case EXOSIP_CALL_NOANSWER:
				break;
			case EXOSIP_CALL_PROCEEDING:
				break;
			case EXOSIP_CALL_RINGING:
				break;
			case EXOSIP_CALL_ANSWERED:
				break;
			case EXOSIP_CALL_REDIRECTED:
				break;
			case EXOSIP_CALL_REQUESTFAILURE:
				break;
			case EXOSIP_CALL_SERVERFAILURE:
				break;
			case EXOSIP_CALL_GLOBALFAILURE:
				break;
			case EXOSIP_CALL_ACK:
				break;
			case EXOSIP_CALL_CANCELLED:
				break;
			case EXOSIP_CALL_MESSAGE_NEW:

				break;
			case EXOSIP_CALL_MESSAGE_PROCEEDING:
				printf("EXOSIP_CALL_MESSAGE_PROCEEDING\r\n");
				break;
			case EXOSIP_CALL_MESSAGE_ANSWERED:
				break;
			case EXOSIP_CALL_MESSAGE_REDIRECTED:
				break;
			case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
				break;
			case EXOSIP_CALL_MESSAGE_SERVERFAILURE:
				break;
			case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
				break;
			case EXOSIP_CALL_CLOSED:
				break;
			case EXOSIP_CALL_RELEASED:
				break;
			case EXOSIP_MESSAGE_NEW:
				printf("answer EXOSIP_MESSAGE_NEW\r\n");
				if(MSG_IS_MESSAGE(event->request)) 
				{
					flag = handle_request_message(context_eXosip, event);
					if(flag == succeed_type_failed)
					{
						printf("handle message request failed\r\n");
					}else{
						printf("handle message request succeed\r\n");
					}
					continue;
				}

				if(MSG_IS_REGISTER(event->request))
				{
					flag = handle_request_register(context_eXosip, event);
					if(flag == succeed_type_failed)
					{
						printf("handle register request failed\r\n");
					}else{
						printf("handle register request succeed\r\n");
					}
					continue;
				}

				break;
			case EXOSIP_MESSAGE_PROCEEDING:
				break;
			case EXOSIP_MESSAGE_ANSWERED:
				break;
			case EXOSIP_MESSAGE_REDIRECTED:
				break;
			case EXOSIP_MESSAGE_REQUESTFAILURE:
				break;
			case EXOSIP_MESSAGE_SERVERFAILURE:
				break;
			case EXOSIP_MESSAGE_GLOBALFAILURE:
				break;
			case EXOSIP_SUBSCRIPTION_NOANSWER:
				break;
			case EXOSIP_SUBSCRIPTION_PROCEEDING:
				break;
			case EXOSIP_SUBSCRIPTION_ANSWERED:
				break;
			case EXOSIP_SUBSCRIPTION_REDIRECTED:
				break;
			case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
				break;
			case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
				break;
			case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
				break;
			case EXOSIP_SUBSCRIPTION_NOTIFY:
				break;
			case EXOSIP_IN_SUBSCRIPTION_NEW:  //subscribe event
				if(MSG_IS_SUBSCRIBE(event->request))
				{

					flag = handle_request_subscribe(context_eXosip, event);
					if(flag == succeed_type_failed)
					{
						printf("handle subscribe request failed\r\n");
					}else{
						printf("handle subscribe request succeed\r\n");
					}
					continue;
				}
				break;
			case EXOSIP_NOTIFICATION_NOANSWER:
				break;
			case EXOSIP_NOTIFICATION_PROCEEDING:
				break;
			case EXOSIP_NOTIFICATION_ANSWERED:
				break;
			case EXOSIP_NOTIFICATION_REDIRECTED:
				break;
			case EXOSIP_NOTIFICATION_REQUESTFAILURE:
				break;
			case EXOSIP_NOTIFICATION_SERVERFAILURE:
				break;
			case EXOSIP_NOTIFICATION_GLOBALFAILURE:
				break;
			case EXOSIP_EVENT_COUNT:
				break;
			default:
				printf( "recieved unknown eXosip event (type, did, cid) = (%d, %d, %d)", event->type, event->did, event->cid);

		}
		eXosip_event_free (event);
	}
		

	
}








#if 0
//message
{
	osip_message_t *message = NULL;
	int i ;
	int cid;
	void *reference = NULL;

	i = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",  "sip:to@192.168.5.104:5060","sip:from@192.168.5.234:5060", NULL);
	if(i != 0)
	{
		printf("eXosip_message_build_request failure \r\n");
		return 1;
	}
	//		osip_message_set_supported(message, "100rel");
	{
		char tmp[4096];
		char localip[128];
		eXosip_guess_localip (context_eXosip, AF_INET, localip, 128);
		printf("localip=%s\r\n", localip);
		printf("before snprintf\r\n");
		snprintf (tmp, 4096,
				"v=0\r\n"
				"o=jack 0 0 IN IP4 %s\r\n"
				"s=conversation\r\n"
				"c=IN IP4 %s\r\n"
				"t=0 0\r\n"
				"m=audio %d RTP/AVP 0 8 101\r\n"
				"a=rtpmap:0 PCMU/8000\r\n"
				"a=rtpmap:8 PCMA/8000\r\n"
				"a=rtpmap:101 telephone-event/8000\r\n"
				"a=fmtp:101 0-11\r\n", localip, localip, port);
		printf("before osip_message_set_body\r\n");
		osip_message_set_body (message, tmp, strlen (tmp));
		printf("after osip_message_set_body\r\n");
		osip_message_set_content_type (message, "application/sdp");
		printf("after osip_message_set_content_type\r\n");

		printf("------------------------------------\r\n");
		printf("tmp=%s", tmp);
		printf("------------------------------------\r\n");
	}
	eXosip_lock (context_eXosip);
	printf("before call_send_inital\r\n");
	cid = eXosip_message_send_request(context_eXosip, message);
	printf("after call_send_inital\r\n");
	if(cid > 0)
	{
		printf("into \r\n");
		eXosip_call_set_reference(context_eXosip, cid , reference);
		printf("out \r\n");
	}
	eXosip_unlock(context_eXosip);		


}
#endif
