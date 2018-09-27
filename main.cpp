#include "Simplex.h"


int main()
{
    setlocale(LC_ALL, "Russian");
    Simplex *simplex_method = new Simplex();
    simplex_method->init();
    simplex_method->calculate();
    simplex_method->result();
    system("pause");
    return 0;
}