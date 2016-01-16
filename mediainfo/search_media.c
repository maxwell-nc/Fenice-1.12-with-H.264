

#include <stdio.h>
#include <string.h>
#include <fenice/mediainfo.h>
#include <fenice/log.h>

// return NULL if not found.
media_entry *search_media(media_entry * req, media_entry * list)
{
	media_entry *p;
	char *m_filename;

	for (p = list; p != NULL; p = p->next) {

		if (req->flags & ME_FILENAME) {
			m_filename = strrchr(req->filename, '!');
			if (m_filename == NULL) {
				m_filename = req->filename;
			} else {
				m_filename += 1;
			}
			if STRCMP(p->filename, !=, m_filename) {
				continue;
			}
		}
		if (req->flags & ME_DESCR_FORMAT) {
			if (p->descr_format != req->descr_format) {
				continue;
			}
		}
		if (req->flags & ME_AGGREGATE) {
			if (strcmp(p->aggregate, req->aggregate) != 0) {
				continue;
			}
		}
		if (req->description.flags & MED_MSOURCE) {
			if (p->description.msource != req->description.msource) {
				continue;
			}
		}
		if (req->description.flags & MED_MTYPE) {
			if (p->description.mtype != req->description.mtype) {
				continue;
			}
		}
		if (req->description.flags & MED_PAYLOAD_TYPE) {
			if (p->description.payload_type !=
			    req->description.payload_type) {
				continue;
			}
		}
		if (req->description.flags & MED_CLOCK_RATE) {
			if (p->description.clock_rate !=
			    req->description.clock_rate) {
				continue;
			}
		}
		if (req->description.flags & MED_ENCODING_NAME) {
			if (strcmp
			    (p->description.encoding_name,
			     req->description.encoding_name) != 0) {
				continue;
			}
		}
		if (req->description.flags & MED_AUDIO_CHANNELS) {
			if (p->description.audio_channels !=
			    req->description.audio_channels) {
				continue;
			}
		}
		if (req->description.flags & MED_BIT_PER_SAMPLE) {
			if (p->description.bit_per_sample !=
			    req->description.bit_per_sample) {
				continue;
			}
		}
		if (req->description.flags & MED_SAMPLE_RATE) {
			if (p->description.sample_rate !=
			    req->description.sample_rate) {
				continue;
			}
		}
		if (req->description.flags & MED_BITRATE) {
			if (p->description.bitrate != req->description.bitrate) {
				continue;
			}
		}
		if (req->description.flags & MED_PRIORITY) {
			if (p->description.priority !=
			    req->description.priority) {
				continue;
			}
		}
		if (req->description.flags & MED_FRAME_RATE) {
			if (p->description.frame_rate !=
			    req->description.frame_rate) {
				continue;
			}
		}
		if (req->description.flags & MED_BYTE_PER_PCKT) {
			if (p->description.byte_per_pckt !=
			    req->description.byte_per_pckt) {
				continue;
			}
		}
		return p;
	}
	return NULL;
}
