
// #include <time.h>
#include <sys/time.h>

#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/schedule.h>
#include <fenice/log.h>


int32 schedule_start(int id, play_args * args)
{
	struct timeval now;
	double mnow;
	OMSConsumer *cons;
	media_entry* current_media = sched[id].rtp_session->current_media;

	TRACE_FUNC();

	SCHEDULE_LOCK(&sched_control.lock);

	gettimeofday(&now, NULL);
	mnow = (double) now.tv_sec * 1000 + (double) now.tv_usec / 1000;
	cons = OMSbuff_ref(current_media->pkt_buffer);
	sched[id].rtp_session->cons = cons;

	if (current_media->pkt_buffer->control->refs == 1) {
		/*If and only if this session is the first session related to this media_entry, then it runs here */
		if (!args->playback_time_valid) {
			current_media->mstart = mnow;
		} else {
			current_media->mstart = mktime(&(args->playback_time));
		}

		current_media->mtime = mnow;

		current_media->mstart_offset = args->start_time * 1000;
		current_media->play_offset = args->start_time * 1000;	/*TODO:chicco. For Random Access */
	}
	sched[id].rtp_session->mprev_tx_time = mnow;
	sched[id].rtp_session->pause = 0;
	sched[id].rtp_session->started = 1;
	sched[id].rtp_session->MinimumReached = 0;
	sched[id].rtp_session->MaximumReached = 0;
	sched[id].rtp_session->PreviousCount = 0;
	sched[id].rtp_session->rtcp_stats[i_client].RR_received = 0;
	sched[id].rtp_session->rtcp_stats[i_client].SR_received = 0;

	SCHEDULE_UNLOCK(&sched_control.lock);

	return ERR_NOERROR;
}
