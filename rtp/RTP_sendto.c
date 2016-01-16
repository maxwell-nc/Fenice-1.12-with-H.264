
#include <fenice/rtp.h>
#include <fenice/log.h>

ssize_t RTP_sendto(
		RTP_session * session,
		rtp_protos proto,
		unsigned char *pkt,
		ssize_t pkt_size)
{
	ssize_t sent = -1;

	tsocket fd =
	    (proto ==
	     rtp_proto) ? session->transport.rtp_fd : session->transport.
	    rtcp_fd_out;
	struct sockaddr *peer =
	    (proto ==
	     rtp_proto) ? &(session->transport.u.udp.rtp_peer) : &(session->
								   transport.u.
								   udp.
								   rtcp_out_peer);
	socklen_t peer_len =
	    (proto ==
	     rtp_proto) ? sizeof(session->transport.u.udp.
				 rtp_peer) : sizeof(session->transport.u.udp.
						    rtcp_out_peer);

	if (fd < 0)
		return -1;

	switch (session->transport.type) {
		case RTP_rtp_avp:
			sent = sendto(fd, pkt, pkt_size, 0, peer, peer_len);
			break;
		case RTP_rtp_avp_tcp:
			sent = send(fd, pkt, pkt_size, MSG_DONTWAIT | MSG_EOR);
			break;
		case RTP_rtp_avp_sctp:
			break;
		default:
			break;
	}
	if (sent < 0) {
		ERRORLOGG("sendto() failed [%s]", strerror(errno));
	}

	return sent;
}
