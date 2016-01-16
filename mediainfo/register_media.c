
#include <string.h>		/*strcmp */
#include <fenice/utils.h>
#include <fenice/mp3.h>
#include <fenice/mpeg.h>
#include <fenice/itu_h264.h>

// load the file for ready
// sent the frame to client
// close file
media_fn media_func[FILE_COUNT] = {
		[FILE_NONE] = {
				.load_media = NULL,
				.read_media = NULL,
				.free_media = NULL,
		},
		[FILE_MP3] = {
				.load_media = load_MPA,
				.read_media = read_MP3,
				.free_media = free_MPA,
		},
		[FILE_MPEG] = {
				.load_media = load_MPV,
				.read_media = read_MPEG_video,
				.free_media = free_MPV,
		},
		[FILE_H264] = {
				.load_media = load_h264,
				.read_media = read_h264,
				.free_media = free_h264,
		}
};

int register_media(media_entry * me)
{
	media_file_type ftype = me->description.ftype;
	me->media_handler = &media_func[ftype];

	return ERR_NOERROR;
#if 0

	const char *encoding_name = me->description.encoding_name;

	me->media_handler = xnew(media_fn); // (media_fn *) calloc(1, sizeof(media_fn));
	if STRCMP(encoding_name, ==, "MP4V-ES") {
		me->media_handler->free_media = free_MP4ES;
		me->media_handler->load_media = load_MP4ES;
		me->media_handler->read_media = read_MPEG4ES_video;
	} else if STRCMP(encoding_name, ==, "MPV") {
		me->media_handler->free_media = free_MPV;
		me->media_handler->load_media = load_MPV;
		me->media_handler->read_media = read_MPEG_video;
	} else if (strcmp(me->description.encoding_name, "MPA") == 0) {
		me->media_handler->free_media = free_MPA;
		me->media_handler->load_media = load_MPA;
		me->media_handler->read_media = read_MP3;
	} else if (strcmp(me->description.encoding_name, "GSM") == 0) {
		me->media_handler->free_media = free_GSM;
		me->media_handler->load_media = load_GSM;
		me->media_handler->read_media = read_GSM;
	} else if (strcmp(me->description.encoding_name, "L16") == 0) {
		me->media_handler->free_media = free_L16;
		me->media_handler->load_media = load_L16;
		me->media_handler->read_media = read_PCM;
	} else if (strcmp(me->description.encoding_name, "H26L") == 0) {
		me->media_handler->free_media = free_H26L;
		me->media_handler->load_media = load_H26L;
		me->media_handler->read_media = read_H26L;
	} else if (strcmp(me->description.encoding_name, "MP2T") == 0) {
		me->media_handler->free_media = free_MP2T;
		me->media_handler->load_media = load_MP2T;
		me->media_handler->read_media = read_MPEG_ts;
	} else if (strcmp(me->description.encoding_name, "RTP_SHM") == 0) {
		me->media_handler->free_media = free_RTP_SHM;
		me->media_handler->load_media = load_RTP_SHM;
		me->media_handler->read_media = NULL;	// read_RTP_SHM;            
	} else {
		ERRORLOGG("Encoding type not supported.");
		return ERR_UNSUPPORTED_PT;
	}
	return ERR_NOERROR;
#endif
}
