#ifndef _HANDLE_REQUEST_H_
#define _HANDLE_REQUEST_H_


#ifdef __cplusplus
extern "C"{
#endif

#include "sip_system.h"


/*DES:hadle message request
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 1=success 0=failure
 */
succeed_type handle_request_message(struct eXosip_t *eXosip, struct eXosip_event *event );


/*DES:hadle register request
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 1=success 0=failure
 */
succeed_type handle_request_register(struct eXosip_t *eXosip, struct eXosip_event *event);


/*DES:hadle subscribe request
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 1=success 0=failure
 */
succeed_type handle_request_subscribe(struct eXosip_t *eXosip, struct eXosip_event *event);


/*DES:hadle invite request
 *INPUT:eXosip_t , eXosip_event_t
 *OUTPUT: 1=success 0=failure
 */
succeed_type handle_request_invite(struct eXosip_t *eXosip, struct eXosip_event *event);


#ifdef __cplusplus
}
#endif


#endif
