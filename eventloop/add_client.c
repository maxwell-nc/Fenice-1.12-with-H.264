
#include <stdio.h>

#include <fenice/eventloop.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

void add_client(
		RTSP_buffer ** rtsp_list,
		tsocket fd,
		RTSP_proto proto)
{
	RTSP_buffer *p = NULL, *pp = NULL;
	// Add a client
	if (*rtsp_list == NULL) {
		*rtsp_list = xnew0(RTSP_buffer);
		p = *rtsp_list;
	} else {
		for (p = *rtsp_list; p != NULL; p = p->next) {
			pp = p;
		}
		if (pp != NULL) {
			if (!(pp->next = (RTSP_buffer *) calloc(1, sizeof(RTSP_buffer)))) {
				FATALLOGG("Could not alloc memory");
				return;
			}
			p = pp->next;
			p->next = NULL;
		}
	}
	RTSP_initserver(p, fd, proto);
	INFOLOGG("Incoming RTSP connection accepted on socket %d", p->fd);
}
