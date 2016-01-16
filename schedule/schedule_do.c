
#include <stdio.h>
#include <sys/time.h>

#include <fenice/schedule.h>
#include <fenice/intnet.h>
#include <fenice/utils.h>
#include <fenice/log.h>

extern int stop_schedule;

void *schedule_do(void *nothing)
{
	int i = 0, res = ERR_GENERIC;
	struct timeval now;
	double mnow;
	// Fake timespec for fake nanosleep. See below.
	// struct timespec ts = { 0, 0 };
	do {
		// Fake waiting. Break the while loop to achieve fair kernel (re)scheduling and fair CPU loads.
		// See also main.c
		//nanosleep(&ts, NULL);

		//printf("[%s:%d]Running to here\n", __FILE__, __LINE__);

		SCHEDULE_LOCK(&sched_control.lock);
		while (0 == sched_control.client_count) {
			SCHEDULE_WAIT(&sched_control.cond, &sched_control.lock);
		}
		SCHEDULE_UNLOCK(&sched_control.lock);

		//printf("[%s:%d]Running to here\n", __FILE__, __LINE__);

		for (i = 0; i < ONE_FORK_MAX_CONNECTION; ++i) {
			if (sched[i].valid) {
				if (!sched[i].rtp_session->pause) {
					gettimeofday(&now, NULL);
					mnow = (double) now.tv_sec * 1000 + (double) now.tv_usec / 1000;

					if (mnow >=sched[i].rtp_session->current_media->mstart) {
						if (mnow - sched[i].rtp_session->mprev_tx_time >=
						    sched[i].rtp_session->current_media->description.pkt_len) {
							/*if (mnow-sched[i].rtp_session->mtime>=sched[i].rtp_session->current_media->description.pkt_len) {    old scheduler */

							stream_change(sched[i].rtp_session, change_check(sched[i].rtp_session));

							/*This operation is in RTP_send_packet function because it runs only if producer writes the slot */
							//sched[i].rtp_session->mtime += sched[i].rtp_session->current_media->description.delta_mtime; //emma  
							//sched[i].rtp_session->mtime+=sched[i].rtp_session->current_media->description.pkt_len;     // old scheduler

							RTCP_handler(sched[i].rtp_session);
							/*if RTCP_handler return ERR_GENERIC what do i have to do? */

							// Send an RTP packet
							res = sched[i].play_action(sched[i].rtp_session);
							if (res != ERR_NOERROR) {
								if (res == ERR_EOF) {
									if ((sched[i].rtp_session)->current_media->description.msource == live) {
										WARNLOGG("Live source empty!\n");
									} else {
										INFOLOGG("Stream Finished\n");
										schedule_stop(i);
									}
								} else if (res == ERR_ALLOC) {
									FATALLOGG("Upss, FATAL ERROR ALLOC!!\n");
									schedule_stop(i);
								} else
									WARNLOGG("Packet Lost");
									/*continue; */
							}
							sched[i].rtp_session->mprev_tx_time += 
                            sched[i].rtp_session->current_media->description.pkt_len;
						}
					}
				}


			} else if (sched[i].rtp_session) {
				if (sched[i].rtp_session->is_multicast_dad) {	/*unicast always is a multicast_dad */
					// fprintf(stderr, "rtp session not valid, but still present...\n");
					RTP_session_destroy(sched[i].rtp_session);
					sched[i].rtp_session = NULL;
					INFOLOGG("rtp session closed");
				}
			}
		}
	} while (!stop_schedule);
	stop_schedule = 0;
	return ERR_NOERROR;
}
