

#ifndef _MPEG_SYSTEMH
#define _MPEG_SYSTEMH

#include <fenice/types.h>
#include <fenice/mediainfo.h>

typedef struct {
	unsigned int msb;
	unsigned int scr;
} SCR;


typedef struct {
	unsigned int msb;
	unsigned int pts;
} PTS;

typedef struct {
	unsigned char final_byte;
	PTS pts;
	PTS next_pts;
	PTS dts;
	PTS next_dts;
	PTS pts_audio;
	SCR scr;
	unsigned int data_total;
	int offset_flag;
	int offset;
	int new_dts;
} static_MPEG_system;

	/* reads pack header */
int read_pack_head(uint8 *, uint32 * e, int fin, unsigned char *final_byte,
		   SCR * scr);
	/* reads packet header */
int read_packet_head(uint8 *, uint32 *, int fin, unsigned char *final_byte,
		     int *time_set, PTS * pts, PTS * dts, int *dts_present,
		     PTS * pts_audio);
	/* reads a packet */
int read_packet(uint8 *, uint32 *, int fin, unsigned char *final_byte);

	/*TODO: load_MPEGSYSTEM */
int read_MPEG_system(media_entry * me, uint8 * buffer, uint32 * buffer_size,
		     double *mtime, int *recallme, uint8 * marker);


#endif
