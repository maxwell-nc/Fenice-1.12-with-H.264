

#include <fenice/rtp.h>

ssize_t RTP_recv(RTP_session * session, rtp_protos proto)
{
	switch (session->transport.type) {
	case RTP_rtp_avp:{
			socklen_t peer_len =
			    sizeof(session->transport.u.udp.rtcp_in_peer);
			session->rtcp_insize =
			    recvfrom(session->transport.rtcp_fd_in,
				     session->rtcp_inbuffer,
				     sizeof(session->rtcp_inbuffer), 0,
				     &(session->transport.u.udp.rtcp_in_peer),
				     &peer_len);
			break;
		}
	case RTP_rtp_avp_tcp:	// usually not used because we receive rtcp pkts through rtsp socket, 
		// but implemented for completeness.
		session->rtcp_insize =
		    recv(session->transport.rtcp_fd_in, session->rtcp_inbuffer,
			 sizeof(session->rtcp_inbuffer), 0);
		break;
	case RTP_rtp_avp_sctp:	// not usable due to stream demultiplexing
		break;
	default:
		break;
	}


	return session->rtcp_insize;
}
