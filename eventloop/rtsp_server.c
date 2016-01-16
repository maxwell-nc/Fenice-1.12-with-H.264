

#include <stdio.h>
#include <string.h>

#include <fenice/eventloop.h>
#include <fenice/log.h>

int rtsp_server(
		RTSP_buffer * rtsp,
		fd_set * rset,
		fd_set * wset,
		fd_set * xset)
{
	int size;
	char buffer[RTSP_BUFFERSIZE + 1];	/* +1 to control the final '\0' */
	int n;
	int res;
	RTSP_session *q = NULL;
	RTP_session *p = NULL;
	RTSP_interleaved *intlvd;
	uint16 *pkt_size = (uint16 *) & rtsp->out_buffer[2];

	if (rtsp == NULL) {
		return ERR_NOERROR;
	}
	if (FD_ISSET(rtsp->fd, wset)) { // first of all: there is some data to send?
		//char is_interlvd = rtsp->interleaved_size ? 1 : 0; 
		// There are RTSP packets to send
		if ( (n = RTSP_send(rtsp)) < 0) {
			send_reply(500, NULL, rtsp);
			return ERR_GENERIC;// internal server error
		}

		DEBUGLOGG("S-->C\n%s", rtsp->out_buffer);
#ifdef VERBOSE
		else if (!is_interlvd) {
		 	INFOLOGG("OUTPUT_BUFFER was:");
			dump_buffer(rtsp->out_buffer);
		}
#endif
	}

	if (FD_ISSET(rtsp->fd, rset)) {
		// There are RTSP or RTCP packets to read in
		memset(buffer, 0, sizeof(buffer));
		size = sizeof(buffer) - 1;
		n = tcp_read(rtsp->fd, buffer, size);
		if (n == 0) {
			return ERR_CONNECTION_CLOSE;
		}
		if (n < 0) {
			DEBUGLOGG("tcp_read() error in rtsp_server()");
			send_reply(500, NULL, rtsp);
			return ERR_GENERIC;	//errore interno al server                           
		}
		if (rtsp->in_size + n > RTSP_BUFFERSIZE) {
			DEBUGLOGG("RTSP buffer overflow (input RTSP message is most likely invalid).\n");
			send_reply(500, NULL, rtsp);
			return ERR_GENERIC;	//errore da comunicare
		}
		DEBUGLOGG("C-->S\n%s", buffer);
#ifdef VERBOSE
			INFOLOGG("INPUT_BUFFER was:");
		dump_buffer(buffer);
#endif
		memcpy(&(rtsp->in_buffer[rtsp->in_size]), buffer, n);
		rtsp->in_size += n;
		//TODO: SCTP is packet aware! Change behaviour to reflect flags == MSG_EOR or not
		if ((res = RTSP_handler(rtsp)) == ERR_GENERIC) {
			ERRORLOGG("Invalid input message.");
			return ERR_NOERROR;
		}
	}

	for (intlvd=rtsp->interleaved; intlvd && !rtsp->out_size; intlvd=intlvd->next) {
		if ( FD_ISSET(intlvd->rtcp_fd, rset) ) {
			if ( (n = read(intlvd->rtcp_fd, rtsp->out_buffer+4, sizeof(rtsp->out_buffer)-4)) < 0) {
				// if ( (intlvd->out_size = read(intlvd->rtcp_fd, intlvd->out_buffer, sizeof(intlvd->out_buffer))) < 0) {
				ERRORLOGG("Error reading from local socket.");
				continue;
			}

			rtsp->out_buffer[0] = '$';
			rtsp->out_buffer[1] = (unsigned char) intlvd->proto.tcp.rtcp_ch;
			*pkt_size = htons((uint16) n);
			rtsp->out_size = n+4;
			if ( (n = RTSP_send(rtsp)) < 0) {
				send_reply(500, NULL, rtsp);
				return ERR_GENERIC;// internal server error
			}
		} else if ( FD_ISSET(intlvd->rtp_fd, rset) ) {
			if ( (n = read(intlvd->rtp_fd, rtsp->out_buffer+4, sizeof(rtsp->out_buffer)-4)) < 0) {
				// if ( (n = read(intlvd->rtp_fd, intlvd->out_buffer, sizeof(intlvd->out_buffer))) < 0) {
				ERRORLOGG("Error reading from local socket.");
				continue;
			}
			rtsp->out_buffer[0] = '$';
			rtsp->out_buffer[1] = (unsigned char) intlvd->proto.tcp.rtp_ch;
			*pkt_size = htons((uint16) n);
			rtsp->out_size = n+4;
			if ( (n = RTSP_send(rtsp)) < 0) {
				send_reply(500, NULL, rtsp);
				return ERR_GENERIC;// internal server error
			}
		}
	}

	for (q = rtsp->session_list, p = q ? q->rtp_session : NULL; p; p = p->next) {
		if ( (p->transport.rtcp_fd_in >= 0) &&
			 FD_ISSET(p->transport.rtcp_fd_in, rset)) {
			// There are RTCP packets to read in
			if (RTP_recv(p, rtcp_proto) < 0) {
				INFOLOGG("Input RTCP packet Lost.");
			} else {
				RTCP_recv_packet(p);
			}
			INFOLOGG("IN RTCP.");
		}
		/*---------SEE rtcp/RTCP_handler.c-----------------*/
		/* if (FD_ISSET(p->rtcp_fd_out,wset)) {
		 * 	// There are RTCP packets to send
		 * 	if ((psize_sent=sendto(p->rtcp_fd_out,p->rtcp_outbuffer,p->rtcp_outsize,0,&(p->rtcp_out_peer),sizeof(p->rtcp_out_peer)))<0) {
		 * 		fnc_log(FNC_LOG_VERBOSE,"RTCP Packet Lost\n");
		 * 	}
		 * 	p->rtcp_outsize=0;
		 * 	fnc_log(FNC_LOG_VERBOSE,"OUT RTCP\n");
		 * } */
	}

	return ERR_NOERROR;
}
