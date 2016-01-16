
#include <fenice/schedule.h>
#include <fenice/utils.h>


schedule_list sched[ONE_FORK_MAX_CONNECTION];

schedule_control_t sched_control;

int schedule_init()
{
	int i;
	pthread_t thread;
	
	pthread_mutex_init(&sched_control.lock, NULL);
	pthread_cond_init(&sched_control.cond, NULL);
	sched_control.client_count = 0;

	for (i = 0; i < ONE_FORK_MAX_CONNECTION; ++i) {
		sched[i].rtp_session = NULL;
		sched[i].play_action = NULL;
		sched[i].valid = 0;
	}
	pthread_create(&thread, NULL, schedule_do, 0);
	return ERR_NOERROR;
}
