
#include <fenice/rtcp.h>

int RTCP_get_pkt_lost(RTP_session * rtp_session)
{
	return rtp_session->rtcp_stats[i_client].pkt_lost;
}
