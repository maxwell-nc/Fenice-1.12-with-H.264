
#include <stdlib.h>

#include <fenice/bufferpool.h>
#include <fenice/en_xmalloc.h>
#include <fenice/log.h>

void OMSbuff_free(OMSBuffer * buffer)
{
	TRACE_FUNC();

	switch (buffer->type) {
		case buff_shm:
			OMSbuff_shm_unmap(buffer);
			DEBUGLOGG("Buffer in SHM unmapped");
			break;
		case buff_local:{
			pthread_mutex_destroy(&buffer->control->syn);
			xfree(buffer->control);
			free(buffer->slots);
			xfree(buffer);
			DEBUGLOGG("Buffer is freed");
			break;
		}
		default:
			break;
	}

}
