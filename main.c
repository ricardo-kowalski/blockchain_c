/********************************************************************************
*							~~   KOWALSKICHAIN v.1  ~~						                          	
*						Simple Blockchain using md5 hash			                        			
*																		                                        		
*	Os arquivos md5.c e md5.h foram adaptados, os originais encontram-se em:    				
*	https://github.com/chinaran/Compute-file-or-string-md5 						            
*																				                                        
*********************************************************************************/

/*O HASH DE CADA BLOCO é o HASH( NUMERO_DO_BLOCO+HASH_DO_ARQUIVO+HASH_ANTERIOR+NONCE)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include "block.h"

#include <unistd.h>					//apenas no linux
unsigned int sleep(unsigned int seconds);		//apenas no linux

#define BUFFER_SIZE 576		//501+33+33+4+4+1 -define um máximo de caracteres lidos em cada linha do arquivo

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct bloco{
	unsigned int numero;
	unsigned int nonce;
	char hash_ant[33];
	char dados[501] ;
	char hash[33];
}Bloco;

int main(int argc, char *argv[]){
	int cap = 1;								//capacidade inicial
    Bloco *b;
	b = (Bloco*)malloc(sizeof(Bloco)*cap);
	printf("~~   KOWALSKICHAIN v.1  ~~\n--------------------------\n");
	printf("\nCarregando blockchain. . .\n");
	printf("\n--------------------------\n");
    sleep(1); 									//espera 1 segundo (only on linux)

	//ADICIONAR ROTINA QUE EXCLUI AS LINHAS EM BRANCO /************************************************/

	char buffer[BUFFER_SIZE];					//string com até n caracteres
    int n = 0; 									//qtidade de elementos struct no arquivo blockchain

    FILE* f = fopen("blockchain.txt", "r");
    if (!f){
		printf("\nErro na leitura do arquivo!\n");
		return -1;
	}
    while (fgets(buffer, BUFFER_SIZE, f)){		//lê cada uma das linhas como string
		b[n] = txtToStruct(buffer);				//armazena a sring lida como elemento da struct
        if (b[n].numero != n){
			printf(ANSI_COLOR_RED"\nBloco %i Corrompido!\n\n"ANSI_COLOR_RESET, n);
			exit(1);
		}
		n++;									//conta o nº de elementos
		cap++;
		b = realloc(b, sizeof(Bloco)*cap);		//realoca a quantidade de vetores de b
    }
	//printUntil(b, n);							//imprime todos até o último elemento
	verificaTudo_noPrint(b, n);
	printf("\ntotal de blocos: %d \n", n);		//qtidade de elementos struct no arquivo blockchain
	
	int opc; 									//Opcao do menu selecionada.
	int i;
	do{
		opc = menuInit();						//chama menu principal
		if(opc==1){
			if(n==cap){							//se capacidade esgotada do vetor usa, "realloc" (ex: dobrar capacidade).
				cap += 1;
				b = realloc(b, sizeof(Bloco)*cap);
			}
			if(n==0){
				b[n] = *genesis();
				//printf("\n%d#%d#%s#%s#%s", b[n].numero, b[n].nonce,b[n].hash_ant,b[n].dados,b[n].hash);
			}
			else{	
				b[n] = *newBlock((n-1), (b[n-1].hash));	//cria próximo elemento
				//printf("\nBloco:\t\t%d\nNonce:\t\t%d\nHash_ant:\t%s\nDados:\t\t%s\nHash:\t\t%s\n", b[n].numero, b[n].nonce, b[n].hash_ant, b[n].dados, b[n].hash);
			}
			n++; 								//Incrementa numero de registros cadastrados.
		}
		else if(opc==2){
			//FAZER
			verificaTudo(b, n);
		} 
		else if(opc==3){						//busca um bloco cadastrado.
			int entr;
			printf("numero do bloco: ");
			scanf("%d",&entr);
			printOne(b, entr, n);
		}
		else if(opc==4){
			printUntil(b, n);
		} 
	}while(opc!=5); 
	free(b);
	return 0;
}



/* how nonce is calculated: https://blockchaindemo.io/static/media/nonce.a6291d01.gif*/
