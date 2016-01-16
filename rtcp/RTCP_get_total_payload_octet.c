
#include <fenice/rtcp.h>

unsigned int RTCP_get_total_payload_octet(RTP_session * rtp_session)
{
	return rtp_session->rtcp_stats[i_server].octet_count;

}
