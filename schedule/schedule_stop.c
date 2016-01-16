
#include <fenice/schedule.h>
#include <fenice/rtcp.h>
#include <fenice/log.h>

void schedule_stop(int id)
{
	TRACE_FUNC();

	sched[id].rtp_session->pause = 1;
	sched[id].rtp_session->started = 0;
	//sched[id].rtsp_session->cur_state=READY_STATE;

	RTCP_send_packet(sched[id].rtp_session, SR);
	RTCP_send_packet(sched[id].rtp_session, BYE);
}
