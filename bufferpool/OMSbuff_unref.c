
#include <sys/mman.h>

#include <fenice/bufferpool.h>
#include <fenice/log.h>

void OMSbuff_unref(OMSConsumer * cons)
{
	if (cons) {
		OMSbuff_lock(cons->buffer);
		if (cons->buffer->control->refs > 0) {
			--(cons->buffer->control->refs);
//                      if ( msync(cons->buffer->control, sizeof(OMSControl), MS_ASYNC) )
//                              printf("*** msync error\n");
			OMSbuff_unlock(cons->buffer);
			//Now consumer has to read all unread slots
			while (!OMSbuff_gotreader(cons));
		} else
			OMSbuff_unlock(cons->buffer);

		DEBUGLOGG("Buffer ref (%d)", cons->buffer->control->refs);
		free(cons);
	}
}
