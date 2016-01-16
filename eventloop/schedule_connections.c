
#include <stdio.h>
#include <unistd.h>

#include <fenice/socket.h>
#include <fenice/eventloop.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>

int stop_schedule = 0;
extern int num_conn;

void schedule_connections(
		RTSP_buffer ** rtsp_list,
		int *conn_count,
		fd_set * rset,
		fd_set * wset,
		fd_set * xset)
{
	int res;
	RTSP_buffer *p = *rtsp_list, *pp = NULL;
	RTP_session *r = NULL, *t = NULL;
	RTSP_interleaved *intlvd;

	while (p != NULL) {
		if ((res = rtsp_server(p, rset, wset, xset)) != ERR_NOERROR) {
			if (res == ERR_CONNECTION_CLOSE || res == ERR_GENERIC) {
				// The connection is closed
				if (res == ERR_CONNECTION_CLOSE) {
					INFOLOGG("RTSP connection closed by client.");
				} else {
					INFOLOGG("RTSP connection closed by server.");
				}

				if (p->session_list != NULL) {	//if client truncated RTSP connection before sending TEARDOWN: error
					r = p->session_list->rtp_session;
					// Release all RTP sessions
					while (r != NULL) {
						// if (r->current_media->pkt_buffer);
						// Release the scheduler entry
						t = r->next;
						schedule_remove(r->sched_id);
						r = t;
					}
					// Close connection                     
					//close(p->session_list->fd);
					// Release the RTSP session
					xfree(p->session_list);
					p->session_list = NULL;
					WARNLOGG("RTSP connection truncated before ending operations.");
				}
				// close localfds
				for (intlvd=p->interleaved; intlvd; intlvd = intlvd->next) {
					close(intlvd->rtp_fd);
					close(intlvd->rtcp_fd);
				}
				// wait for 
				close(p->fd);
				--*conn_count;
				num_conn--;
				// Release the RTSP_buffer
				if (p == *rtsp_list) {
					*rtsp_list = p->next;
					xfree(p);
					p = *rtsp_list;
				} else {
					pp->next = p->next;
					xfree(p);
					p = pp->next;
				}
				// Release the scheduler if necessary
				if (p == NULL && *conn_count < 0) {
					DEBUGLOGG("The thread is over");
					stop_schedule = 1;
				}
			} else {
				p = p->next;
			}
		} else {
			pp = p;
			p = p->next;
		}
	}
}
