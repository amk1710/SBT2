#include <stdlib.h>
#include <stdio.h>


typedef int (*funcp) ();

int pos;
//a posição do vetor de unsigned char que será a função.


union intchar{
	int i;
	unsigned char c[4];
};
typedef union intchar Intchar;


int alocada[20]; /* usado para informar quais variaveis já foram alocadas na pilha */

int posdalinha[50]; /* usado para guardar em que posição do vetor começa cada linha do programa */


//função aloca espaço para todas as variáveis até a variável de número n, se ainda não foram alocadas.
void prepara_variavel(int n, unsigned char * area){
	int i;
	Intchar aloc;
	aloc.i = 0;
	if(alocada[0] == 0){ /* inicializa pilha de memoria */
		area[pos] = 0x55;
		area[pos+1] = 0x48;
		area[pos+2] = 0x89;
		area[pos+3] = 0xe5;
		pos += 4;
	}
	while(alocada[aloc.i] != 0 && aloc.i < 20){
		aloc.i = aloc.i + 1;
	}
	i = aloc.i;	
	aloc.i = (n - aloc.i) * 4;
	//aloc é o número de bytes a serem alocados na pilha agora.
	//subtrai %rsp por aloc.i
	area[pos] = 0x48;
	area[pos+1] = 0x83;
	area[pos+2] = 0xec;
	area[pos+3] = aloc.c[0];
	pos += 4;
	
	// isso funciona para o número máximo de variáveis que estamos lidando, 20
	
	for(i = i; i <= n; i++){
		alocada[i] = 1;
	} /* atualiza o vetor que diz quais variaveis já foram alocadas */
}



// função que retorna o valor numerico do varpc passado, seja ele uma constante ou uma variavel. resultará em erro se a variavel pedida não tiver sido inicializada

void varpc1_2_r10d(char *varpc, unsigned char *area){ /* move varpc1 para r10d */
	Intchar constante;
	int varnum;
	switch(varpc[0]){
		case '$': /* constante */
			constante.i = atoi(&varpc[1]);
			area[pos] = 0x41;	 /* movl para %r10d */
			area[pos+1] = 0xba;
			area[pos+2] = constante.c[0];
			area[pos+3] = constante.c[1];
			area[pos+4] = constante.c[2];
			area[pos+5] = constante.c[3];
			pos += 6;
			break;	
		case 'v': /* variavel local */
			//      movl -((varnum+1)*4)(%rbp), %r10d
			varnum = atoi(&varpc[1]);
			area[pos] = 0x44;
			area[pos+1] = 0x8b;
			area[pos+2] = 0x55;
			area[pos+3] = (unsigned char)(0x100 - (varnum+1)*4);
			pos += 4;
			break;

		case 'p': /* parametro */
			// movl de %edi, %esi ou %edx para %r10d
			area[pos] = 0x41;
			area[pos+1] = 0x89;
			switch(varpc[1]){
				case '0':
					area[pos+2] = 0xfa;
					break;
				case '1':
					area[pos+2] = 0xf2;
					break;
				case '2':
					area[pos+2] = 0xd2;
					break;
			}
			pos += 3;
			break;
	}		
	return;
}

void varpc2_2_r11d(char *varpc, unsigned char *area){ /* move varpc2 para r11d */
	Intchar constante;
	int varnum;
	switch(varpc[0]){
		case '$': /* constante */
			constante.i = atoi(&varpc[1]);
			area[pos] = 0x41;	 /* movl para %r11d */
			area[pos+1] = 0xbb;
			area[pos+2] = constante.c[0];
			area[pos+3] = constante.c[1];
			area[pos+4] = constante.c[2];
			area[pos+5] = constante.c[3];
			pos += 6;
			break;	
		case 'v': /* variavel local */
			//      movl -((varnum+1)*4)(%rbp), %r11d
			varnum = atoi(&varpc[1]);
			area[pos] = 0x44;
			area[pos+1] = 0x8b;
			area[pos+2] = 0x5d;
			area[pos+3] = (unsigned char)(0x100 - (varnum+1)*4);
			pos += 4;
			break;

		case 'p': /* parametro */
			// movl de %edi, %esi ou %edx para %r11d
			area[pos] = 0x41;
			area[pos+1] = 0x89;
			switch(varpc[1]){
				case '0':
					area[pos+2] = 0xfb;
					break;
				case '1':
					area[pos+2] = 0xf3;
					break;
				case '2':
					area[pos+2] = 0xd3;
					break;
			}
			pos += 3;
			break;
	}		
	return;
}



void retorno(char *varpc, unsigned char* area){
	Intchar constante;
	int varnum;
	switch(varpc[0]){
		case '$': /* constante */
			constante.i = atoi(&varpc[1]);
			area[pos] = 0xb8;	 /* movl para %eax */
			area[pos+1] = constante.c[0];
			area[pos+2] = constante.c[1];
			area[pos+3] = constante.c[2];
			area[pos+4] = constante.c[3];
			pos += 5;
			break;	
		case 'v': /* variavel local */
		//      movl -((varnum+1)*4)(%rbp), %eax
			varnum = atoi(&varpc[1]);
			area[pos] = 0x8b;
			area[pos+1] = 0x45;
			area[pos+2] = (unsigned char)(0x100 - (varnum+1)*4);
			pos += 3;
			break;

		case 'p': /* parametro */
			area[pos] = 0x89;
			switch(varpc[1]){
				case '0':
					area[pos+1] = 0xf8;
					break;
				case '1':
					area[pos+1] = 0xf0;
					break;
				case '2':
					area[pos+1] = 0xd0;
					break;
			}
			pos += 2;
			break;
	}		
	return;
}

void atribuicao(char *var, char * varpc1, char op, char *varpc2, unsigned char* area){
	
	int varnum = atoi(&var[1]);
	if(alocada[varnum] == 0) prepara_variavel(varnum, area); /* se a variavel não foi alocada, a aloca */
	varpc1_2_r10d(varpc1, area); /* move varpc1 para %r10d */
	varpc2_2_r11d(varpc2, area); /* move varpc2 para %r11d */	
	switch(op){
		case '+': /* %r11d += %r10d */
			area[pos] = 0x45;
			area[pos+1] = 0x01;
			area[pos+2] = 0xd3;
			pos += 3;
			break;
		case '-': /* %r11d -= %r10d */
			area[pos] = 0x45;
			area[pos+1] = 0x29;
			area[pos+2] = 0xd3;
			pos += 3;
			break;
		case '*': /* %r11d *= %r10d */
			area[pos] = 0x45;
			area[pos+1] = 0x0f;
			area[pos+2] = 0xaf;
			area[pos+3] = 0xda;
			pos += 4;
			break;
	}
	// move %r11d para variavel
	area[pos] = 0x44;
	area[pos+1] = 0x89;
	area[pos+2] = 0x5d;
	area[pos+3] = (unsigned char)(0x100 - (varnum+1)*4);
	pos += 4; 
	
	return;
}

void se(char *var, int n1, int n2, int n3, unsigned char *area){
	int varnum;
	//      movl -((varnum+1)*4)(%rbp), %r10d
	varnum = atoi(&varpc[1]);
	area[pos] = 0x44;
	area[pos+1] = 0x8b;
	area[pos+2] = 0x55;
	area[pos+3] = (unsigned char)(0x100 - (varnum+1)*4);
	pos += 4;
	//********
	
	
	return;
}

funcp compila (FILE *f)
{
	funcp funcao;
	char cmd[4];
	char varpc1[3];
	char varpc2[3];
	char op;
	int n1, n2, n3;
	int nlinhas;
	int retornoleitura;
	unsigned char *area = (unsigned char*) malloc(1000 * sizeof(unsigned char));
//     IMPORTANTE: substituir esse tamanho 1000 pelo maior tamanho possivelmente usado depois. Ajustar depois com realloc
	for(nlinhas = 0; nlinhas < 20; nlinhas++) { alocada[nlinhas] = 0; }
	pos = 0;
	nlinhas = 0;
	posdalinha[0] = 0; /* a posição de inicio de cada linhas sera aqui gravada */
	retornoleitura = fscanf(f, "%s", cmd);
	while(retornoleitura != EOF && nlinhas <= 50){
		switch(cmd[0]){
			case 'r': /* retorno */
				posdalinha[nlinhas] = pos;
				fscanf(f, "%s", varpc1);
				retorno(varpc1, area);
				nlinhas++;
				break;
			case 'v': /* atribuição */
				posdalinha[nlinhas] = pos;
				fscanf(f, "%s", varpc1); /* lê o = */
				fscanf(f, "%s %c %s", varpc1, &op, varpc2);
				atribuicao(cmd, varpc1, op, varpc2, area);
				nlinhas++;
				break;
			case 'i': /* if */
				posdalinha[nlinhas] = pos;
				fscanf(f, "%s %d %d %d", varpc1, &n1, &n2, &n3);
				se(varpc1, n1, n2, n3, area);
				nlinhas++;
				break;
			default: /* comando não reconhecido */
				printf("Comando não reconhecido %s na linha %d \n", cmd, nlinhas);

		}
		retornoleitura = fscanf(f, "%s", cmd);
	}
	if(alocada[0] == 1){ /* se pelo menos uma variável foi alocada: */
		area[pos] = 0x48;
		area[pos+1] = 0x89;  /* desaloca a pilha */
		area[pos+2] = 0xec;
		area[pos+3] = 0x5d;
		pos += 4;
	}
	area[pos] = 0xc3; /* ret */
	area = realloc(area, pos*sizeof(unsigned char)); /* muda o tamanho da memoria alocada para o tamanho necessário */
	funcao = (funcp)area;
	return funcao;
}



void libera (void *p)
{
	free(p);
}
