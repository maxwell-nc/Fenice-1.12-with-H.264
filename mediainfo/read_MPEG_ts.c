
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/mpeg_ts.h>

int read_MPEG_ts(media_entry * me, uint8 * data_slot, uint32 * data_size,
		 double *mtime, int *recallme, uint8 * marker)
{
	int ret;
	uint32 num_bytes;
	uint8 *data;

	*marker = *recallme = 0;
	num_bytes = ((me->description).byte_per_pckt / 188) * 188;
#if HAVE_ALLOCA
	data = (unsigned char *) alloca(num_bytes + 4);
#else
	data = (unsigned char *) calloc(1, num_bytes + 4);
#endif
	if (data == NULL)
		return ERR_ALLOC;

	if (!(me->flags & ME_FD)) {
		if ((ret = mediaopen(me)) < 0) {
#if !HAVE_ALLOCA
			free(data);
#endif
			return ret;
		}
		*data_size = 0;
	}

	*data_size = read(me->fd, data, num_bytes);
	if (*data_size <= 0) {
#if !HAVE_ALLOCA
		free(data);
#endif

		return ERR_EOF;
	}
	memcpy(data_slot, data, *data_size);
#if !HAVE_ALLOCA
	free(data);
#endif

	return ERR_NOERROR;
}
