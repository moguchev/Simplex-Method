#include "Simplex.hpp"


int main()
{
    setlocale(LC_ALL, "Russian");
    Simplex *simplex_method = new Simplex();
    // simplex_method->init();
    simplex_method->init_my_variant();
    simplex_method->calculate();
    simplex_method->result();
    delete simplex_method;
    return 0;
}
