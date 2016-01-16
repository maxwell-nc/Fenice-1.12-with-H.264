#ifndef GECOMS_ITU_H264_H
#define GECOMS_ITU_H264_H

#include <stdio.h>
#include <fenice/types.h>
#include <fenice/mediainfo.h>


#define PACKET_BUFFER_END      (unsigned int)0x00000000

#define MAX_RTP_PKT_LENGTH     1400

//#define DEST_IP                "192.168.0.25"
//#define DEST_PORT               1234

#define H264                    96
#define UDP_MAX_SIZE            1400

#define H264_SHARDING_PACKET    1
#define H264_NOT_SHARDING       0

#define TYPE_FU_A                28
#define MAX_FRAME_ZISE           100000

typedef struct {
#if (BYTE_ORDER == LITTLE_ENDIAN)
    unsigned char type:5;
    unsigned char nri:2;
    unsigned char f:1;
#elif (BYTE_ORDER == BIG_ENDIAN)
    unsigned char f:1;
    unsigned char nri:2;
    unsigned char type:5;
#endif
}NALU_header;

typedef struct {
#if (BYTE_ORDER == LITTLE_ENDIAN)
    unsigned char type:5;
    unsigned char nri:2;
    unsigned char f:1;
#elif (BYTE_ORDER == BIG_ENDIAN)
    unsigned char f:1;
    unsigned char nri:2;
    unsigned char type:5;
#endif
}FU_indicator;

typedef struct {
#if (BYTE_ORDER == LITTLE_ENDIAN)
    unsigned char type:5;
    unsigned char r:1;
    unsigned char e:1;
    unsigned char s:1;
#elif (BYTE_ORDER == BIG_ENDIAN)
    unsigned char s:1;
    unsigned char e:1;
    unsigned char r:1;
    unsigned char type:5;
#endif
}FU_header;

typedef struct {
    int startcodeprefix_len;
    unsigned int len;
    unsigned max_size;
    unsigned char forbidden_bit;
    unsigned char nal_reference_idc;
    unsigned char nal_unit_type;
    char *buf;
    unsigned short lost_packets;
} NALU_t;

typedef struct {
    unsigned int sharding_packet;
    NALU_t *n;
    unsigned int packet_num;
    unsigned int last_packet_byte;
    unsigned int current_packet;
    FILE *f;
}static_h264;


static_h264* init_static_h264(media_entry * me);
int load_h264(media_entry * me);
int read_h264(media_entry * me, uint8 * buffer, uint32 * buffer_size,
                    double *mtime, int *recallme, uint8 * marker);
int free_h264(void *stat);
#endif //GECOMS_ITU_H264_H

