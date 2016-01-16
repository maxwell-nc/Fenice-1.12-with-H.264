
#include <fenice/eventloop.h>
#include <fenice/log.h>

int RTCP_handler(RTP_session * session)
{
	fd_set wset;
	struct timeval t;

//	TRACE_FUNC();

	if (session->rtcp_stats[i_server].pkt_count % 20 == 0) {
		if (session->rtcp_stats[i_server].pkt_count == 0)
			RTCP_send_packet(session, RR);
		else
			RTCP_send_packet(session, SR);
		RTCP_send_packet(session, SDES);
		/*---------------SEND PKT-------------------------*/
		/*---------------SEE eventloop/rtsp_server.c-------*/
		FD_ZERO(&wset);
		t.tv_sec = 0;
		t.tv_usec = 100000;

		if (session->rtcp_outsize > 0)
			FD_SET(session->transport.rtcp_fd_out, &wset);

		if (select(MAX_FDS, 0, &wset, 0, &t) < 0) {
			ERRORLOGG("select() error");
			/*send_reply(500, NULL, rtsp); */
			return ERR_GENERIC;	//errore interno al server
		}

		if (FD_ISSET(session->transport.rtcp_fd_out, &wset)) {
			if (RTP_sendto(session, rtcp_proto, session->rtcp_outbuffer,
			     session->rtcp_outsize) < 0)
				ERRORLOGG("RTCP Packet Lost");

			session->rtcp_outsize = 0;
//			INFOLOGG("OUT RTCP");
		}
		/*------------------------------------------------*/
	}
	return ERR_NOERROR;
}
