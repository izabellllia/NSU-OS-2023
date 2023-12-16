#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"

static char *blankskip(char *);

Job* parseline(char *line)
{
	Job* headJob = createNewJob(NULL);
	Job* newJob = headJob;
	char *infile = NULL, *outfile = NULL;
	Command cmds[MAXCMDS];
	int nargs = 0, ncmds = 0;
	char *currentLinePtr;
	char error = 0;
	static char delim[] = " \t|&<>;\n";
	// TODO: добавить корректный парсинг аргументов кавычках

	/* initialize  */
	currentLinePtr = line;
	cmds[0].cmdargs[0] = (char *) NULL;
	for (int i = 0; i < MAXCMDS; i++)
		cmds[i].cmdflag = 0;

	while (*currentLinePtr) {        /* until line has been parsed */
		currentLinePtr = blankskip(currentLinePtr);       /*  skip white space */
		if (!*currentLinePtr) break; /*  done with line */

		/*  handle <, >, |, &, and ;  */
		if (*currentLinePtr == '&' || *currentLinePtr == ';') {
			if (*currentLinePtr == '&')
				newJob->initialFg = 0;
			*currentLinePtr++ = '\0';
			createNewProcessInJob(newJob, cmds[ncmds]);
			++ncmds;
			nargs = 0;
			newJob = createNewJob(newJob);
			continue;
		}
		if (*currentLinePtr == '>') {
			if (newJob->outPath) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			if (*(currentLinePtr+1) == '>') {
				newJob->appendFlag = 1;
				*currentLinePtr++ = '\0';
			}
			*currentLinePtr++ = '\0';
			currentLinePtr = blankskip(currentLinePtr);
			if (!*currentLinePtr) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}

			outfile = currentLinePtr;
			currentLinePtr = strpbrk(currentLinePtr, delim);
			newJob->outPath = (char*) malloc(currentLinePtr - outfile);
			memcpy(newJob->outPath, outfile, currentLinePtr - outfile);
			// newJob->inPath[currentLinePtr - outfile] = '\0';
			// newJob->outFd = open(tmpStr, flags);
			// if (newJob->outFd < 0) {
			// 	perror("STDOUT redefining error");
			// 	error = 1;
			// 	break;
			// }
			if (isspace(*currentLinePtr))
				*currentLinePtr++ = '\0';
			continue;
		}
		if (*currentLinePtr == '<') {
			*currentLinePtr++ = '\0';
			currentLinePtr = blankskip(currentLinePtr);
			if (!*currentLinePtr || newJob->inPath) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			infile = currentLinePtr;
			currentLinePtr = strpbrk(currentLinePtr, delim);
			newJob->inPath = (char*) malloc(currentLinePtr - infile);
			memcpy(newJob->inPath, infile, currentLinePtr - infile);
			// newJob->inPath[currentLinePtr - infile] = '\0';
			// newJob->inFd = open(tmpStr, O_RDONLY);
			// if (newJob->inFd < 0) {
			// 	perror("STDIN redefining error");
			// 	error = 1;
			// 	break;
			// }
			if (isspace(*currentLinePtr))
				*currentLinePtr++ = '\0';
			continue;
		}
		if (*currentLinePtr == '|') {
			if (nargs == 0) {
				fprintf(stderr, "syntax error\n");
				error = 1;
				break;
			}
			cmds[ncmds].cmdflag |= OUTPIP;
			createNewProcessInJob(newJob, cmds[ncmds]);
			cmds[++ncmds].cmdflag |= INPIP;
			*currentLinePtr++ = '\0';
			nargs = 0;
			continue;
		}
		/*  a command argument  */
		cmds[ncmds].cmdargs[nargs++] = currentLinePtr;
		cmds[ncmds].cmdargs[nargs] = (char *) NULL;
		currentLinePtr = strpbrk(currentLinePtr, delim);
		if (isspace(*currentLinePtr))
			*currentLinePtr++ = '\0';
	}  /* close while  */
	if (nargs > 0 && !error) {
		createNewProcessInJob(newJob, cmds[ncmds]);
	}

	/*  error check  */

	/*
	*  The only errors that will be checked for are
	*  no command on the right side of a pipe
	*  no command to the left of a pipe is checked above
	*/
	if (cmds[ncmds-1].cmdflag & OUTPIP) {
		if (nargs == 0) {
			fprintf(stderr, "syntax error\n");
			error = 1;
		}
	}

	newJob = headJob;
	while (newJob->next) {
		if (newJob->next->headProcess == NULL) {
			freeJobs(newJob->next);
			newJob->next = NULL;
			break;
		}
		newJob = newJob->next;
	}
	if (error) {
		freeJobs(headJob);
		return NULL;
	}
	if (headJob->headProcess == NULL)
		return NULL;
	return headJob;
}

static char * blankskip(register char *s)
{
	while (isspace(*s) && *s) ++s;
	return(s);
}