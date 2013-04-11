#include "sip_system.h"
#include <osipparser2/sdp_message.h>
#include <osipparser2/osip_port.h>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>
#include "sdp_handle_request.h"


function_index HANDLE_REQUEST[SIP_MAXSIZE] = 
{
	{
		"data_reg",
		register_reg_func,
	},
	{
		"data_dereg",
		register_dereg_func,
	},
	{
		"data_voice",
		invite_voice_func,
	},
	{
		"data_ptt_video",
		invite_ppt_voice_func,
	},
	{
		"data_data_broadcast",
		invite_data_broadcast_func,
	},
	{
		"data_voice_broadcast",
			invite_voice_broadcast_func,
	},
	{
		"data_urgent_data",
		invite_urgent_data_func,
	},
	{
		"data_email",
		invite_email_func,
	},
	{
		"data_txt_msg",
		invite_txt_msg_func,
	},
	{
		"data_code_cmd",
		invite_code_cmd_func,
	},
	{
		"data_define_data",
		invite_define_data_func,
	},
	{
		"data_status_subscribe",
		subscribe_status_subscribe_func,
	},
	{
		"data_route_update",
		message_route_update_func,
	},
	{
		"data_route_del",
		message_route_del_func,
	},
	{
		"data_service_verify",
		message_service_verify_func,
	},
	{
		"data_info_modify",
		message_info_modify_func,
	},
	{
		"data_mp_reg",
		message_mp_reg_func,
	},
	{
		"data_mp_dereg",
		message_mp_dereg_func,
	},
	{
		"data_status_update",
		message_status_update_func,
	},
	{
		"data_info_query",
		message_info_query_func,
	},
	{
		"data_phone_access",
		message_phone_access_func,
	},
	{
		"data_cache_notify",
		message_cache_notify_func,
	},
	{
		"data_cache_obtain",
		message_cache_obtain_func,
	},
	{
		"data_obtain_notify",
		message_obtain_notify_func,
	},
	{
		"data_cache_del",
		message_cache_del_func,
	},
	{
		"data_link_detech",
		message_link_detech_func,
	},
	{
		"data_heart_beat",
		message_heart_beat_func,
	},
};

succeed_type switch_to_function(struct osip_message *request , struct osip_message *answer)
{
	int index = 0 , childindex = 0;
	succeed_type flag = succeed_type_succeed;
	unsigned char key[128] ;
	char *request_type = NULL;
	struct sdp_attribute attri_type ;
	osip_body_t *body = NULL;
	sdp_message_t *sdp_request = NULL;
	char *dest = NULL;

	
	if(request==NULL || answer == NULL)
	{
		return succeed_type_failed;
	}

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		printf("sdp_message_init failure\r\n");
		return succeed_type_failed;
	}

	osip_message_get_body(request, 0 , &body);
	if(body == NULL)
	{
		printf("osip_message_get_body failure\r\n");
		return succeed_type_failed;	
	}
	
	printf("I get mesg is :%s \r\n", body->body);
	flag = sdp_message_parse(sdp_request , body->body);
	if(flag)
	{
		printf("sdp_message_parse failure \r\n");
		return succeed_type_failed;
	}

	while(!sdp_message_endof_media(sdp_request, index))
	{
/*
		printf("m=%s %s %s %s\r\n", sdp_message_m_media_get(sdp_request, index),
				sdp_message_m_port_get(sdp_request , index),
				sdp_message_m_proto_get(sdp_request , index),
				"");
*/
		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
/*
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
*/			}
			if((attri != NULL)&&(strcmp(attri->a_att_field,"type")==0))
			{
				attri_type =  *attri;
			}	
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if((request_type == NULL))
	{
		return succeed_type_failed;
	}
//	printf("request_type=%s, attri_field=%s\r\n", request_type, attri_type.a_att_value);

	memset(key , 0x00, sizeof(key));
	snprintf(key,128,"%s_%s",
			request_type, attri_type.a_att_value);
	
//	printf("key:%s\r\n", key);
	for(index = 0 ; index < SIP_MAXSIZE && HANDLE_REQUEST[index].key != NULL; index++)
	{
		
//		printf("==%s \r\n", HANDLE_REQUEST[index].key);
		if(strcmp(HANDLE_REQUEST[index].key, key) ==0)
		{
			flag = HANDLE_REQUEST[index].fun(request, answer);
			if(flag == succeed_type_succeed)
			{
				return succeed_type_succeed;
			}else{
				return succeed_type_failed;
			}
		}
	}
	printf("the type is error, we must the reback , that we can stop the client continue send packages \r\n");
	return succeed_type_failed;

}


succeed_type message_route_update_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{

	int index = 0 , childindex = 0;
    struct sdp_attribute *phonenum_attri = NULL;
	struct sdp_attribute *route_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
//				printf("\tattri->field=%s , attri->value=%s\r\n", 
//						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "phonenum"))
				{
					phonenum_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "route"))
				{
					route_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(phonenum_attri == NULL || route_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("phonenum=%s\r\n", phonenum_attri->a_att_value);
		printf("route=%s\r\n", route_attri->a_att_value);		
	}
			
	//need to modfity according to the database  :hzc
	{
		sdp_message_init(&sdp_response);
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

//		sdp_message_a_attribute_add(sdp_response, 0, "type", "route_update");
//		printf("media_get=%s\r\n",sdp_message_m_media_get(sdp_request, 0));
//		printf("m_port=%s\r\n",sdp_message_m_port_get(sdp_request, 0));
//		printf("m_protp_get=%s\r\n", sdp_message_m_proto_get(sdp_request, 0));
		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));
/*		char *type = (char *)malloc(128);
		snprintf(type, 128 , "type");
		char *route_update = (char*)malloc(128);
		snprintf(route_update, 128, "route_update"); //notice here, this is must be malloc, and in the sdp_message_free() free 
*/		
		sdp_message_a_attribute_add(sdp_response, 0, "type", "route_update");
	}
/*	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	
	printf("send sdp_buf=%s\r\n", buf);
	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
	
	osip_free(buf);
	sdp_message_free(sdp_response);

	return succeed_type_succeed;
}

succeed_type message_route_del_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{

	struct sip_from *sip_from = NULL;
	char *dest;
	sip_from = osip_message_get_from(sip_request);	
	if(sip_from == NULL)
	{
		return succeed_type_failed;
	}
	osip_from_to_str(sip_request->from, &dest);
	printf("sip:from=%s\r\n", dest);
	/*no sdp message body , what about the sip_response osip_message*/	
	return succeed_type_succeed;
}

succeed_type message_service_verify_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	
	int index = 0 , childindex = 0;
    struct sdp_attribute *service_type_attri = NULL;
	struct sdp_attribute *loc_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
	/*			printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
	*/			if(!strcmp(attri->a_att_field, "service_type"))
				{
					service_type_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "loc"))
				{
					loc_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(service_type_attri == NULL || loc_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("service_type=%s\r\n", service_type_attri->a_att_value);
		printf("loc=%s\r\n", loc_attri->a_att_value);		
	}
			
	//need to modfity according to the database  :hzc
	sdp_message_init(&sdp_response);
	{
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "service_verify");
		sdp_message_a_attribute_add(sdp_response, 0, "status", "online");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
	}
/*
	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	
//	printf("sdp_message_to_str\r\n");
//	printf("buf=%s  ......strlen(buf)..............%d\r\n", buf , strlen(buf));
	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
	
	if(buf != NULL)
		osip_free(buf);
	sdp_message_free(sdp_response);

	return succeed_type_succeed;
}

succeed_type message_info_modify_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *mphone_attri = NULL;
	struct sdp_attribute *email_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "email"))
				{
					email_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "mphone"))
				{
					mphone_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(email_attri == NULL && mphone_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		if(email_attri != NULL)
			printf("email=%s\r\n", email_attri->a_att_value);
		if(mphone_attri != NULL)
			printf("mphone=%s\r\n", mphone_attri->a_att_value);		
	}
			
	//need to modfity according to the database  :hzc

	sdp_message_init(&sdp_response);
	{
		/*the page is must needed*/
		
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "info_modify");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
											  
	}
/*	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");

	osip_free(buf);
	sdp_message_free(sdp_response);
	return succeed_type_succeed;
}

succeed_type message_mp_reg_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{

	int index = 0 , childindex = 0;
    struct sdp_attribute *phonenum_attri = NULL;
	struct sdp_attribute *password_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "phonenum"))
				{
					phonenum_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "password"))
				{
					password_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(phonenum_attri == NULL || password_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("--------------phonenum=%s\r\n", phonenum_attri->a_att_value);
		printf("--------------password=%s\r\n", password_attri->a_att_value);		
	}
#if 0			
	//need to modfity according to the database  :hzc
	{
		/*the page is must needed*/
		sdp_message_init(&sdp_response);
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "<info_modify>");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
											  
	}
/*	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
#endif
	return succeed_type_succeed;
}

succeed_type message_mp_dereg_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *phonenum_attri = NULL;
	struct sdp_attribute *password_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "phonenum"))
				{
					phonenum_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "password"))
				{
					password_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(phonenum_attri == NULL || password_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("phonenum=%s\r\n", phonenum_attri->a_att_value);
		printf("password=%s\r\n", password_attri->a_att_value);		
	}

#if 0

	//need to modfity according to the database  :hzc
	{
		/*the page is must needed*/
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "<mp_reg>");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
											  
	}
	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
	sdp_message_to_str(sdp_response , buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
#endif
	return succeed_type_succeed;
}

succeed_type message_status_update_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *status_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	printf("heere..............................\r\n");
	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "status"))
				{
					status_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(status_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("status=%s\r\n", status_attri->a_att_value);
	}
			
	//need to modfity according to the database  :hzc
	sdp_message_init(&sdp_response);
	{
		/*the page is must needed*/
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "status_update");
		sdp_message_a_attribute_add(sdp_response, 0, "status", "online");
											  
	}
/*	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
	
	osip_free(buf);
	sdp_message_free(sdp_response);
	
	return succeed_type_succeed;
}

succeed_type message_info_query_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *mphone_attri = NULL;
	struct sdp_attribute *email_attri = NULL;
    struct sdp_attribute *loc_attri = NULL;
	struct sdp_attribute *service_type_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;
	printf("message_info_query ................................................\r\n");
	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "email"))
				{
					email_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "mphone"))
				{
					mphone_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "service_type"))
				{
					service_type_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "loc"))
				{
					loc_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(email_attri == NULL || mphone_attri == NULL || 
			service_type_attri == NULL || loc_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("email=%s\r\n", email_attri->a_att_value);
		printf("mphone=%s\r\n", mphone_attri->a_att_value);		
		printf("loc=%s\r\n", loc_attri->a_att_value);
		printf("service_type=%s\r\n", service_type_attri->a_att_value);		
	}
			
	//need to modfity according to the database  :hzc
	sdp_message_init(&sdp_response);
	{
		/*the page is must needed*/
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "<info_modify>");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
											  
	}
/*	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
*/	sdp_message_to_str(sdp_response , &buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
	
	osip_free(buf);
	sdp_message_free(sdp_response);
	return succeed_type_succeed;
}
succeed_type message_phone_access_func(struct osip_message *sip_request , struct osip_message *sip_response)
{

	int index = 0 , childindex = 0;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	return succeed_type_succeed;
}



succeed_type message_cache_notify_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *cache_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "cache"))
				{
					cache_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(cache_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("cache=%s\r\n", cache_attri->a_att_value);
	}
#if 0			
	//need to modfity according to the database  :hzc
	{
		/*the page is must needed*/
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "type", "<info_modify>");
		sdp_message_a_attribute_add(sdp_response, 0, "result", "ok");
											  
	}
	buf = (char*)osip_malloc(4096);
	if(buf == NULL)
	{
		return succeed_type_failed;
	}
	memset(buf , 0x00, sizeof(char)*4096);
	sdp_message_to_str(sdp_response , buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");
#endif
	return succeed_type_succeed;
}

succeed_type message_cache_obtain_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *cache_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "cache"))
				{
					cache_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(cache_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("cache=%s\r\n", cache_attri->a_att_value);
	}
	return succeed_type_succeed;
}

succeed_type message_obtain_notify_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *cache_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "cache"))
				{
					cache_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(cache_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("cache=%s\r\n", cache_attri->a_att_value);
	}
	return succeed_type_succeed;
}

succeed_type message_cache_del_func(struct osip_message *sip_request , struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
    struct sdp_attribute *cache_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "cache"))
				{
					cache_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(cache_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("cache=%s\r\n", cache_attri->a_att_value);
	}
	return succeed_type_succeed;
}


succeed_type message_link_detech_func(struct osip_message *sip_request , struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	return succeed_type_succeed;
}

succeed_type message_heart_beat_func(struct osip_message *sip_request , struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	return succeed_type_succeed;
}

succeed_type register_reg_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{

	int index = 0 , childindex = 0;
    struct sdp_attribute *loc_attri = NULL;
    struct sdp_attribute *timestamp_attri = NULL;
    struct sdp_attribute *authdata_attri = NULL;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	printf("into register_reg_func...................................\r\n");
	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				if(!strcmp(attri->a_att_field, "loc"))
				{
					loc_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "timestamp"))
				{
					timestamp_attri = attri;
				}
				if(!strcmp(attri->a_att_field, "authdata"))
				{
					authdata_attri = attri;
				}
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	if(loc_attri == NULL || timestamp_attri == NULL || authdata_attri == NULL)
	{
		return succeed_type_failed;	
	}	

	{
		printf("loc=%s\r\n", loc_attri->a_att_value);
		printf("timestamp=%s\r\n", timestamp_attri->a_att_value);
		printf("authdata=%s\r\n", authdata_attri->a_att_value);
	}

	
	//need to modfity according to the database  :hzc
	sdp_message_init(&sdp_response);
	{
		/*the page is must needed*/
		sdp_message_v_version_set(sdp_response,sdp_message_v_version_get(sdp_request));
		sdp_message_o_origin_set(sdp_response, sdp_message_o_username_get(sdp_request),
		   									   sdp_message_o_sess_id_get(sdp_request),
											   sdp_message_o_sess_version_get(sdp_request),
											   sdp_message_o_nettype_get(sdp_request),
											   sdp_message_o_addrtype_get(sdp_request),
											   sdp_message_o_addr_get(sdp_request));	
		sdp_message_s_name_set(sdp_response ,  sdp_message_s_name_get(sdp_request));
		sdp_message_t_time_descr_add(sdp_response , sdp_message_t_start_time_get(sdp_request,0),
							  						sdp_message_t_stop_time_get(sdp_request,0));

		sdp_message_m_media_add(sdp_response, sdp_message_m_media_get(sdp_request, 0), 
											  sdp_message_m_port_get(sdp_request, 0), 
											  NULL,
											  sdp_message_m_proto_get(sdp_request, 0));

		sdp_message_a_attribute_add(sdp_response, 0, "phonenum", "XXXXXXXXXX");
		sdp_message_a_attribute_add(sdp_response, 0, "alloc", "TEMCA");
		sdp_message_a_attribute_add(sdp_response, 0, "cache", "100033 011 email length time");
		sdp_message_a_attribute_add(sdp_response, 0, "route", "<ID@HOST>");
		sdp_message_a_attribute_add(sdp_response, 0, "user_type", "normal");
	}

	sdp_message_to_str(sdp_response , &buf);	

	osip_message_set_body(sip_response, buf, strlen(buf));
	osip_message_set_content_type(sip_response, "application/sdp");

	osip_free(buf);
	sdp_message_free(sdp_response);
	
	return succeed_type_succeed;
}

succeed_type register_dereg_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	int index = 0 , childindex = 0;
	struct sdp_message *sdp_response = NULL;
	struct sdp_message *sdp_request = NULL;
	struct osip_body *body = NULL;
	succeed_type flag = succeed_type_succeed;
	char *buf = NULL;

	sdp_message_init(&sdp_request);
	if(sdp_request == NULL)
	{
		return succeed_type_failed;
	}
	osip_message_get_body(sip_request ,0,  &body);
	if(body == NULL)
	{
		return succeed_type_failed;
	}
	sdp_message_parse(sdp_request, body->body);
	index = 0;
	while(!sdp_message_endof_media(sdp_request, index))
	{
//		request_type = sdp_message_m_media_get(sdp_request, index);
		childindex = 0;
		struct sdp_attribute *attri = NULL;
		do{
			attri = sdp_message_attribute_get(sdp_request , index , childindex);
			if(attri != NULL)
			{
				printf("\tattri->field=%s , attri->value=%s\r\n", 
						attri->a_att_field, attri->a_att_value);
				
			}
			childindex++;
		}while(attri != NULL);
		index++;
	}

	return succeed_type_succeed;
}

succeed_type invite_voice_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_ppt_voice_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_data_broadcast_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_voice_broadcast_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_urgent_data_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_email_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_txt_msg_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_code_cmd_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type invite_define_data_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

succeed_type subscribe_status_subscribe_func(struct osip_message *sip_request ,struct osip_message *sip_response)
{
	return succeed_type_succeed;
}

