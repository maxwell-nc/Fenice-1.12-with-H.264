
#ifndef _SCHEDULEH
#define _SCHEDULEH


#include <time.h>
#include <fenice/types.h>
#include <fenice/rtp.h>
#include <fenice/prefs.h>
#include <fenice/debug.h>


typedef struct _play_args {
	struct tm playback_time;
	short playback_time_valid;
	float start_time;	// In secondi, anche frazionari
	short start_time_valid;
	float end_time;
} play_args;

typedef struct _schedule_list {
	int valid;
	RTP_session *rtp_session;
	//RTSP_session *rtsp_session;
	RTP_play_action play_action;
} schedule_list;

typedef struct _schedule_control {
	size_t client_count;
	pthread_mutex_t lock;
	pthread_cond_t cond;
} schedule_control_t;


extern schedule_control_t sched_control;
extern schedule_list sched[ONE_FORK_MAX_CONNECTION];

int schedule_init();

void *schedule_do(void *nothing);

int schedule_add(RTP_session * rtp_session /*,RTSP_session *rtsp_session */ );
int32 schedule_start(int id, play_args * args);
void schedule_stop(int id);
int schedule_remove(int id);
int schedule_resume(int id, play_args * args);

#define SCHEDULE_LOCK(lock) pthread_mutex_lock((lock))
#define SCHEDULE_UNLOCK(lock) pthread_mutex_unlock((lock))
#define SCHEDULE_WAIT(cond, lock) pthread_cond_wait((cond), (lock))
#define SCHEDULE_WAKE(cond) pthread_cond_signal(cond)

#endif
