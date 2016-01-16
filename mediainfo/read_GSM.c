
#include <unistd.h>
#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/gsm.h>

int read_GSM(media_entry * me, uint8 * data, uint32 * data_size, double *mtime,
	     int *recallme, uint8 * marker)
{
	unsigned char byte1;
	unsigned int N = 0, res;
	int ret;
	// long frame_skip;
	unsigned int frame_skip;

	*marker = 0;
	*recallme = 0;
	if (!(me->flags & ME_FD)) {
		if ((ret = mediaopen(me)) < 0)
			return ret;
		me->data_chunk = 0;
	}

	frame_skip = (long) lround(*mtime / (double) me->description.pkt_len);
	*mtime = (double) frame_skip *(double) (me->description.pkt_len);
	for (; me->data_chunk < frame_skip; ++me->data_chunk) {
		if ((read(me->fd, &byte1, 1)) != 1)
			return ERR_EOF;
		switch (byte1 & 0x07) {
		case 0:
			N = 12;
			break;
		case 1:
			N = 13;
			break;
		case 2:
			N = 15;
			break;
		case 3:
			N = 17;
			break;
		case 4:
			N = 18;
			break;
		case 5:
			N = 20;
			break;
		case 6:
			N = 25;
			break;
		case 7:
			N = 30;
			break;
		}
		if ((me->data_chunk + 1) < frame_skip) {
			lseek(me->fd, N, SEEK_CUR);
		}
	}
	*data_size = N + 1;
	//*data=(unsigned char *)calloc(1,*data_size);
	//if (*data==NULL) {
	//       return ERR_ALLOC;
	//}
	data[0] = byte1;
	if ((res = read(me->fd, &(data[1]), *data_size - 1)) <= *data_size - 1) {
		if (res <= 0)
			return ERR_EOF;
		else
			*data_size = res + 1;
	}
	return ERR_NOERROR;
}
