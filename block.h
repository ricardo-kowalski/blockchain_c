typedef struct bloco Bloco;

Bloco txtToStruct (char* str);

void printUntil(Bloco *b,unsigned int size);

int menuInit();

Bloco *genesis(void);

Bloco *newBlock( int num, char hash[33]);

void printOne(Bloco *b,unsigned int entr, unsigned int size);

void imprBloco(Bloco *b);

char toString(char *str, Bloco *b);

void verificaTudo(Bloco *b, unsigned int size);

void verificaTudo_noPrint(Bloco *b, unsigned int size);

