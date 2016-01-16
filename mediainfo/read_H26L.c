

#include <stdio.h>
#include <unistd.h>

#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/h26l.h>

int read_H26L(media_entry * me, uint8 * data, uint32 * data_size, double *mtime,
	      int *recallme, uint8 * marker)
{
	int ret;
	unsigned char intime[4];
	unsigned char h26l_header[12];
	static_H26L *s = NULL;

	*marker = 0;
	/* At this point it should be right to find the nearest lower frame */
	/* computing it from the value of mtime */
	if (!(me->flags & ME_FD)) {	/* and starting the reading from this */
		if ((ret = mediaopen(me)) < 0)
			return ret;
		s = (static_H26L *) calloc(1, sizeof(static_H26L));
		me->stat = (void *) s;
		s->pkt_sent = 0;
	} else
		s = (static_H26L *) me->stat;


	if (s->pkt_sent == 0) {
		lseek(me->fd, 0, SEEK_SET);
		read(me->fd, &(s->bufsize), 4);	/* The H26L RTP-Stream has 4 bytes for the size and 4 bytes for the intime */
		read(me->fd, &intime, 4);
		read(me->fd, &h26l_header, 12);	/* Then there is the RTP-Header */
		s->current_timestamp =
		    h26l_header[4] | (h26l_header[5] << 8) | (h26l_header[6] <<
							      16) |
		    (h26l_header[7] << 24);
	}

	*data_size = s->bufsize - 12;
	//*data=(unsigned char *)calloc(1,*data_size);
	//if (*data==NULL) {
	//        return ERR_ALLOC;
	//}
	if (read(me->fd, &(data[0]), *data_size) <= 0) {
		s->pkt_sent = 0;
		return ERR_EOF;
	}

	read(me->fd, &s->bufsize, 4);	/* reads next packet to have next time-stamp */
	read(me->fd, &intime, 4);
	read(me->fd, &h26l_header, 12);
	s->next_timestamp =
	    h26l_header[4] | (h26l_header[5] << 8) | (h26l_header[6] << 16) |
	    (h26l_header[7] << 24);

	if (s->current_timestamp == s->next_timestamp) {	/* If the time-stamps are the same then next-packet belongs to the same */
		*recallme = 1;	/* frame and the scheduler hasn't to wait to send it */
	} else {
		*recallme = 0;
	}

	if (*recallme == 0) {	/* to compute the new value of mtime for the next call of readH26L */
		me->description.delta_mtime =
		    ((s->next_timestamp -
		      s->current_timestamp) / 1000) * me->description.pkt_len;
	}

	*mtime = (s->current_timestamp * me->description.pkt_len) / 1000;	/* to compute the time-stamp */
	s->pkt_sent++;
	s->current_timestamp = s->next_timestamp;
	return ERR_NOERROR;
}
