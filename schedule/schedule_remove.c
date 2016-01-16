#include <fenice/schedule.h>
#include <fenice/utils.h>


int schedule_remove(int id)
{
	SCHEDULE_LOCK(&sched_control.lock);

	sched[id].valid = 0;
	--sched_control.client_count;

	SCHEDULE_UNLOCK(&sched_control.lock);

	return ERR_NOERROR;
}
