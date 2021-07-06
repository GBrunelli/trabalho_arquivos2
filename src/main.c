// #include "car_routines.h"
#include "line_routines.h"
#include "car_routines.h"

int main(void)
{
    // Checking which routine should we execute
    int n_routine = 0;
    scanf("%d", &n_routine);

    // Switching for correct routine
    switch (n_routine)
    {
    case 9:
        break;
    case 10:
        break;
        /*
        Entrada do programa para a funcionalidade [9] e [10]: 
        9 veiculo.bin indicePrefixo.bin
        */

    case 11:
        break;
    case 12:
        break;
        /*
        Sintaxe do comando para a funcionalidade [11] e [12]: 
        11 veiculo.bin indicePrefixo.bin prefixo "valor"
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
        */


    default:
        break;
    }

    return EXIT_SUCCESS;
}