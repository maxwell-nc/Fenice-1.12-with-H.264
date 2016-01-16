
#include <stdio.h>

#include <fenice/eventloop.h>

void rtsp_set_fdsets(
		RTSP_buffer * rtsp,
		int * max_fd ,
		fd_set * rset,
		fd_set * wset,
		fd_set * xset)
{
	RTSP_session *q = NULL;
	RTP_session *p = NULL;
	RTSP_interleaved *intlvd;

	if (rtsp == NULL) {
		return;
	}
	// FD used for RTSP connection
	FD_SET(rtsp->fd, rset);
	*max_fd = max(*max_fd, rtsp->fd);
	if (rtsp->out_size > 0) {
		FD_SET(rtsp->fd, wset);
	}
	// Local FDS for interleaved trasmission
	for (intlvd=rtsp->interleaved; intlvd; intlvd=intlvd->next) {
		FD_SET(intlvd->rtp_fd, rset);
		FD_SET(intlvd->rtcp_fd, rset);
		*max_fd = max(*max_fd, intlvd->rtp_fd);
		*max_fd = max(*max_fd, intlvd->rtcp_fd);
	}
	// RTCP input
	for (q = rtsp->session_list, p = q ? q->rtp_session : NULL; p; p = p->next) {

		if (!p->started) {
			q->cur_state = READY_STATE;	// play finished, go to ready state
			/* TODO: RTP struct to be freed */
		} else if (p->transport.rtcp_fd_in >= 0) {
			FD_SET(p->transport.rtcp_fd_in, rset);
			*max_fd = max(*max_fd, p->transport.rtcp_fd_in);
		}
	}
}
