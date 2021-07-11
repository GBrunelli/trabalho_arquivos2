#include "project.h"

#include "index.h"
#include "search.h"
#include "insert.h"


int main(void)
{
    // Checking which routine should we execute
    int n_routine = 0;
    scanf("%d", &n_routine);

    // Switching for correct routine
    switch (n_routine)
    {
    case 9:
        indexCars();
        break;
    case 10:
        indexLines();
        break;
    case 11:
        searchCar();
        break;
    case 12:
        searchLine();
        break;
    case 13:
        insertCars();
        break;
    case 14:
        insertLines();
        break;
    }

    return EXIT_SUCCESS;
}