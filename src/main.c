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
    case 11:
        searchIndexedCars();
        break;
    case 12:
        searchIndexedLines();
        break;
    case 13:
        insertAndIndexCars();
        break;
    case 14:
        insertAndIndexLines();
        break;
    }

    return EXIT_SUCCESS;
}