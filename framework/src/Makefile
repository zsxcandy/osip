CFLAGS=-pedantic -DENABLE_DEBUG -g -DENABLE_TRACE -g   -pthread
CC=gcc
SIP_CFLAGS=-I/home/zsx/osip/install/include
SIP_LIBS=-L/home/zsx/osip/install/lib -leXosip2 -losip2 -losipparser2  -lpthread -pedantic -lrt 

uac:uac.c
	${CC} -o $@ $^  ${CFLAGS} ${SIP_CFLAGS} ${SIP_LIBS}

#sip_server:sip_handle_request.o sdp_handle_request.o sip_server.c
#	${CC} -o $@ $^  ${CFLAGS} ${SIP_CFLAGS} ${SIP_LIBS}

sip_handle_request.o:sip_handle_request.c 
	${CC} -c -o $@ $^  ${CFLAGS} ${SIP_CFLAGS} ${SIP_LIBS}


clean:
	rm *.o sip_server uac
