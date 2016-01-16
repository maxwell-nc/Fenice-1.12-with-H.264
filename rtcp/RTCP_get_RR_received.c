
#include <fenice/rtcp.h>

unsigned int RTCP_get_RR_received(RTP_session * rtp_session)
{
	return rtp_session->rtcp_stats[i_client].RR_received;
}
