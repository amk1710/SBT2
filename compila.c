/* André Mazal Krauss 1410386 Prof. Ana */


#include <stdlib.h>
#include <stdio.h>


typedef int (*funcp) ();

int pos;
//a posição do vetor de unsigned char que será a função.


union intchar{
	int i;
	unsigned char c[4];
};
typedef union intchar Intchar; /* usado para usar pedaços de inteiros em comandos */


int alocada[20]; /* usado para informar quais variaveis já foram alocadas na pilha */

int posdalinha[50]; /* usado para guardar em que posição do vetor começa cada linha do programa */


//função aloca espaço para todas as variáveis até a variável de número n, se ainda não foram alocadas.
void prepara_variavel(int n, unsigned char * area){
	int i;
	Intchar aloc;
	aloc.i = 0;
	if(alocada[0] == 0){ /* se nenhuma foi alocada, inicializa pilha de memoria */
		area[pos] = 0x55;
		area[pos+1] = 0x48;
		area[pos+2] = 0x89;
		area[pos+3] = 0xe5;
		pos += 4;
	}
	while(alocada[aloc.i] != 0 && aloc.i < 20){ /* acha a maior variavel que ja foi alocada */
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
			area[pos+2] = (unsigned char)(0x100 - (varnum+1)*4); /* posição da variavel na pilha é -(4*n)(%rbp) */
			pos += 3;
			break;

		case 'p': /* parametro */
			area[pos] = 0x89; /* movl ___, %eax */
			switch(varpc[1]){
				case '0': /* edi */
					area[pos+1] = 0xf8;
					break;
				case '1': /* esi */
					area[pos+1] = 0xf0;
					break;
				case '2': /* edx */
					area[pos+1] = 0xd0;
					break;
			}
			pos += 2;
			break;
	}
	
	if(alocada[0] == 1){ /* se pelo menos uma variável foi alocada: */
		area[pos] = 0x48;
		area[pos+1] = 0x89;  /* desaloca a pilha */
		area[pos+2] = 0xec;
		area[pos+3] = 0x5d;
		pos += 4;
	}
	area[pos] = 0xc3; /* ret */
	pos += 1;		
	return;
}

void atribuicao(char *var, char * varpc1, char op, char *varpc2, unsigned char* area){
	
	int varnum = atoi(&var[1]);
	if(alocada[varnum] == 0) prepara_variavel(varnum, area); /* se a variavel não foi alocada, a aloca */
	varpc1_2_r10d(varpc1, area); /* move varpc1 para %r10d */
	varpc2_2_r11d(varpc2, area); /* move varpc2 para %r11d */	
	switch(op){
		case '+': /* %r10d += %r11d */
			area[pos] = 0x45;
			area[pos+1] = 0x01;
			area[pos+2] = 0xda;
			pos += 3;
			break;
		case '-': /* %r10d -= %r11d */
			area[pos] = 0x45;
			area[pos+1] = 0x29;
			area[pos+2] = 0xda;
			pos += 3;
			break;
		case '*': /* %r10d *= %r11d */
			area[pos] = 0x45;
			area[pos+1] = 0x0f;
			area[pos+2] = 0xaf;
			area[pos+3] = 0xd3;
			pos += 4;
			break;
	}
	// move %r10d para variavel
	
	area[pos] = 0x44;
	area[pos+1] = 0x89;
	area[pos+2] = 0x55;
	area[pos+3] = (unsigned char)(0x100 - (varnum+1)*4);
	pos += 4; 

	return;
}


// função se recebe uma instrução if e a posição que ela começa no vetor. Os if's ocupam sempre o mesmo tamanho no vetor para programas de até 50 linhas, o que permite que sua construção seja adiada até o resto do programa ser construído. Isso foi necessário porque as demais instruções ocupam números variáveis de posições no vetor.
void se(char *var, int n1, int n2, int n3, unsigned char *area, int tpos){
	int varnum;
	Intchar descn1; 
	Intchar descn2;
	Intchar descn3; /* deslocamentos para linhas n1, n2 e n3 */
	
	//movl -((varnum+1)*4)(%rbp), %r10d
	
	varnum = atoi(&var[1]);	
	
	area[tpos] = 0x44;
	area[tpos+1] = 0x8b;
	area[tpos+2] = 0x55;
	area[tpos+3] = (unsigned char)(0x100 - (varnum+1)*4);
	tpos += 4;
	//********
	
	descn1.i = posdalinha[n1-1] - (tpos + 6) -2;
	descn2.i = posdalinha[n2-1] - (tpos + 8) -2;
	descn3.i = posdalinha[n3-1] - (tpos + 4) -2; 
		
	//cmp $0, %r10d
	area[tpos] = 0x41;
	area[tpos+1] = 0x83;
	area[tpos+2] = 0xfa;
	area[tpos+3] = 0x00;
	
	//jg para n3
	area[tpos+4] = 0x7f;
	area[tpos+5] = descn3.c[0];
	
	//jl para n1
	area[tpos+6] = 0x7c;
	area[tpos+7] = descn1.c[0];
	
	//je para n2
	area[tpos+8] = 0x74;
	area[tpos+9] = descn2.c[0];
	
	tpos += 10;
	
	
	
	return;
}


int ses[50]; /* usado para guardar as posições no vetor area dos if's que nao foram tratados no primeiro momento */

funcp compila (FILE *f)
{
	funcp funcao;
	char cmd[4];
	char varpc1[12]; /* 20 = número de digitos de maxint + '\n' + '$' */
	char varpc2[12];
	char op;
	int tpos; /* usado para voltar no vetor de char sem perder pos */
	int n1, n2, n3;
	int nlinhas;
	int retornoleitura;
	int protelado; /* numero de if's protelados */
	unsigned char *area = (unsigned char*) malloc(1300 * sizeof(unsigned char));
// 1300 = 50 * 26. 14 é o número de posições necessárias para a maior instrução, o vi = const + const, e 50 o número maximo de linhas
	for(nlinhas = 0; nlinhas < 20; nlinhas++) { alocada[nlinhas] = 0; }
	for(nlinhas = 0; nlinhas < 50; nlinhas++) { ses[nlinhas] = -1; }
	pos = 0;
	nlinhas = 0;
	protelado = 0;
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
				fscanf(f, "%s", varpc1); /* lê o '=' */
				fscanf(f, "%s %c %s", varpc1, &op, varpc2);
				atribuicao(cmd, varpc1, op, varpc2, area);
				nlinhas++;
				break;
			case 'i': /* if */
				
				posdalinha[nlinhas] = pos;
				ses[protelado] = pos;
				fscanf(f, "%s %d %d %d", varpc1, &n1, &n2, &n3);
				
				pos += 14;
				protelado++;
				nlinhas++;
				break;
			default: /* comando não reconhecido */
				printf("Comando não reconhecido %s na linha %d \n", cmd, nlinhas);

		}
		retornoleitura = fscanf(f, "%s", cmd);
	}
	rewind(f);
	fscanf(f, "%s", cmd);
	protelado = 0;
	
	while(ses[protelado] != -1){
		while(cmd[0] != 'i'){
			fscanf(f, "%s", cmd);
		}
		fscanf(f, "%s %d %d %d", varpc1, &n1, &n2, &n3);
		fscanf(f, "%s", cmd);
		tpos = ses[protelado];
		protelado++;
		se(varpc1, n1, n2, n3, area, tpos);
	}
	area = realloc(area, (pos-1)*sizeof(unsigned char)); /* muda o tamanho da memoria alocada para o tamanho necessário */
	funcao = (funcp)area;
	return funcao;
}



void libera (void *p)
{
	free(p);
}
