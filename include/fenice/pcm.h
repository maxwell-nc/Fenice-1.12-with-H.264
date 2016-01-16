
#ifndef _PCMH
#define _PCMH

#include <fenice/types.h>
#include <fenice/mediainfo.h>

int load_L16(media_entry * me);
int read_PCM(media_entry * me, uint8 * buffer, uint32 * buffer_size,
	     double *mtime, int *recallme, uint8 * marker);
int free_L16(void *stat);

#endif
