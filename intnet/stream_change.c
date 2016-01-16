
#include <fenice/rtp.h>
#include <fenice/utils.h>
#include <fenice/intnet.h>
#include <string.h>

int stream_change(RTP_session * changing_session, int value)
{
	const char *encoding_name;

	encoding_name = changing_session->current_media->description.encoding_name;

	if (value == 0) {
		return ERR_NOERROR;
	} else if (value == -1) {
		if (changing_session->MinimumReached != 1) {
			changing_session->MaximumReached = 0;
			if (strcmp(encoding_name, "MPA") == 0)
				return downgrade_MP3(changing_session);
			if (strcmp(encoding_name, "GSM") == 0)
				return downgrade_GSM(changing_session);
			if (strcmp(encoding_name, "L16") == 0)
				return downgrade_L16(changing_session);
		}
	} else if (value == 1) {
		if (changing_session->MaximumReached != 1) {
			changing_session->MinimumReached = 0;
			if (strcmp(encoding_name, "MPA") == 0)
				return upgrade_MP3(changing_session);
			if (strcmp(encoding_name, "GSM") == 0)
				return upgrade_GSM(changing_session);
		}
	} else if (value == -2) {
		if (changing_session->MinimumReached != 1) {
			changing_session->MaximumReached = 0;
			if (strcmp(encoding_name, "MPA") == 0)
				return half_MP3(changing_session);
			if (strcmp(encoding_name, "GSM") == 0)
				return half_GSM(changing_session);
			if (strcmp(encoding_name, "L16") == 0)
				return half_L16(changing_session);
		}
	}

	return ERR_NOERROR;
}
