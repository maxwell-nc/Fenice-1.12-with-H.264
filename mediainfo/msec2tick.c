
#include <fenice/utils.h>

long msec2tick(double mtime, media_entry * me)
{
	return (me->description.flags & MED_CLOCK_RATE) ? me->description.
	    clock_rate * mtime / 1000 : ERR_GENERIC;
	/*
	   if (me->description.flags & MED_CLOCK_RATE)
	   return me->description.clock_rate*mtime/1000;
	   else
	   return ERR_GENERIC;
	 */
}
