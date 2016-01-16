
#include <string.h>

#include <fenice/rtsp.h>
#include <fenice/log.h>

int RTSP_handler(RTSP_buffer * rtsp)
{
	unsigned short status;
	char msg[100];
	int m, op;
	int full_msg;
	RTP_session *rtp_s;
	int hlen, blen;

	while (rtsp->in_size) {
		full_msg = RTSP_full_msg_rcvd(rtsp, &hlen, &blen);
		switch (full_msg) {
		case RTSP_method_rcvd:
			op = RTSP_valid_response_msg(&status, msg, rtsp);
			if (op == 0) {
				// There is NOT an input RTSP message, therefore it's a request
				m = RTSP_validate_method(rtsp);
				if (m < 0) {
					// Bad request: non-existing method
					ERRORLOGG("Bad Request ");
					send_reply(400, NULL, rtsp);
				} else
					RTSP_state_machine(rtsp, m);
			} else {
				// There's a RTSP answer in input.
				if (op == ERR_GENERIC) {
					// Invalid answer
				}
			}
			RTSP_discard_msg(rtsp);
			break;
		case RTSP_interlvd_rcvd:
			m = rtsp->in_buffer[1];
			for (rtp_s = (rtsp->session_list) ?
						 rtsp->session_list->rtp_session : NULL;
			     rtp_s
			     && !((rtp_s->transport.u.tcp.interleaved.RTP == m)
				  || (rtp_s->transport.u.tcp.interleaved.RTCP == m));
				 rtp_s = rtp_s->next);
			if (!rtp_s) {	// session not found
				DEBUGLOGG("Interleaved RTP or RTCP packet arrived for unknown channel (%d)... discarding.\n", m);
				RTSP_discard_msg(rtsp);
				break;
			}
			if (m == rtp_s->transport.u.tcp.interleaved.RTCP) {	// RTCP pkt arrived
				DEBUGLOGG("Interleaved RTCP packet arrived for channel %d (len: %d).\n", m, blen);
				if ((int) sizeof(rtp_s->rtcp_inbuffer) >= hlen + blen) {
					memcpy(rtp_s->rtcp_inbuffer, &rtsp->in_buffer[hlen], hlen + blen);
					rtp_s->rtcp_insize = blen;
				} else
					DEBUGLOGG("Interleaved RTCP packet too big!.", m);
				RTCP_recv_packet(rtp_s);
			} else {// RTP pkt arrived: do nothing...
				DEBUGLOGG("Interleaved RTP packet arrived for channel %d.", m);
			}
			RTSP_discard_msg(rtsp);
			break;
		default:
			return full_msg;
			break;
		}
	}

	return ERR_NOERROR;
}
