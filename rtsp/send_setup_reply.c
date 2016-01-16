
#include <stdio.h>
#include <string.h>
#include <config.h>

#include <fenice/rtsp.h>
#include <fenice/multicast.h>
#include <fenice/log.h>

int send_setup_reply(RTSP_buffer * rtsp, RTSP_session * session,
		     SD_descr * descr, RTP_session * rtp_s)
{
	char r[1024];
	int w_pos = 0;
	char temp[30];
	char ttl[4];
	/* build a reply message */

	sprintf(r,
		"%s %d %s" RTSP_EL "CSeq: %d" RTSP_EL "Server: %s/%s" RTSP_EL,
		RTSP_VER, 200, get_stat(200), rtsp->rtsp_cseq, PACKAGE,
		VERSION);
	add_time_stamp(r, 0);
	w_pos = strlen(r);
	/*
	   strcat(r, "Session: ");
	   sprintf(temp, "%d", session->session_id);
	   strcat(r, temp);
	   strcat(r, RTSP_EL);
	 */
	w_pos += sprintf(r + w_pos, "Session: %d" RTSP_EL, session->session_id);

	w_pos += sprintf(r + w_pos, "Transport: ");
	switch (rtp_s->transport.type) {
	case RTP_rtp_avp:
		// if (!(descr->flags & SD_FL_MULTICAST)) {
		if (rtp_s->transport.u.udp.is_multicast) {
			/*
			   strcat(r, "Transport: RTP/AVP;multicast;");
			   sprintf(temp, "destination=%s;", descr->multicast);
			   strcat(r, temp);
			   strcat(r, "port=");
			 */
			w_pos +=
			    sprintf(r + w_pos,
				    "RTP/AVP;multicast;ttl=%d;destination=%s;port=",
				    (int) DEFAULT_TTL, descr->multicast);
		} else {
			/*
			   strcat(r, "Transport: RTP/AVP;unicast;client_port=");
			   sprintf(temp, "%d", rtp_s->transport.u.udp.cli_ports.RTP);
			   strcat(r, temp);
			   strcat(r, "-");
			   sprintf(temp, "%d", rtp_s->transport.u.udp.cli_ports.RTCP);
			   strcat(r, temp);

			   sprintf(temp, ";source=%s", get_address());
			   strcat(r, temp);

			   strcat(r, ";server_port=");
			 */
			w_pos +=
			    sprintf(r + w_pos,
				    "RTP/AVP;unicast;client_port=%d-%d;source=%s;server_port=",
				    rtp_s->transport.u.udp.cli_ports.RTP,
				    rtp_s->transport.u.udp.cli_ports.RTCP,
				    get_address());
		}
		/*
		   sprintf(temp, "%d", rtp_s->transport.u.udp.ser_ports.RTP);
		   strcat(r, temp);
		   strcat(r, "-");
		   sprintf(temp, "%d", rtp_s->transport.u.udp.ser_ports.RTCP);
		   strcat(r, temp);
		 */
		w_pos +=
		    sprintf(r + w_pos, "%d-%d",
			    rtp_s->transport.u.udp.ser_ports.RTP,
			    rtp_s->transport.u.udp.ser_ports.RTCP);

#if 0
		// if ((descr->flags & SD_FL_MULTICAST)) {
		if (rtp_s->transport.u.udp.is_multicast) {
			/*
			   strcat(r,";ttl=");
			   sprintf(ttl,"%d",(int)DEFAULT_TTL);
			   strcat(r,ttl);
			 */
			w_pos +=
			    sprintf(r + w_pos, ";ttl=%d", (int) DEFAULT_TTL);
		}
#endif
		break;
	case RTP_rtp_avp_tcp:
		w_pos += sprintf(r + w_pos, "RTP/AVP/TCP;interleaved=%d-%d",
				 rtp_s->transport.u.tcp.interleaved.RTP,
				 rtp_s->transport.u.tcp.interleaved.RTCP);
		break;
	case RTP_rtp_avp_sctp:
		w_pos += sprintf(r + w_pos, "RTP/AVP/SCTP;server_streams=%d-%d",
				 rtp_s->transport.u.sctp.streams.RTP,
				 rtp_s->transport.u.sctp.streams.RTCP);
		break;
	default:
		break;
	}
	/*
	   //sprintf(temp, ";ssrc=%u", session->rtp_session->ssrc);
	   sprintf(temp, ";ssrc=%u", rtp_s->ssrc); // xxx
	   strcat(r, temp);
	 */
	w_pos += sprintf(r + w_pos, ";ssrc=%u", rtp_s->ssrc);
	// strcat(r, "\r\n\r\n");
	strcat(r, RTSP_EL RTSP_EL);

	bwrite(r, (unsigned short) strlen(r), rtsp);

	INFOLOGG("200 - %s ", descr->filename);

	return ERR_NOERROR;
}
