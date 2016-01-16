
#include <stdio.h>
#include <getopt.h>
#include <fenice/command_environment.h>

void usage()
{
	fprintf(stderr,
		"fenice [--config-file | -c <config_file>] [ --view-err | -v ]\n \
			 --view-err enables stderr output\n\n");
	return;
}

uint32 command_environment(int argc, char **argv)
{

	static const char short_options[] = "r:p:c:v";

	int n;
	uint32 nerr = 0;	/*number of error */
	uint32 config_file_not_present = 1;
	uint32 view_log = 0;
	uint32 flag = 0;	/*0 to show help */
	static struct option long_options[] = {
		{"config-file", 1, 0, 'c'},
		{"view-err", 0, 0, 'v'},
		{"rtsp-port", 1, 0, 'p'},
		{"avroot-dir", 1, 0, 'r'},
		{"help", 0, 0, '?'},
		{0, 0, 0, 0}
	};


	while ((n = getopt_long(argc, argv, short_options,
							long_options, NULL)) != -1) {
		flag = 1;
		switch (n) {
		case 0:	/* Flag setting handled by getopt-long */
			break;

		case 'c':
			// = atoi(optarg);
			prefs_init(optarg);
			config_file_not_present = 0;
			/* prefs_init() loads root directory, port, hostname and domain name on
			   a static variable prefs */
			break;
		case 'v':
			view_log = 1;
			break;
		case 'p':

			break;
		case 'r':

			break;
		case ':':
			fprintf(stderr, "Missing parameter to option!");
			break;
		case '?':
			flag = 0;
			nerr++;
			break;
		default:
			nerr++;
		}
	}
	if (!flag) {
		nerr++;
		usage();
	} else if (config_file_not_present)
		prefs_init(NULL);

#if 0
	if (view_log)
		fnc_log_init(prefs_get_log(), FNC_LOG_OUT);
	else
		fnc_log_init(prefs_get_log(), FNC_LOG_SYS);
#endif

	return nerr;
}
