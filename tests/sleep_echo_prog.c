#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

int main1(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("error no argument given\n");
		return 1;
	}
	(void) argc;

	const int32_t sleep_time = atoi(argv[1]);
	printf("sleeping for [%i]\n", sleep_time);

	struct timeval start;
	gettimeofday(&start, NULL);

	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ("start time and date: %s", asctime (timeinfo) );

	sleep(sleep_time);
	printf("done sleeping\n");


	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ("end time and date: %s", asctime (timeinfo) );

	struct timeval stop;
	//do stuff
	gettimeofday(&stop, NULL);
    int elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms

	// printf("took %lu us\n", (stop.tv_sec) * 1000000 + stop.tv_usec);
	printf("took %d ms\n", elapsedTime);
	// execl("/home/joppe/.pyenv/shims/python3", "python3", "-m", "http.server", "8997", (char *) NULL);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("error no argument given\n");
		return 1;
	}
	(void) argc;

	printf("args: %s\n", argv[1]);
}

