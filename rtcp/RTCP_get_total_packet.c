
#include <fenice/rtcp.h>

unsigned int RTCP_get_total_packet(RTP_session * rtp_session)
{
	return rtp_session->rtcp_stats[i_server].pkt_count;
}
