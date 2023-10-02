/**
 * arraydouble - loads the data and stores all the records into an array. As we don’t know how big the array
 * must be in advance, we will have to grow the array as we go and copy the data
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "fasta.h"

int processFasta(char *filename, double *timeTaken)
{
	return 0; // TEMP RETURN PLEASE DON'T FORGET TO CHANGE THIS LUKAS
}

int processFastaRepeatedly(
	char *filename,
	long repeatsRequested)
{
	double timeThisIterationInSeconds;
	double totalTimeInSeconds = 0;
	int minutesPortion;
	int status;
	long i;

	for (i = 0; i < repeatsRequested; i++)
	{
		status = processFasta(filename, &timeThisIterationInSeconds);
		if (status < 0)
			return -1;
		totalTimeInSeconds += timeThisIterationInSeconds;
	}

	printf("%lf seconds taken for processing total\n", totalTimeInSeconds);

	totalTimeInSeconds /= (double)repeatsRequested;

	minutesPortion = (int)(totalTimeInSeconds / 60);
	totalTimeInSeconds = totalTimeInSeconds - (60 * minutesPortion);
	printf("On average: %d minutes, %lf second per run\n",
		   minutesPortion, totalTimeInSeconds);

	return status;
}

void usage(char *progname)
{
	fprintf(stderr, "%s [<OPTIONS>] <file> [ <file> ...]\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Prints timing of loading and storing FASTA records.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options: \n");
	fprintf(stderr, "-R <REPEATS> : Number of times to repeat load.\n");
	fprintf(stderr, "             : Time reported will be average time.\n");
	fprintf(stderr, "\n");
}

/**
 * Program main function
 */
int main(int argc, char **argv)
{
	int i, recordsProcessed = 0;
	long repeatsRequested = 1;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'R')
			{
				if (i >= argc)
				{
					fprintf(stderr,
							"Error: need argument for repeats requested\n");
					return 1;
				}
				if (sscanf(argv[++i], "%ld", &repeatsRequested) != 1)
				{
					fprintf(stderr,
							"Error: cannot parse repeats requested from '%s'\n",
							argv[i]);
					return 1;
				}
			}
			else
			{
				fprintf(stderr,
						"Error: unknown option '%s'\n", argv[i]);
				usage(argv[0]);
			}
		}
		else
		{
			recordsProcessed = processFastaRepeatedly(argv[i],
													  repeatsRequested);
			if (recordsProcessed < 0)
			{
				fprintf(stderr, "Error: Processing '%s' failed -- exitting\n",
						argv[i]);
				return 1;
			}
			printf("%d records processed from '%s'\n",
				   recordsProcessed, argv[i]);
		}
	}

	if (recordsProcessed == 0)
	{
		fprintf(stderr,
				"No data processed -- provide the name of"
				" a file on the command line\n");
		usage(argv[0]);
		return 1;
	}

	return 0;
}
