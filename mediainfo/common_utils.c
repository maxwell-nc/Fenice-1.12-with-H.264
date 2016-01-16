
#include <stdio.h>
#include <unistd.h>

#include <fenice/types.h>
#include <fenice/utils.h>	/*TODO: return ERR_X */

int next_start_code(uint8 * buf, uint32 * buf_size, int fin)
{
	char buf_aux[3];
	int i;
	unsigned int count = 0;
	if (read(fin, &buf_aux, 3) < 3) {	/* If there aren't 3 more bytes we are at EOF */
		return -1;
	}
	while (!
	       ((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
		&& (buf_aux[2] == 0x01))) {
		buf[*buf_size] = buf_aux[0];
		*buf_size += 1;
		buf_aux[0] = buf_aux[1];
		buf_aux[1] = buf_aux[2];
		if (read(fin, &buf_aux[2], 1) < 1) {
			return -1;
		}
		count++;
	}
	for (i = 0; i < 3; i++) {
		buf[*buf_size] = buf_aux[i];
		*buf_size += 1;
	}
	return count;
}

uint32 random_access(media_entry * me)
{
	return me->description.bitrate / 8 * me->play_offset / 1000;
}

int changePacketLength(float offset, media_entry * me)
{
	(me->description).pkt_len = offset;
	return 1;
}
