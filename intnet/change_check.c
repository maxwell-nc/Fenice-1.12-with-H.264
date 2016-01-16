

#include <fenice/intnet.h>
#include <fenice/log.h>


int change_check(RTP_session * changing_session)
{
	unsigned int res;

	if ((res = RTCP_get_RR_received(changing_session)) ==
	    changing_session->PreviousCount) {
		return 0;
	}
	changing_session->PreviousCount = res;

	DEBUGLOGG("[intnet] RTCP received = %d, Fract lost = %f",
		res, RTCP_get_fract_lost(changing_session));

	if (RTCP_get_fract_lost(changing_session) > 0.03) {
		return -2;
	} else if (RTCP_get_jitter(changing_session) > 10) {
		return -1;
	} else if ((RTCP_get_fract_lost(changing_session) < 0.02)
		   && (RTCP_get_jitter(changing_session) < 10)) {
		return 1;
	} else {
		return 0;
	}
}
