

#include <fenice/utils.h>

float NTP_time(time_t t)
{
	return (float) t + 2208988800U;
}
