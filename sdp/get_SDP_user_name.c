

#include <fenice/sdp.h>
#include <string.h>

char *get_SDP_user_name(char *buffer)
{
	strcpy(buffer, "fenice");
	return buffer;
}
