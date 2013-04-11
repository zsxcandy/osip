
	{
		//print the sdp 
		printf("v=%s\r\n", sdp_message_v_version_get(sdp_request);
		printf("o=<%s %s %s %s %s %s>\r\n",sdp_message_o_username_get(sdp_request),
		   				sdp_message_o_sess_id_get(sdp_request),
						sdp_message_o_sess_version_get(sdp_request),
						sdp_message_o_nettype_get(sdp_request),
						sdp_message_o_addrtype_get(sdp_request),
						sdp_message_o_addr(sdp_request));	
		printf("s=%s\r\n", sdp_message_s_name_get(sdp_request));
		printf("t=%s %s\r\n", sdp_message_t_start_time_get(sdp_request),
							  sdp_message_t_stop_time_get(sdp_request));
		
		while(!sdp_message_endof_media(sdp_request, index))
		{
			printf("m=%s %s %s %s\r\n", sdp_message_m_media_get(sdp_request, index),
										sdp_message_m_port_get(sdp_request , index),
										sdp_message_m_proto_get(sdp_request , index),
										"");
			childindex = 0;
			struct sdp_attribute *attri = NULL;
			do{
				attri = sdp_message_attribute_get(sdp_request , index , childindex);
				if(attri != NULL)
				{
					printf("\tattri->field=%s , attri->value=%s\r\n", 
							attri->a_att_field, attr->a_att_value);
				}
				if((attri != NULL)&&(strcmp(attri->a_att_field,"type")==0))
				{
					attri_type =  *attri;
				}	
				childindex++;
			}while(attri != NULL);
			index++;
		}
			
	}
