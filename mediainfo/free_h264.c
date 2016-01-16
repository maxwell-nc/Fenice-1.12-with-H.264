#include <fenice/itu_h264.h>
#include <fenice/utils.h>
#include <fenice/types.h>

int free_h264(void *stat)
{
    static_h264 *s;
    if(stat != NULL) {
        s = (static_h264*)stat;
        free(s->n->buf);
        free(s->n);
        free(s);
        s = NULL;
    }

    return ERR_NOERROR;
}
