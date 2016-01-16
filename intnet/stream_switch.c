
#include <fenice/intnet.h>
#include <fenice/utils.h>

int stream_switch(RTP_session * changing_session, media_entry * new_media)
{
	close(changing_session->current_media->fd);
	changing_session->current_media->flags &= ~ME_FD;
	new_media->pkt_buffer = changing_session->current_media->pkt_buffer;
	changing_session->current_media = new_media;

	return ERR_NOERROR;
}
