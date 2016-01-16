
#include <fenice/rtcp.h>

unsigned int RTCP_get_jitter(RTP_session * rtp_session)
{
	return rtp_session->rtcp_stats[i_client].jitter;
}
