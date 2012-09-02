#ifndef __DPRINTS__
#define __DPRINTS__

#ifdef DEBUG_PRG
#define dprintf printf
#define dfprintf fprintf
#else
#define dprintf dummy_printf
#define dfprintf dummy_fprintf
#endif

int dummy_printf(char *fmt, ...)
{
	return 0;
}

int dummy_fprintf(FILE *f, char *fmt, ...)
{
	return 0;
}

#endif

