#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "md5.h"

#define BUFFER_SIZE 576					//501+33+33+4+4+1 -define um máximo de caracteres lidos em cada linha do arquivo

char caminho_arquivo[25] = "blockchain.txt";	

char hash_ant_zero[33] = "00000000000000000000000000000000";

typedef struct bloco{
	unsigned int numero;
	unsigned int nonce;
	char hash_ant[33];
	char dados[501] ;
	char hash[33];
}Bloco;

Bloco txtToStruct (char *str){
    struct bloco res;
    int flag = 0;
    char *token = strtok(str, "#"); 

    while( token != NULL )
    {
		if (0 == flag)
            res.numero = atoi(token);
		else if (1 == flag)
			res.nonce = atoi(token);
		else if (2 == flag)
            strcpy(res.hash_ant, token);
        else if (3 == flag)
            strcpy(res.dados, token);
		else 
			strcpy(res.hash, token);

        flag++;
        token = strtok( NULL, "#" ); 
    }
    return res;
}

void printUntil(Bloco *b,unsigned int size){
    for (int i = 0; i < size; i++){
        //printf("Bloco:\t\t%d\nNonce:\t\t%d\nHash_ant:\t%s\nDados:\t\t%s\nHash:\t\t%s\n", b[i].numero, b[i].nonce, b[i].hash_ant, b[i].dados, b[i].hash);
		//printf("------------------------------------------------\n");
		printf("\nBloco:\t\t%d",b[i].numero);
		printf("\nNonce:\t\t%d", b[i].nonce);
		printf("\nPrev_Hash:\t%s\n",b[i].hash_ant);
		printf("Dados:\t\t");
		int j = 0;
		while (j <= strlen(b[i].dados)){
			if (b[i].dados[j] == ';'){
				printf("\n\t\t");
			}
			else 
				printf("%c",b[i].dados[j]);
			j++;
		}
		printf("\nHash:\t\t%s\n",b[i].hash);
    }
	//free(b);
}

//FUNÇÃO Exibe o menu de opcoes.
int menuInit(){						
	unsigned int opc;
	
	//system("cls");
	fflush(stdin);
	printf("\n~~~~~~~~~~~~Menu~~~~~~~~~~\n");
	printf("1. Inserir um novo bloco.\n");		//(ver ultimo bloco (n-1),cria txt(n), calc hash_arq, lê ultimo_hash lista(n-1), minera Hash(n), add lista_blocos)
	printf("2. Verificar toda blockchain.\n");	//(valida toda a lista de blocos)
	//(opc 2.1 minera toda a blockchain a partir de um dado modificado)
	printf("3. Imprimir um unico bloco.\n");
	printf("4. Imprime todos os blocos.\n");	//as opções de impressão exigem que todos os blocos estejam armazenados na struct
	printf("5. Sair.\n\nopcao:");
	scanf("%d", &opc);							//Le a opcao selecionada
	return opc;
}

//FUNÇÃO CRIA NOVO BLOCO + CRIA ARQUIVO BLOCO'n'.TXT + ADICIONA À LISTA_BLOCOS
Bloco *newBlock(int num_a, char hash_a[33]){

	Bloco *b = NULL;
	b = (Bloco*) malloc(sizeof(struct bloco));
	if(b == NULL){
		printf("\nErro na alocacao de memoria!\n");
	}
	
	b->numero = num_a+1;

	strcpy(b->hash_ant , hash_a);
	
	char opc;
	do{
		getchar();
		printf("\n--- Conteudo do bloco %i ---\n", b->numero);
		//printf("Utilize [;] como separador de transações\nExemplo:\n\n> Joao -> Pedro $65;Maria -> Antonio $98\n");
		//printf("\n--------------------------------\n");
		printf("\n>");
		fgets(b->dados, 500, stdin);
		//getchar();
		fflush(stdin);
		printf("\nTecle [s] para salvar o conteúdo: ");
		scanf("%[^\n]%*c", &opc);

	}while (opc != 's' && opc != 'S');
	
	b->nonce = 0;
	char md5_str[MD5_STR_LEN + 1];
	char *bloco_string;
	//char c = '0';
	do{							//função que calcula hash de um bloco
		sprintf(bloco_string, "%d%d", b->numero, b->nonce);
		strcat(bloco_string, b->hash_ant);
		strcat(bloco_string, b->dados);
		Compute_string_md5((unsigned char *)bloco_string, strlen(bloco_string), md5_str);
		strcpy(b->hash , md5_str);
		b->nonce = b->nonce+1;
	}while(b->hash[0] != '0' || b->hash[1] != '0');	//dificuldade o hash (2 ZEROS À ESQUERDA)
	
	imprBloco(b);
	return b;
} 

//imprime apenas um elemento no terminal
void printOne(Bloco *b,unsigned int entr, unsigned int size){
	
	int i = 0;
	int aux = 0;					//variavel para mensagem (bloco não encontrado!)

	for(i=0; i<size; i++){	
		if (b[i].numero == entr){
			printf("\nBloco:\t\t%d",b[i].numero);
			printf("\nNonce:\t\t%d", b[i].nonce);
			printf("\nPrev_Hash:\t%s\n",b[i].hash_ant);
			printf("Dados:\t\t");
			int j = 0;
			while (j <= strlen(b[i].dados)){
				if (b[i].dados[j] == ';'){
					printf("\n\t\t");
				}
				else 
					printf("%c",b[i].dados[j]);
				j++;
			}
			printf("\nHash:\t\t%s",b[i].hash);
		aux = 1;
		}	
	}
	if ( aux == 0) printf("\nBloco nao encontrado!");
	
}


//função que imprime os blocos no arquivo blockchain
/*https://msdn.microsoft.com/pt-br/library/8f30b0db.aspx */ //talvez isso resolva a gravação em tempo de execução
void imprBloco(Bloco *b){
	FILE *f;
	int i = 0;
	f = fopen(caminho_arquivo, "a"); 	//cria arquivo 
	if (f == NULL){
		printf("\nErro na abertura!");
		system("pause");
		exit(1);
	}	
	//INSERE OS DADOS ATRAVES DO PROGRAMA 
	b->dados[strcspn(b->dados,"\n")] = 0;	//ignora o \n no fprintf

	//ADICIONAR ROTINA QUE POSICIONE O PONTEIRO AO FINAL DO ÚLTIMO BLOCO/********/												

	fprintf(f, "\n%d#%d#%s#%s#%s", b->numero, b->nonce,b->hash_ant,b->dados,b->hash);
	//fwrite(b, sizeof(Bloco), 1, f);
	fclose(f);
}

//FUNÇÃO QUE ADICIONA BLOCO GENESIS
Bloco *genesis(void){				
	Bloco *b = NULL;
	b = (Bloco*) malloc(sizeof(struct bloco));
	if(b == NULL){
		printf("\nErro na alocacao de memoria!\n");
	}
	b->numero = 0;
	char opc;
	do{
		getchar();
		printf("\n--- Conteudo do bloco GENESIS ---\n");
		//printf("Utilize [;] como separador de transações\nExemplo:\n\n> Joao -> Pedro $65;Maria -> Antonio $98\n");
		//printf("\n--------------------------------\n");
		printf("\n>");
		fgets(b->dados, 500, stdin);
		fflush(stdin);
		printf("\nTecle [s] para salvar o conteúdo: ");
		scanf("%[^\n]%*c", &opc);
	}while (opc != 's' && opc != 'S');
	
	b->nonce = 0;
	strcpy(b->hash_ant , hash_ant_zero);

	char md5_str[33];
	char bloco_string[BUFFER_SIZE];

	do{
		sprintf(bloco_string, "%d%d", b->numero, b->nonce);
		strcat(bloco_string, b->hash_ant);
		strcat(bloco_string, b->dados);
		Compute_string_md5((unsigned char *)bloco_string, strlen(bloco_string), md5_str);
		strcpy(b->hash , md5_str);
		b->nonce = b->nonce+1;
	}while(b->hash[0] != '0' || b->hash[1] != '0');		//dificuldade o hash (dois zeros à esquerda)
	
	//IMPRESSÃO BLOCO GENESIS NO BLOCO
	FILE *f;
	int i = 0;
	f = fopen(caminho_arquivo, "a"); 					//cria arquivo 
	if (f == NULL){
		printf("\nErro na abertura!");
		system("pause");
		exit(1);
	}	
	b->dados[strcspn(b->dados,"\n")] = 0;				//ignora o \n no fprintf
	//sem pular uma linha 
	fprintf(f, "%d#%d#%s#%s#%s", b->numero, b->nonce,b->hash_ant,b->dados,b->hash);
	fclose(f);
	return b;
}


/*função (opc 2) que valida (apenas validação e não mineração) todos os blocos anteriores:*/
//-se o número de elementos da lista é igual ao número de blocos							- OK (apenas no início do main)
//-verifica se o hash do arquivo corresponde ao arq_hash da LISTA							-
//verifica se o prev_hash é igual ao hash do bloco n-1 										- OK
//imprime os blocos ok e os blocos corrompidos 												- OK
//Mostra qual o erro																		-
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

void verificaTudo(Bloco *b, unsigned int size){
	unsigned int i ;
	printf("\n%d\n", size);
	printf("\nVerificando hashes. . .\n");
	for (i = 0; i < size; i++){
		if (i == 0){
			if (strcmp(hash_ant_zero, b[0].hash_ant) == 0){
				//printf(ANSI_COLOR_GREEN);
				//printOne(b, i, size);
				//printf(ANSI_COLOR_RESET);

				printf(ANSI_COLOR_GREEN"\n\t\t%s\n"ANSI_COLOR_RESET"Bloco %i:"ANSI_COLOR_GREEN"\t%s\n" ANSI_COLOR_RESET,hash_ant_zero,i, b[0].hash_ant);
			}
			else {
				//printf(ANSI_COLOR_RED);
				//printOne(b, i, size);
				//printf(ANSI_COLOR_RESET);

				printf(ANSI_COLOR_RED"\n\t\t%s\n"ANSI_COLOR_RESET"Bloco %i:"ANSI_COLOR_RED"\t%s" ANSI_COLOR_RESET,hash_ant_zero,i, b[0].hash_ant);
				printf("  Corrompido!\n");
				
			} 
		}
		else {
			//printf("\n%i",memcmp(b[i-1].hash, b[i].hash_ant, 32));
			if (memcmp(b[i-1].hash, b[i].hash_ant, 32) == 0){
				//printf(ANSI_COLOR_GREEN);
				//printOne(b, i, size);
				//printf(ANSI_COLOR_RESET);

				printf(ANSI_COLOR_GREEN"\n\t\t%s"ANSI_COLOR_RESET"Bloco %i:"ANSI_COLOR_GREEN"\t%s\n" ANSI_COLOR_RESET, b[i-1].hash,i, b[i].hash_ant);
			}
			else {
				//printf(ANSI_COLOR_RED);
				//printOne(b, i, size);
				//printf(ANSI_COLOR_RESET);

				printf(ANSI_COLOR_RED"\n\t\t%s"ANSI_COLOR_RESET"Bloco %i:"ANSI_COLOR_RED"\t%s" ANSI_COLOR_RESET, b[i-1].hash,i, b[i].hash_ant);
				printf("  Corrompido!\n");
		
			} 
		}	
	} 
}


void verificaTudo_noPrint(Bloco *b, unsigned int size){
	unsigned int i ;
	for (i = 0; i < size; i++){
		if (i == 0){
			if (strcmp(hash_ant_zero, b[0].hash_ant) == 0){
			}
			else {
				printf(ANSI_COLOR_RED"\nBloco %i Corrompido!\n"ANSI_COLOR_RESET, i);
			} 
		}
		else {
			
			if (memcmp(b[i-1].hash, b[i].hash_ant, 32) == 0){
			}
			else {
				printf(ANSI_COLOR_RED"\nBloco %i Corrompido!\n"ANSI_COLOR_RESET, i);
			} 
		}	
	} 
}




/*FUNÇÃO QUE MINERA NOVAMENTE TODA A LISTA E VERIFICA DIVERGÊNCIAS NOS HASHES INDICANDO CORRUPÇÃO DOS DADOS*/


//                              <<FAZER>>



/*opções de correção de erros*/
//refazer e minerar (hack block) a lista de blocos a partir de novos elementos no arquivo blocos


//								<<FAZER>>
