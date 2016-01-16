#include <fenice/itu_h264.h>
#include <fenice/utils.h>
#include <fenice/log.h>
#include <include/fenice/en_xmalloc.h>
int load_h264(media_entry * p)
{
    TRACE_FUNC();
    int ret;
    static_h264 *s = NULL;
    if (!(p->flags & ME_FD)) {
        if ((ret = mediaopen(p)) < 0)
            return ret;
    }
    mediaclose(p);
    if (!(p->description.flags & MED_PKT_LEN)) {
        if (!(p->description.flags & MED_FRAME_RATE)) {
            return ERR_PARSE;
        }
        p->description.pkt_len =
                1 / (double) p->description.frame_rate * 1000;
        p->description.flags |= MED_PKT_LEN;
    }
    p->description.delta_mtime = p->description.pkt_len;

    if ((p->description.byte_per_pckt != 0)
        && (p->description.byte_per_pckt < 261)) {
        printf
                ("Warning: the max size for MPEG Video packet is smaller than 261 bytes and if a video header\n");
        printf("is greater the max size would be ignored \n");
        printf("Using Default \n");
    }
    return ERR_NOERROR;
}


