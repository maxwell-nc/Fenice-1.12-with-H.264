/*#if HAVE_ALLOCA_H
#include <alloca.h>
#endif*/

#include <string.h>
#include <stdlib.h>

#include <fenice/rtp.h>

RTP_session *RTP_session_destroy(RTP_session * session)
{
	RTP_session *next = session->next;
	OMSBuffer *buff = session->current_media->pkt_buffer;
	//struct stat fdstat;

	RTP_transport_close(session);
	//Release SD_flag using in multicast and unjoing the multicast group
	// if(session->sd_descr->flags & SD_FL_MULTICAST){

	// destroy consumer
	OMSbuff_unref(session->cons);
	if (session->current_media->pkt_buffer->control->refs == 0) {
		session->current_media->pkt_buffer = NULL;
		OMSbuff_free(buff);
		// close file if it's not a pipe
		//fstat(session->current_media->fd, &fdstat);
		//if ( !S_ISFIFO(fdstat.st_mode) )
		mediaclose(session->current_media);
	}
	// Deallocate memory
	free(session);

	return next;
}
