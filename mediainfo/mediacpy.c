
#include <stdio.h>
#include <string.h>
#include <fenice/utils.h>
#include <fenice/log.h>
//#include <fenice/mediainfo.h>
//#include <fenice/bufferpool.h>
//#include <fenice/prefs.h>

int mediacpy(media_entry ** media_out, media_entry ** media_in)
{

	uint32 dim_buff;

	TRACE_FUNC();

	switch ((*media_in)->description.mtype) {
	case audio:
		dim_buff = DIM_AUDIO_BUFFER;
		break;
	case video:
		dim_buff = DIM_VIDEO_BUFFER;
		break;
	default:
		return ERR_GENERIC;
	}

	if ((*media_in)->description.msource == live)
		*media_out = *media_in;
	else
		memcpy((*media_out), (*media_in), sizeof(media_entry));

	if ((*media_out)->pkt_buffer == NULL) {
		if (!strcasecmp
		    ((*media_out)->description.encoding_name, "RTP_SHM")) {
			if (!
			    ((*media_out)->pkt_buffer =
			     OMSbuff_shm_map((*media_out)->filename)))
				return ERR_ALLOC;
		} else {
			(*media_out)->pkt_buffer = OMSbuff_new(dim_buff);
			//if (!((*media_out)->pkt_buffer = OMSbuff_new(dim_buff)))
			//	return ERR_ALLOC;
		}
	}

	/*if (((*media_out)->cons = OMSbuff_ref((*media_out)->pkt_buffer)) == NULL)
	   return ERR_ALLOC; */

	return ERR_NOERROR;
}
