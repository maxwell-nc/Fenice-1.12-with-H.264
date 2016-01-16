
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/mpeg.h>
#include <fenice/en_xmalloc.h>

#define DEFAULT_BYTE_X_PKT 1400


int read_MPEG_video(
		media_entry * me,
		uint8 * data,
		uint32 * data_size,
		double *mtime,
		int *recallme,
		uint8 * marker)
{				/* reads MPEG-1,2 Video */
	int ret;
	uint32 num_bytes;
	char *vsh1_1;
	uint32 count, flag = 0, seq_head_pres = 0;
	unsigned char *data_tmp;
	static_MPEG_video *s = NULL;
	uint32 wasSeeking = 0;

	if (!(me->flags & ME_FD)) {
		if ((ret = mediaopen(me)) < 0)
			return ret;
		s = xnew0(static_MPEG_video);
		me->stat = (void *) s;
		s->final_byte = 0x00;
		s->std = TO_PROBE;
		s->fragmented = 0;
	} else
		s = (static_MPEG_video *) me->stat;

	num_bytes =
	    ((me->description).byte_per_pckt >
	     261) ? (me->description).byte_per_pckt : DEFAULT_BYTE_X_PKT;

	data_tmp = xmalloc(65000); // (unsigned char *) calloc(1, 65000);

	if (s->std == MPEG_2) {	/*the first time is TO_PROBE */
		*data_size = 4;
	} else {
		*data_size = 4;
	}


	if (s->std == TO_PROBE) {
		probe_standard(me, data_tmp, data_size, me->fd, &s->std);
		flag = 1;
		seq_head_pres = 1;
	}

	/*---- Random Access ----*/
	if ((me->description).msource != live
	    && me->play_offset != me->prev_mstart_offset) {
		if (!me->description.bitrate) {
			//Bit Rate unavaible in sequence header, so i calculate it from scratch
			me->description.bitrate = (int)(s->data_total * 8 / (*mtime)) * 1000;
		} else {
			s->fragmented = 0;
			wasSeeking = 1;
			if (!flag) {
				lseek(me->fd, 0, SEEK_SET);
				probe_standard(me, data_tmp, data_size, me->fd, &s->std);
				seq_head_pres = 1;
			}
		}
		count = random_access(me);
		lseek(me->fd, count, SEEK_SET);
		me->prev_mstart_offset = me->play_offset;
	}
	/*---- end Random Access ----*/
	if (!s->fragmented) {	/*num_bytes !=0 and slice was not fragmented */
		char buf_aux[3];
		int i;
		if (flag && wasSeeking == 0)
			s->final_byte = data_tmp[*data_size - 1];
		else {
			data_tmp[*data_size] = 0x00;
			*data_size += 1;
			data_tmp[*data_size] = 0x00;
			*data_size += 1;
			data_tmp[*data_size] = 0x01;
			*data_size += 1;
			data_tmp[*data_size] = s->final_byte;
			*data_size += 1;
		}
		if (s->final_byte == 0xb7) {
			if (next_start_code(data_tmp, data_size, me->fd) == -1) {	/* If there aren't 3 more bytes we are at EOF */
				xfree(data_tmp);
				return ERR_EOF;
			}
			if (read(me->fd, &s->final_byte, 1) < 1) {
				xfree(data_tmp);
				return ERR_EOF;
			}
			data_tmp[*data_size] = s->final_byte;
			*data_size += 1;
		}
		*recallme = 1;
		while ((s->final_byte > 0xAF || s->final_byte == 0x00)
		       && *recallme) {
			if (s->final_byte == 0xb3) {
				read_seq_head(me, data_tmp, data_size, me->fd,
					      &s->final_byte, s->std);
				seq_head_pres = 1;
			}

			if (s->final_byte == 0xb8) {
				read_gop_head(data_tmp, data_size, me->fd,
					      &s->final_byte, &s->hours,
					      &s->minutes, &s->seconds,
					      &s->picture, s->std);
			}

			if (s->final_byte == 0x00) {
				read_picture_head(data_tmp, data_size, me->fd,
						  &s->final_byte, &s->temp_ref,
						  &s->vsh1, s->std);
				if (s->std == MPEG_2 && *data_size < num_bytes) {
					read_picture_coding_ext(data_tmp,
								data_size,
								me->fd,
								&s->final_byte,
								&s->vsh2);
				}
			}
			if (s->final_byte == 0xb7) {	/*sequence end code */
				*recallme = 0;
				s->fragmented = 0;
			}
			while (s->final_byte == 0xb2 || s->final_byte == 0xb5) {
				if (next_start_code(data_tmp, data_size, me->fd)
				    < 0) {
					xfree(data_tmp);
					return ERR_EOF;
				}
				if (read(me->fd, &s->final_byte, 1) < 1) {
					xfree(data_tmp);
					return ERR_EOF;
				}
				data_tmp[*data_size] = s->final_byte;
				*data_size += 1;
			}
		}
		//      *data_size-=4;
		while (num_bytes > *data_size && *recallme) {
			//              *data_size+=4;
			if (read(me->fd, &buf_aux, 3) < 3) {	/* If there aren't 3 more bytes we are at EOF */
				// close(me->fd);
				xfree(data_tmp);
				return ERR_EOF;
			}
			while (!((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
				&& (buf_aux[2] == 0x01)) && *data_size < num_bytes) {
				data_tmp[*data_size] = buf_aux[0];
				*data_size += 1;
				buf_aux[0] = buf_aux[1];
				buf_aux[1] = buf_aux[2];
				if (read(me->fd, &buf_aux[2], 1) < 1) {
					// close(me->fd);
					xfree(data_tmp);
					return ERR_EOF;
				}
			}
			for (i = 0; i < 3; i++) {
				data_tmp[*data_size] = buf_aux[i];
				*data_size += 1;
			}
			if (read(me->fd, &s->final_byte, 1) < 1) {
				xfree(data_tmp);
				return ERR_EOF;
			}
			data_tmp[*data_size] = s->final_byte;
			*data_size += 1;
			if (((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
			     && (buf_aux[2] == 0x01))) {
				if (((s->final_byte > 0xAF) || (s->final_byte == 0x00))) {	/*TODO: 0xb7 */
					*recallme = 0;
					//      *data_size-=4;
				} else
					*recallme = 1;
				s->fragmented = 0;
			} else {
				*recallme = 1;
				s->fragmented = 1;
			}
		}		/*end while *data_size < num_bytes && *recallme */
		s->vsh1.b = 1;
		if (((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
		     && (buf_aux[2] == 0x01)))
			*data_size -= 4;
	} /*end else num_bytes!=0 and slice was not fragmented */
	else {			/*num_bytes!=0 and slice was fragmented */
		char buf_aux[3];
		int i;

		if (read(me->fd, &buf_aux, 3) < 3) {	/* If there aren't 3 more bytes we are at EOF */
			// close(me->fd);
			xfree(data_tmp);
			return ERR_EOF;
		}
		while (!((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
			&& (buf_aux[2] == 0x01)) && *data_size < num_bytes) {
			data_tmp[*data_size] = buf_aux[0];
			*data_size += 1;
			buf_aux[0] = buf_aux[1];
			buf_aux[1] = buf_aux[2];
			if (read(me->fd, &buf_aux[2], 1) < 1) {
				// close(me->fd);
				xfree(data_tmp);
				return ERR_EOF;
			}
		}
		for (i = 0; i < 3; i++) {
			data_tmp[*data_size] = buf_aux[i];
			*data_size += 1;
		}
		if (read(me->fd, &s->final_byte, 1) < 1) {
			xfree(data_tmp);
			return ERR_EOF;
		}
		data_tmp[*data_size] = s->final_byte;
		*data_size += 1;

		if (((buf_aux[0] == 0x00) && (buf_aux[1] == 0x00)
		     && (buf_aux[2] == 0x01))) {
			if (((s->final_byte > 0xAF) || (s->final_byte == 0x00))) {	/*TODO: 0xb7 */
				*recallme = 0;
			} else
				*recallme = 1;
			s->fragmented = 0;
			*data_size -= 4;
		} else {
			*recallme = 1;
			s->fragmented = 1;
		}


		s->vsh1.b = 0;
	}
	if (me->description.msource != live)
		*mtime =
		    (s->hours * 3.6e6) + (s->minutes * 6e4) +
		    (s->seconds * 1000) + (s->temp_ref * 40) +
		    (s->picture * 40);
	s->data_total += *data_size;
	if (s->std == MPEG_2 && !flag) {
		s->data_total -= 4;
		s->vsh1.t = 0;
	} else {
		s->data_total -= 4;
		s->vsh1.t = 0;
	}
	s->vsh1.mbz = 0;
	s->vsh1.an = 0;
	s->vsh1.n = 0;
	if (seq_head_pres) {
		s->vsh1.s = 1;
	} else {
		s->vsh1.s = 0;
	}
//        s->vsh1.b=1;
	if (!s->fragmented) {
		s->vsh1.e = 1;
	} else {
		s->vsh1.e = 0;
	}
	vsh1_1 = (char *) (&s->vsh1);	/* to see the struct as a set of bytes */
	data_tmp[0] = vsh1_1[3];
	data_tmp[1] = vsh1_1[2];
	data_tmp[2] = vsh1_1[1];
	data_tmp[3] = vsh1_1[0];
	flag = 0;		/*so it is just probed */
	memcpy(data, data_tmp, *data_size);
	xfree(data_tmp);
	*marker = !(*recallme);

	return ERR_NOERROR;
}
