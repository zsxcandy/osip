#include "sip_private.h"

extern eXosip_t *context_eXosip;

int fun_main (int argc, char *argv[])
{
	int c;
	int port = 5060;
	char *contact = NULL;
	char *fromuser = NULL;
	const char *localip = NULL;
	const char *firewallip = NULL;
	char *proxy = NULL;
	int debug = 1;
	char *username = NULL;
	char *password = NULL;
	struct regparam_t regparam = { 0, 3600, 0 };
	int i = 0;
	osip_message_t *answer = NULL;
	osip_message_t *notify_msg = NULL;
	osip_body_t *body = NULL;
	TRACE_INITIALIZE(6 , NULL);
	char tmp[4096];
	int dialog_id = 0;
	int flag = 0;
#if 0
	openlog(PROG_NAME, LOG_PID | debug , SYSLOG_FACILITY);
#endif
	context_eXosip = eXosip_malloc ();

	if (eXosip_init (context_eXosip)) {
		printf ("%s\r\n", "eXosip_init failed");
		return 1;
	}

	if (eXosip_listen_addr (context_eXosip, IPPROTO_UDP, NULL, port, AF_INET, 0)) {
		printf ("%s", "eXosip_listen_addr failed");
		return 1;
	}

	for (;;) {
		eXosip_event_t *event; 
		osip_message_t  *answer;

		if(flag == 1)
		{
			if(dialog_id != 0 )
			{
				i = eXosip_insubscription_build_notify(context_eXosip, dialog_id ,
						EXOSIP_SUBCRSTATE_ACTIVE ,
						DEACTIVATED,
						&notify_msg);
				if(i == 0)
				{	
					printf("notify build success \r\n");
					char dtmf_body[1000];
					_snprintf (dtmf_body, 999, "Signal=%c\r\nDuration=250 huzhengchuan_notify \r\n", 'H');
						osip_message_set_content_type (notify_msg, "application/dtmf-relay");
						osip_message_set_body ( notify_msg , dtmf_body, strlen (dtmf_body));
					eXosip_insubscription_send_request( context_eXosip, dialog_id , notify_msg);
					printf("send notify message \r\n");

					for(;;)
					{
						event = eXosip_event_wait (context_eXosip, 0, 1);
						if(event == NULL)
						{
							eXosip_execute(context_eXosip);
							eXosip_automatic_action (context_eXosip);
							osip_usleep(10000);
							continue;
						}
						eXosip_lock(context_eXosip);
						eXosip_execute(context_eXosip);
						eXosip_unlock(context_eXosip);

						printf("\n recv sip packet\r\n");
						switch (event->type) 
						{
							case EXOSIP_NOTIFICATION_ANSWERED:
								printf("EXOSIP_NOTIFICATION_ANSWERED\r\n");

								break;
							case EXOSIP_NOTIFICATION_NOANSWER:
								printf("EXOSIP_NOTIFICATION_NOANSWERED\r\n");
								break;
							case EXOSIP_NOTIFICATION_PROCEEDING:
								printf("EXOSIP_NOTIFICATION_PROCEEDING\r\n");
								break;
							default:
								printf("NO KNOW BACK  %d ", event->type);
								break;
						}
					}//FOR(;;;)
				}else{// IF(I ==0)
					printf("notify send failure ........\r\n");
				}
			}//IF(DIALOG_ID != 0)

			flag = 0;
		}//IF(FLAG != 0)

		event = eXosip_event_wait (context_eXosip, 0, 1);

		if(event == NULL)
		{
			eXosip_execute(context_eXosip);
			eXosip_automatic_action (context_eXosip);
			osip_usleep(10000);
			continue;
		}

		eXosip_lock(context_eXosip);
		eXosip_execute(context_eXosip);
		eXosip_unlock(context_eXosip);

		printf("\n recv sip packet\r\n");
		switch (event->type) 
		{

			case EXOSIP_CALL_INVITE:  //处理INVITE事件
				regparam.auth = 1;
					eXosip_lock (context_eXosip);
					
					eXosip_lock (context_eXosip);
					i = eXosip_call_send_answer (context_eXosip, event->tid, 180, NULL);
					if(i  != 0)
					{
						printf("invite send 180  failure \r\n");
					}else{
						printf("invite send 180 success \r\n");
					}
				i = eXosip_call_build_answer(context_eXosip, event->tid, 200, &answer);
					if( i != 0)
					{
						printf("send 200ok  failure \r\n");
							eXosip_call_send_answer( context_eXosip, event->tid, 400, NULL);
							continue;
					}
				memset(tmp , 0x00 , sizeof(char)*4096);
					sprintf (tmp, 
							"v=0\r\n"
							"o=anonymous 0 0 IN IP4 0.0.0.0\r\n"
							"t=1 10\r\n"
							"a=username:rainfish\r\n"
							"a=password:123\r\n");
					osip_message_set_body(answer , tmp , strlen(tmp));
					osip_message_set_content_type(answer , "application/sdp");
					i = eXosip_call_send_answer( context_eXosip, event->tid, 200, answer);
					if( i != 0)
					{
						printf("send 200ok failure \r\n");
					}
				eXosip_unlock (context_eXosip);
					printf("send 200ok success \r\n");
#if 0
					i = eXosip_call_build_answer (context_eXosip, event->tid, 200, &answer);
					if (i != 0)
					{
						(context_eXosip, event->tid, 401, NULL);
					}
					else
					{
						printf("send register ok \r\n");
							eXosip_call_send_answer (context_eXosip, event->tid, 200, answer);
					}
				eXosip_unlock (context_eXosip);
#endif
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
				printf("answer EXOSIP_CALL_MESSAGE_NEW\r\n");
				if(MSG_IS_MESSAGE(event->request))  // 处理消息事件
				{
					printf("recv message event.............\r\n");

					osip_message_get_body(event->request, 0 , &body);
					if(body != NULL)
					{
						printf("I get mesg is :%s \r\n", body->body);
					}

					i = eXosip_message_build_answer(context_eXosip , event->tid, 200, &answer);
					if(i != 0)
					{
						printf("build 200 answer failure \r\n");
						continue;
					}
					eXosip_message_send_answer(context_eXosip, event->tid, 200, answer);
				}

				if(MSG_IS_REGISTER(event->request)) //处理注册事件
				{
					eXosip_lock(context_eXosip);
					i = eXosip_call_build_answer( context_eXosip , event->tid , 200,  &answer);
					if( i != 0)
					{
						//This request msg is invalid. canot response
						printf("send to the server that  401=====%d\r\n", i);
						eXosip_call_send_answer( context_eXosip , event->tid, 401, answer);
					}else{
						//insert SDP des into sip msg .
						printf("send to the server that 200ok\r\n");
						char dtmf_body[1000];
						_snprintf (dtmf_body, 999, "Signal=%c\r\nDuration=250\r\n", 'H');
							osip_message_set_content_type (answer, "application/dtmf-relay");
							osip_message_set_body ( answer , dtmf_body, strlen (dtmf_body));
						eXosip_call_send_answer( context_eXosip , event->tid, 200, answer); 
					}
					eXosip_unlock(context_eXosip);
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
			case	EXOSIP_SUBSCRIPTION_NOTIFY:
				break;
			case EXOSIP_IN_SUBSCRIPTION_NEW:  //subscribe event
				if(MSG_IS_SUBSCRIBE(event->request))
				{
					printf("recv subscription \r\n");
					eXosip_lock(context_eXosip);
					i = eXosip_insubscription_build_answer(context_eXosip, event->tid, 200, &answer);
					if(i != 0)
					{
						eXosip_unlock(context_eXosip);
						printf("eXosip build answer failure = %d \r\n" , i);
						continue;
					}
					dialog_id = event->did;
					printf("dialog_id = %d \r\n", dialog_id);
					char dtmf_body[1000];
					_snprintf (dtmf_body, 999, "Signal=%c\r\nDuration=250\r\n", 'H');
						osip_message_set_content_type (answer, "application/dtmf-relay");
						osip_message_set_body ( answer , dtmf_body, strlen (dtmf_body));

					eXosip_insubscription_send_answer(context_eXosip , event->tid , 200, answer);

					eXosip_unlock(context_eXosip);

					flag = 1;
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
