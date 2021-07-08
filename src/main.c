// #include "car_routines.h"
#include "line_routines.h"
#include "car_routines.h"
#include "project.h"

int main(void)
{
    // Checking which routine should we execute
    int n_routine = 0;
    scanf("%d", &n_routine);

    // Switching for correct routine
    switch (n_routine)
    {
    case 9:
        generateIndexCars();
        break;
    case 10:
        generateIndexLines();
        break;
        /*
        Entrada do programa para a funcionalidade [9] e [10]:

        9 veiculo.bin indicePrefixo.bin
        ou
        10 linha.bin indiceCodLinha.bin
        */


    case 11:
        searchIndexedCars();
        break;
    case 12:
        searchIndexedLines();
        break;
        /*
        Sintaxe do comando para a funcionalidade [11] e [12]: 

        11 veiculo.bin indicePrefixo.bin prefixo "valor"
        ou
        12 linha.bin indiceCodLinha.bin codLinha valor
        */


    case 13:
        break;
    case 14:
        break;
        /*
        Entrada do programa para a funcionalidade [13] e [14]: 

        13 veiculo.bin indicePrefixo.bin n 
        prefixo1 data1 quantidadeLugares1 codLinha1 modelo1 categoria1
        prefixo2 data2 quantidadeLugares2 codLinha2 modelo2 categoria2
        ... 
        ou
        14 arquivoLinha.bin indiceCodLinha.bin n 
        codLinha1 aceitaCartao1 nomeLinha1 corLinha1
        codLinha2 aceitaCartao2 nomeLinha2 corLinha2
        ... 
        codLinhan aceitaCartaon nomeLinhan corLinhan
        */
    }

    return EXIT_SUCCESS;
}