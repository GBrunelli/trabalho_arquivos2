#include "project.h"
#include "utils.h"

void openFiles(FILE **bin, char *binFileName, char *binMode, FILE **index, char *indexFileName, char *indexMode, bool (*integrityChecker)(FILE *))
{
    *bin = fopen(binFileName, binMode);
    if (*bin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    *index = fopen(indexFileName, indexMode);
    if (*index == NULL)
    {
        fclose(*bin);
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    // Checking file integrity
    char c = 0;
    fread(&c, 1, 1, *index);
    if (integrityChecker(*bin) || (c == '0'))
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(*bin);
        fclose(*index);
        exit(0);
    }
}

// Get the month member given its number
void getMonthName(char *monthName, int month)
{
    switch (month)
    {
    case 1:
        strcpy(monthName, "janeiro");
        break;
    case 2:
        strcpy(monthName, "fevereiro");
        break;
    case 3:
        strcpy(monthName, "março");
        break;
    case 4:
        strcpy(monthName, "abril");
        break;
    case 5:
        strcpy(monthName, "maio");
        break;
    case 6:
        strcpy(monthName, "junho");
        break;
    case 7:
        strcpy(monthName, "julho");
        break;
    case 8:
        strcpy(monthName, "agosto");
        break;
    case 9:
        strcpy(monthName, "setembro");
        break;
    case 10:
        strcpy(monthName, "outubro");
        break;
    case 11:
        strcpy(monthName, "novembro");
        break;
    case 12:
        strcpy(monthName, "dezembro");
        break;
    default:
        break;
    }
}

// Transform a date of format "YYYY-MM-DD" to "_DAY de _MONTH_NAME_ de _YEAR_".
// Example: "2010-05-21" -> "21 de maio de 2021".
void tranformDate(char *date)
{
    // verify iF the string is null
    if (*date == 0)
    {
        strcpy(date, NULL_MESSAGE);
        return;
    }

    int day, month, year;
    char *buffer = calloc(MAX_STRING_SIZE, 1);
    char *bufferPointer = buffer;
    strcpy(buffer, date);

    // get the year
    char *token;
    token = strsep(&buffer, "-");
    sscanf(token, "%d", &year);

    // get the month
    token = strsep(&buffer, "-");
    sscanf(token, "%d", &month);

    // get the day
    token = strsep(&buffer, "-");
    sscanf(token, "%d", &day);

    free(bufferPointer);

    // get the month name
    char monthName[15];
    getMonthName(monthName, month);

    // create the new date format
    sprintf(date, "%02d de %s de %d", day, monthName, year);
    return;
}

int convertePrefixo(char *str)
{

    /* O registro que tem essa string como chave foi removido */
    if (str[0] == '*')
        return -1;

    /* Começamos com o primeiro digito na ordem de 36^0 = 1 */
    int power = 1;

    /* Faz a conversão char por char para chegar ao resultado */
    int result = 0;
    for (int i = 0; i < 5; i++)
    {

        /* 
            Interpreta o char atual como se fosse um digito
            em base 36. Os digitos da base 36 são:
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D,
            E, F, G, H, I, J, K, L, M, N, O, P, Q, R,
            S, T, U, V, W, X, Y, Z
        */
        int cur_digit = 0;
        /* Checa pelos digitos normais e os converte para números */
        if (str[i] >= '0' && str[i] <= '9')
            cur_digit = str[i] - '0';
        /* Checa pelas letras e as converte para números */
        else if (str[i] >= 'A' && str[i] <= 'Z')
            cur_digit = 10 + str[i] - 'A';

        /*
            Multiplica o digito atual pelo ordem da posição atual
            e adiciona no resultado
            Primeira posição:   36^0 = 1
            Segunda posição:    36^1 = 36
            Terceira posição:   36^2 = 1.296
            Quarta posição:     36^3 = 46.656
            Quinta posição:     36^4 = 1.679.616
        */
        result += cur_digit * power;

        /* Aumenta a ordem atual */
        power *= 36;
    }

    return result;
}

void removeQuotations(char *str)
{
    // Checking whether string has quotation marks
    if (str[0] != '"')
        return;

    int len = strlen(str);
    for (int i = 1; i < len; i++)
    {
        str[i - 1] = str[i];
    }

    str[len - 2] = '\0';
}

void leftShift(char *string, int len)
{
    int i;
    for (i = 1; i < len; i++)
    {
        string[i - 1] = string[i];
    }
    string[len - 1] = 0;
}

int isNULO(char *string)
{
    if (!strcmp(string, "NULO"))
    {
        return 1;
    }
    return 0;
}

void fillWithGarbage(char *string, int len)
{
    string[0] = 0;
    for (int i = 1; i < len; i++)
    {
        string[i] = '@';
    }
}

void binarioNaTela(char *nomeArquivoBinario)
{ /* Você não precisa entender o código dessa função. */

    /* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
    {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

void scan_quote_string(char *str)
{

    /*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}

/* ---------------- EXTRA ----------------

OPCIONAL: dicas sobre scanf() e fscanf():

scanf("%[^,]", string) -> lê até encontrar o caractere ',', não incluindo o mesmo na leitura.

Exemplo de entrada: "Oi, esse é um exemplo."
Nesse caso, o scanf("%[^,]") tem como resultado a string "Oi";

scanf("%[^\"]", string) -> lê até encontrar o caractere '"', não incluindo o mesmo na leitura.
scanf("%[^\n]", string) -> lê até encontrar o fim da linha, não incluindo o '\n' na leitura.

scanf("%*c") --> lê um char e não guarda em nenhuma variável, como se tivesse ignorado ele

*/
