
#ifndef _MP3H
#define _MP3H

#include <fenice/types.h>
#include <fenice/mediainfo.h>

int load_MPA(media_entry * me);
int read_MP3(media_entry * me, uint8 * buffer, uint32 * buffer_size,
	     double *mtime, int *recallme, uint8 * marker);
int free_MPA(void *stat);

#endif
