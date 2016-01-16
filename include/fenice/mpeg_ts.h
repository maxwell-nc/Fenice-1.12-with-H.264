

#ifndef _MPEG_TSH
#define _MPEG_TSH

#include <fenice/types.h>
#include <fenice/mediainfo.h>

int load_MP2T(media_entry * me);
int read_MPEG_ts(media_entry * me, uint8 * buffer, uint32 * buffer_size,
		 double *mtime, int *recallme, uint8 * marker);
int free_MP2T(void *stat);

#endif
