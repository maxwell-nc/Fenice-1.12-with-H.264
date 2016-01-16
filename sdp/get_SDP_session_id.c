

#include <stdio.h>

#include <fenice/sdp.h>
#include <fenice/utils.h>

char *get_SDP_session_id(char *buffer)
{
	buffer[0] = '\0';
	sprintf(buffer, "%.0f", NTP_time(time(NULL)));
	return buffer;
}
