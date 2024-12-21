#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("error no argument given\n");
		return 1;
	}
	(void) argc;
	const int32_t sleep_time = atoi(argv[1]);
	printf("sleeping for [%i]\n", sleep_time);
	sleep(sleep_time);
	printf("done sleeping\n");

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ( "Current local time and date: %s", asctime (timeinfo) );

	struct timeval stop;
	//do stuff
	gettimeofday(&stop, NULL);
	printf("took %lu us\n", (stop.tv_sec) * 1000000 + stop.tv_usec);
	return 0;
}
