#include <stdio.h>
#include <stdlib.h>

typedef int (*funcp) ();

funcp compila (FILE *f);

void libera (void *p);


int main(void)
{
	FILE *f = fopen("arquivo.txt", "r");
	funcp function = compila(f);
	int i = (*function)(10, 20);
	printf("%d\n", i);
	libera(function);
	return 0;
}
