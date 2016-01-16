
#include <fenice/schedule.h>
#include <fenice/utils.h>
#include <fenice/log.h>

int schedule_add(RTP_session * rtp_session)
{
	int i;

	TRACE_FUNC();

	SCHEDULE_LOCK(&sched_control.lock);

	for (i = 0; i < ONE_FORK_MAX_CONNECTION; ++i) {
		if (!sched[i].valid) {
			sched[i].valid = 1;
			sched[i].rtp_session = rtp_session;
			++sched_control.client_count;
			if (rtp_session->is_multicast_dad)
				sched[i].play_action = RTP_send_packet;
			SCHEDULE_UNLOCK(&sched_control.lock);
			SCHEDULE_WAKE(&sched_control.cond);
			return i;
		}
	}

	SCHEDULE_UNLOCK(&sched_control.lock);
	// if (i >= MAX_SESSION) {
	return ERR_GENERIC;
	// }
}
