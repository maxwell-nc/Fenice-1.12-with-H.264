
// #include <time.h>
#include <sys/time.h>

#include <fenice/schedule.h>
#include <fenice/utils.h>


int schedule_resume(int id, play_args * args)
{
	struct timeval now;
	double mnow;
	media_entry *current_media = sched[id].rtp_session->current_media;

	SCHEDULE_LOCK(&sched_control.lock);

	gettimeofday(&now, NULL);

	mnow = (double) now.tv_sec * 1000 + (double) now.tv_usec / 1000;
	current_media->mstart_offset +=
	    current_media->mtime -
	    current_media->mstart +
	    (double)current_media->description.pkt_len;


	if (args->start_time_valid)
		current_media->play_offset = args->start_time * 1000;	/*TODO:Federico. For Random Access */

	current_media->mstart = mnow;
	current_media->mtime =
	    sched[id].rtp_session->mprev_tx_time =
	    mnow -
	    (double)current_media->description.pkt_len;
	sched[id].rtp_session->pause = 0;

	SCHEDULE_UNLOCK(&sched_control.lock);

	return ERR_NOERROR;
}
