
#include <stdlib.h>
#include <fenice/mediainfo.h>

int media_is_empty(media_entry * me)
{
	if (me == NULL) {
		return 1;
	}
	if (!(me->flags & ME_FILENAME)) {	//non c'� il nome del file = non � stato specificato il media 
		return 1;
	}
	return 0;
}
