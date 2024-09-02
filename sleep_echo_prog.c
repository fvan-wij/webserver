#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	(void) argc;
	const int32_t sleep_time = atoi(argv[1]);
	printf("sleeping for [%i]\n", sleep_time);
	sleep(sleep_time);
	printf("done sleeping\n");
	return 0;
}
