#include "sip_system.h"
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>
#include <osipparser2/sdp_message.h>
#include <osipparser2/osip_port.h>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>
#include "sip_handle_request.h"


/*DES:handle message request
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 0=succeed_type_succeed -1=succeed_type_failed
 */
succeed_type handle_request_message(struct eXosip_t *eXosip, struct eXosip_event *event)
{

	int flag = 0;
	osip_message_t *answer = NULL;
	osip_message_t *request = NULL;


	request = event->request;
	if(request == NULL)
	{
		return succeed_type_failed;
	}

	
	flag = eXosip_message_build_answer(eXosip , event->tid, 200, &answer);
	if(flag != 0)
	{
		printf("build 200 answer failure \r\n");
		return succeed_type_failed;
	}
	
	flag = switch_to_function(request , answer);		
	if(flag == succeed_type_failed)
	{
	//notice there need to send 180 message	
		return succeed_type_failed;
	}

	eXosip_message_send_answer(eXosip, event->tid, 200, answer);
	

	return succeed_type_succeed;
}



/*DES:handle message register
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 0=succeed_type_succeed -1=succeed_type_failed
 */
succeed_type handle_request_register(struct eXosip_t *eXosip , struct eXosip_event *event )
{
	osip_body_t *body = NULL;
	osip_message_t *answer = NULL;
	osip_message_t *request = NULL;
	int flag = 0;

	if(eXosip == NULL || event == NULL)
	{
		return succeed_type_failed;
	}
	
	request = event->request;

	eXosip_lock(eXosip);
	flag = eXosip_call_build_answer( eXosip , event->tid , 200,  &answer);
	if( flag != 0)
	{
		//This request msg is invalid. canot response
		printf("send to the server that  401=====%d\r\n", flag);
		eXosip_call_send_answer( eXosip , event->tid, 401, answer);
	}else{
		//insert SDP des into sip msg .
		flag = switch_to_function(request , answer);
		if(flag == succeed_type_failed)
		{
			return succeed_type_failed;
		}
		eXosip_call_send_answer( eXosip , event->tid, 200, answer); 
	}
	eXosip_unlock(eXosip);
	return succeed_type_succeed;
}




/*DES:handle message subscribe
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 0=succeed_type_succeed -1=succeed_type_failed
 */
succeed_type handle_request_subscribe(struct eXosip_t *eXosip, struct eXosip_event *event)
{
	osip_body_t *body = NULL;
	osip_message_t *answer = NULL;
	succeed_type flag = succeed_type_succeed;
	int dialog_id;

	if(eXosip == NULL || event == NULL)
	{
		return succeed_type_failed;
	}

	printf("recv subscription \r\n");
	eXosip_lock(eXosip);
	flag = eXosip_insubscription_build_answer(eXosip, event->tid, 200, &answer);
	if(flag != 0)
	{
		eXosip_unlock(eXosip);
		printf("eXosip build answer failure = %d \r\n" , flag);
	}
	dialog_id = event->did;

	
	flag = switch_to_function(event->request , answer);
	if(flag == succeed_type_failed)
	{
			return succeed_type_failed;
	}

#if 0
	printf("dialog_id = %d \r\n", dialog_id);
	char dtmf_body[1000];
	snprintf (dtmf_body, 999, "Signal=%c\r\nDuration=250\r\n", 'H');
	osip_message_set_content_type (answer, "application/dtmf-relay");
	osip_message_set_body ( answer , dtmf_body, strlen (dtmf_body));
#endif
	eXosip_insubscription_send_answer(eXosip , event->tid , 200, answer);

	eXosip_unlock(eXosip);

	{
		//open the thread do notify
		
	}


	return succeed_type_succeed;
}


/*DES:handle message invite
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 0=succeed_type_succeed -1=succeed_type_failed
 */
succeed_type handle_request_invite(struct eXosip_t *eXosip, struct eXosip_event *event)
{
	osip_body_t *body = NULL;
	osip_message_t *answer = NULL;
	int flag = 0;
	char tmp = NULL;

	if(eXosip == NULL || event == NULL)
	{
		return succeed_type_failed;
	}

	eXosip_lock(eXosip);
	flag = eXosip_call_send_answer (eXosip, event->tid, 180, NULL);
	if(flag  != 0)
	{
		printf("invite send 180  failure \r\n");
	}else{
		printf("invite send 180 success \r\n");
	}
	flag = eXosip_call_build_answer(eXosip, event->tid, 200, &answer);
	if( flag != 0)
	{
		printf("send 200ok  failure \r\n");
		eXosip_call_send_answer( eXosip, event->tid, 400, NULL);
		return succeed_type_failed;
	}
	snprintf (tmp, 4096,
			"v=0\r\n"
			"o=anonymous 0 0 IN IP4 0.0.0.0\r\n"
			"t=1 10\r\n"
			"a=username:rainfish\r\n"
			"a=password:123\r\n");
	osip_message_set_body(answer , tmp , strlen(tmp));
	osip_message_set_content_type(answer , "application/sdp");
	flag = eXosip_call_send_answer( eXosip, event->tid, 200, answer);
	if( flag != 0)
	{
		printf("send 200ok failure \r\n");
	}
	eXosip_unlock (eXosip);
	printf("send 200ok success \r\n");

	return succeed_type_succeed;
}


