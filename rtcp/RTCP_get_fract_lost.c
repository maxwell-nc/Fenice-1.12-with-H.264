
#include <fenice/rtcp.h>

float RTCP_get_fract_lost(RTP_session * rtp_session)
{
	float value;
	value = (float) (rtp_session->rtcp_stats[i_client].fract_lost) / 256;
	return value;
}
