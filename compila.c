#include <stdlib.h>
#include <stdio.h>


typedef int (*funcp) ();



funcp compila (FILE *f)
{
	//teste com funçao bemsimples.s
	unsigned char *area = (unsigned char*) malloc(13 * sizeof(unsigned char));
	area[0] = 0x55;
	area[1] = 0x48;
	area[2] = 0x89;
	area[3] = 0xe5;
	area[4] = 0x89;
	area[5] = 0xf8;
	area[6] = 0x01;
	area[7] = 0xf0;
	area[8] = 0x48;
	area[9] = 0x89;
	area[10] = 0xec;
	area[11] = 0x5d;
	area[12] = 0xc3;
	funcp ret = (funcp)area;
	return ret;
}



void libera (void *p)
{
	free(p);
}
