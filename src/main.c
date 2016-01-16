
#include <stdio.h>
#include <stdlib.h>
#include <fenice/socket.h>
#include <fenice/eventloop.h>
#include <fenice/command_environment.h>
#include <fenice/en_xmalloc.h>
#include <libev-4.20/ev.h>

#define LOG_STYLES_GECSM (LOG_STYLE_LOGLEVEL | \
	LOG_STYLE_SOURCE | \
	LOG_STYLE_FORMAT | \
	LOG_STYLE_NEWLINE )


void fncheader(void);	// defined in src/fncheader.c

int exit_system;

static void sigfatal(int sig)
{
	exit_system = 1;
}

static void sigquit(int sig)
{
	exit_system = 1;
}

static int init_log(void)
{
	if (CreateLogHandleG() == NULL) {
		printf("Create log system failed");
		return -1;
	}

	SetLogOutputG(LOG_OUTPUT_FILE, "/var/log/fenice.log", LOG_NO_OUTPUTFUNC);
	SetLogLevelG(LOG_LEVEL_DEBUG);
	SetLogStylesG(LOG_STYLES_GECSM, LOG_NO_STYLEFUNC);

	return 0;
}

int main(int argc, char **argv)
{
	tsocket main_fd, main_sctp_fd = -1;
	unsigned int port;
	int sctp_port;

	exit_system = 0;

	signal(SIGHUP, sigfatal);
	signal(SIGINT, sigfatal);
	signal(SIGBUS, sigfatal);
	signal(SIGQUIT, sigquit);
	signal(SIGILL, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	// Fake timespec for fake nanosleep. See below.
	// struct timespec ts = { 0, 0 };

	// printf("\n%s %s - Open Media Streaming Project \n\n", PACKAGE, VERSION);
	fncheader();
	init_log();

	/*command_environment parses the command line and returns the number of error */
	if (command_environment(argc, argv))
		return 0;
	/* prefs_get_port() reads the static var prefs and returns the port number */
	/*获取默认的端口号:554*/
	port = prefs_get_port();
	/* prefs_get_sctp_port() reads the static var prefs and returns the port number. A value < 0 means disabled */
	//sctp_port = prefs_get_sctp_port();

	printf("CTRL-C terminate the server.\n");
	printf("Waiting for RTSP connections on TCP port %d...\n", port);
	/*建立监听套接字*/
	main_fd = tcp_listen(port);
	if (main_fd < 0) {
		printf("Unable to listen on TCP port %d. Server is aborting.\n", port);
		return 0;
	}
	/* Initialises the array of schedule_list sched
	   and creates the thread schedule_do() -> look at schedule.c */
	if (schedule_init() == ERR_FATAL) {
		FATALLOGG("Can't start scheduler. Server is aborting.");
		return 0;
	}
	RTP_port_pool_init(RTP_DEFAULT_PORT);
	/* puts in the global variable port_pool[MAX_SESSION] all the RTP usable ports
	   from RTP_DEFAULT_PORT = 5004 to 5004 + MAX_SESSION */
	
	while (1) {
		eventloop(main_fd, main_sctp_fd);
		if (exit_system) {
			break;
		}
	}

	DestroyLogHandleG();
	xmalloc_debug_stats();
	/*
	 * eventloop looks for incoming RTSP connections and
	 * generates for each all the information in the
	 * structures RTSP_list, RTP_list, and so on
	 */

	return 0;
}
