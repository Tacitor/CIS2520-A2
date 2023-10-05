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
	FILE *fp;
	FASTArecord fRecord;
	int lineNumber = 0, recordNumber = 0, status;
	int eofSeen = 0;
	clock_t startTime, endTime;

	int fastaRecordsArraySize = 128;																   // start out with 128 records
	FASTArecord *fastaRecordsArr = (FASTArecord *)malloc(fastaRecordsArraySize * sizeof(FASTArecord)); // FASTArecord fastaRecordsArr[fastaRecordsArraySize];

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Failure opening %s : %s\n", filename, strerror(errno));
		return -1;
	}

	/** record the time now, before we do the work */
	startTime = clock();

	do
	{
		/** print a '.' every 10,000 records so
		 * we know something is happening */
		if ((recordNumber % 10000) == 0)
		{
			printf(".");
			fflush(stdout);
		}

		fastaInitializeRecord(&fRecord);

		status = fastaReadRecord(fp, &fRecord);
		if (status == 0)
		{
			eofSeen = 1;
		}
		else if (status > 0)
		{
			lineNumber += status;
			recordNumber++;

			// check to see if we have run out of room
			if (recordNumber >= fastaRecordsArraySize)
			{
				// panic make array bigger
				// double the fastaRecordsArraySize
				fastaRecordsArraySize *= 2;

				// find a new chuck of memory for the existing data that is big enough to store double the data
				fastaRecordsArr = (FASTArecord *)realloc(fastaRecordsArr, (sizeof(FASTArecord) * fastaRecordsArraySize));
			}

			// store the record in the array at the index used to count the number of records starting at 0
			// start by initilizing the record at this index in the array
			fastaInitializeRecord(&(fastaRecordsArr[recordNumber - 1]));

			// now copy over the data
			fastaRecordsArr[recordNumber - 1].id = fRecord.id;
			fastaRecordsArr[recordNumber - 1].description = strdup(fRecord.description);
			fastaRecordsArr[recordNumber - 1].sequence = strdup(fRecord.sequence);

			// deallocate the strings in the buffer fRecord
			fastaClearRecord(&fRecord);
		}
		else
		{
			fprintf(stderr, "status = %d\n", status);
			fprintf(stderr, "Error: failure at line %d of '%s'\n", lineNumber, filename);
			return -1;
		}

	} while (!eofSeen);
	printf(" %d FASTA records -- %d allocated (%.3f%% waste)\n", recordNumber, fastaRecordsArraySize, ((fastaRecordsArraySize - recordNumber) / (double)fastaRecordsArraySize) * 100);

	/** record the time now, when the work is done,
	 *  and calculate the difference*/
	endTime = clock();

	(*timeTaken) = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;

	fclose(fp);

	// debug the records
	/*
	for (int i = 0; i < recordNumber;)
	{
		// for (int j = 0; j < 75; j++)
		// {
		printf("\n\n");
		fastaPrintRecord(stdout, &fastaRecordsArr[i]);
		i++;
		//}
		//getchar();
	}
	*/

	// now go through and clear the records in the array
	// do not deallocate because the momory space was alloc'd at the time of the array as a block
	for (int i = 0; i < recordNumber; i++)
	{
		fastaClearRecord(&fastaRecordsArr[i]);
	}

	// now deallocate the array itself
	free(fastaRecordsArr);

	return recordNumber;
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

	printf("%.3lf seconds taken for processing total\n", totalTimeInSeconds);

	totalTimeInSeconds /= (double)repeatsRequested;

	minutesPortion = (int)(totalTimeInSeconds / 60);
	totalTimeInSeconds = totalTimeInSeconds - (60 * minutesPortion);
	printf("On average: %d minutes, %.3lf second per run\n",
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
 * Program mainline
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
					fprintf(stderr, "Error: need argument for repeats requested\n");
					return 1;
				}
				if (sscanf(argv[++i], "%ld", &repeatsRequested) != 1)
				{
					fprintf(stderr, "Error: cannot parse repeats requested from '%s'\n", argv[i]);
					return 1;
				}
			}
			else
			{
				fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
				usage(argv[0]);
			}
		}
		else
		{
			recordsProcessed = processFastaRepeatedly(argv[i], repeatsRequested);
			if (recordsProcessed < 0)
			{
				fprintf(stderr, "Error: Processing '%s' failed -- exitting\n", argv[i]);
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
