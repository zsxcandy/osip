#ifndef _SDP_HANDLE_REQUEST_H
#define _SDP_HANDLE_REQUEST_H

#if 0
#ifdef __cplusplus
extern "C"{
#endif
#endif
/*message request*/

#define SIP_MAXSIZE 256


typedef succeed_type (handle_sdp_request)(struct sip_message *request, struct sip_message *response);

typedef struct _function_index{
	char *key;
	handle_sdp_request *fun;
}function_index; 


succeed_type switch_to_function(struct osip_message *request , struct osip_message *message);

succeed_type message_route_update_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_route_del_func(struct osip_message *sip_request ,struct osip_message *sip_response);

succeed_type message_service_verify_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_info_modify_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_mp_reg_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_mp_dereg_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_status_update_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type message_info_query_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type message_cache_notify_func(struct osip_message *sip_request , struct osip_message *sip_response);

succeed_type message_cache_del_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type message_link_detech_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type message_heart_beat_func(struct osip_message *sip_request , struct osip_message *sip_response);




succeed_type message_cache_obtain_func(struct osip_message *sip_request , struct osip_message *sip_response);




succeed_type message_phone_access_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type message_obtain_notify_func(struct osip_message *sip_request , struct osip_message *sip_response);
/*register request*/
succeed_type register_reg_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type register_dereg_func(struct osip_message *sip_request , struct osip_message *sip_response);


/*invite request handle*/
succeed_type invite_voice_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_ppt_voice_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_data_broadcast_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_voice_broadcast_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_urgent_data_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_email_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_txt_msg_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_code_cmd_func(struct osip_message *sip_request , struct osip_message *sip_response);


succeed_type invite_define_data_func(struct osip_message *sip_request , struct osip_message *sip_response);


/*subscribe*/
succeed_type subscribe_status_subscribe_func(struct osip_message *sip_request , struct osip_message *sip_response);
#if 0
#ifdef __cplusplus
}
#endif
#endif



#endif
